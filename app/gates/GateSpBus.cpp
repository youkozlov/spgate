#include "GateSpBus.hpp"
#include "sockets/LinkRl.hpp"
#include "types/ParamsDefs.hpp"
#include "modbus/ModbusBuffer.hpp"

#include "GateDefs.hpp"
#include "SpBusCodec.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"

namespace sg
{

GateSpBus::GateSpBus(Init const& init)
    : gateParams(init.gateParams)
    , storage(init.parser)
    , regs(init.regs)
    , fsm(*this)
    , link(std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr)))
    , rx(*link)
    , currentParamId(0)
{
}

GateSpBus::~GateSpBus()
{
}

bool GateSpBus::configure()
{
    return storage.configure(gateParams);
}

void GateSpBus::tickInd()
{
    fsm.tickInd();
}

int GateSpBus::connect()
{
    return link->connect();
}

int GateSpBus::send()
{
    auto& item   = getNext();
    auto& device = item.device;
    auto& prms   = item.prms;

    SpBusFrame frame{};

    frame.hdr.dad = device.addr;
    frame.hdr.sad = gateParams.addr;
    frame.hdr.fc  = prms.func;

    frame.data.numPointers = 1;
    auto& ptr = frame.data.pointers[0];
    sprintf(ptr.chan.param, "%u", prms.chan);
    sprintf(ptr.prm.param, "%u", prms.addr);

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());

    SpBusCodec codec(txBuf, frame);
    if (!codec.encode())
    {
        return 1;
    }

    return link->write(txBuf.cbegin(), txBuf.size());
}

int GateSpBus::receiveFrame(SpBusFrame& frame)
{
    int len = rx.receive(&rawBuffer[0], rawBuffer.size());
    
    if (!len)
    {
        return 0;
    }
    else if (len < 0)
    {
        return len;
    }

    WrapBuffer rxBuf(&rawBuffer[0], len);

    if (Utils::crcode(rxBuf.cbegin() + 2, len - 2))
    {
        LM(LE, "CRC NOK");
        return -1;
    }

    SpBusCodec codec(rxBuf, frame);

    if (!codec.decode())
    {
        return -1;
    }

    if (frame.hdr.fc != RSP)
    {
        LM(LE, "Unexpected function=%02X", frame.hdr.fc);
        return -1;
    }

    return len;
}

int GateSpBus::receive()
{
    GateReadItemResult result{};

    SpBusFrame frame{};

    int len = receiveFrame(frame);

    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        result.st = GateReadItemResult::timeout;
    }
    else if (!frame.data.numInfos)
    {
        result.st = GateReadItemResult::invalid;
    }
    else if (sscanf(frame.data.infos[0].value.param, "%lf", &result.value) != 1)
    {
        result.st = GateReadItemResult::invalid;
    }
    else
    {
        result.st = GateReadItemResult::ready;
    }

    updateModbusRegs(result);

    return len;
}

void GateSpBus::updateModbusRegs(GateReadItemResult const& result)
{
    auto& item = getCurrent();
    auto& prms   = item.prms;

    auto& valReg = regs[prms.id * 8];
    memcpy(&valReg, &result.value, sizeof(result.value));
}

unsigned int GateSpBus::period()
{
    return gateParams.readPeriod;
}

void GateSpBus::reset()
{
    currentParamId = 0;
    link->close();
}

GateReadItem const& GateSpBus::getNext()
{
    return storage.getItem((++currentParamId) % storage.getNumItems());
}

GateReadItem const& GateSpBus::getCurrent() const
{
    return storage.getItem(currentParamId % storage.getNumItems());
}

}