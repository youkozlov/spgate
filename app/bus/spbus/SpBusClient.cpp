#include "SpBusClient.hpp"
#include "sockets/LinkRl.hpp"
#include "types/ParamsDefs.hpp"
#include "modbus/ModbusBuffer.hpp"

#include "GateDefs.hpp"
#include "SpBusDefs.hpp"
#include "SpBusCodec.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"

namespace sg
{

namespace
{

float ReverseFloat( const float inFloat )
{
   float retVal;
   char *floatToConvert = (char*) &inFloat;
   char *returnFloat    = (char*) &retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

}

SpBusClient::SpBusClient(Init const& init)
    : gateParams(init.gateParams)
    , storage(init.parser)
    , regs(init.regs)
    , fsm(*this)
    , link(std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr)))
    , rx(*link)
    , currentParamId(0)
{
    if (!storage.configure(gateParams))
    {
        throw("Configuration is invalid");
    }
}

SpBusClient::~SpBusClient()
{
}

void SpBusClient::tickInd()
{
    fsm.tickInd();
}

int SpBusClient::connect()
{
    return link->connect();
}

int SpBusClient::send()
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

int SpBusClient::receiveFrame(SpBusFrame& frame)
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

int SpBusClient::receive()
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
    else if (sscanf(frame.data.infos[0].value.param, "%f", &result.value) != 1)
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

void SpBusClient::updateModbusRegs(GateReadItemResult const& result)
{
    auto& item = getCurrent();
    auto& prms = item.prms;
    regs[prms.id * 4 + 2] = result.st;
    if (result.st == GateReadItemResult::ready)
    {
        auto& valReg = regs[prms.id * 4];
        float const nfloat = ReverseFloat(result.value);
        memcpy(&valReg, &nfloat, sizeof(nfloat));
    }
}

unsigned int SpBusClient::period()
{
    return gateParams.readPeriod;
}

void SpBusClient::reset()
{
    currentParamId = 0;
    link->close();
}

GateReadItem const& SpBusClient::getNext()
{
    return storage.getItem((++currentParamId) % storage.getNumItems());
}

GateReadItem const& SpBusClient::getCurrent() const
{
    return storage.getItem(currentParamId % storage.getNumItems());
}

}