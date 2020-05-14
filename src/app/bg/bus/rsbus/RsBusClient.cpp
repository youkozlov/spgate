#include "RsBusClient.hpp"
#include "RsBusCodec.hpp"

#include "types/ParamsDefs.hpp"

#include "BusStats.hpp"
#include "RegAccessor.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Logger.hpp"
#include "utils/Utils.hpp"

#include "sockets/LinkRl.hpp"

#include <stdexcept>

namespace sg
{

namespace rsbus
{

RsBusClient::RsBusClient(Init const& init)
    : gateParams(init.gateParams)
    , storage(init.parser)
    , regs(init.regs)
    , stats(init.stats)
    , fsm(*this)
    , link(std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr)))
    , rx(*link)
    , currentParamId(0)
{
    if (!storage.configure(gateParams))
    {
        throw std::runtime_error("Configuration is invalid");
    }
}

RsBusClient::~RsBusClient()
{
}

void RsBusClient::tickInd()
{
    fsm.tickInd();
}

int RsBusClient::connect()
{
    return link->connect();
}

int RsBusClient::sendStartSequence()
{
    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    txBuf.write(0xFF);
    stats.nTx += 1;
    return link->write(txBuf.cbegin(), txBuf.size());
}

int RsBusClient::sendSessionReq()
{
    auto& item   = getNext();
    auto& device = item.device;

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());

    RsBusFrame frame;
    frame.nt      = device.addr;
    frame.rc      = SRC;
    frame.data[0] = 0x00;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;
    frame.data[3] = 0x00;
    frame.qty     = 4;

    RsBusCodec codec(txBuf, frame);
    if (!codec.encode())
    {
        return -1;
    }
    stats.nTx += 1;
    return link->write(txBuf.cbegin(), txBuf.size());
}

int RsBusClient::recvSessionRsp()
{
    int len = rx.receive(&rawBuffer[0], rawBuffer.size());

    if (!len)
    {
        return len;
    }
    else if (len == RsBusRx::invalid)
    {
        stats.nInvalid += 1;
        return 0;
    }
    else if (len < 0)
    {
        stats.nError += 1;
        return len;
    }

    stats.nRx += 1;

    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decode())
    {
        stats.nInvalid += 1;
        return 0;
    }

    if (frame.rc != SRC)
    {
        LM(LE, "Unexpected function code=%02X, data[0]=%02X", frame.rc, frame.data[0]);
        return 0;
    }

    if (frame.qty != 3)
    {
        LM(LE, "Unexpected session respond length: %u", frame.qty);
        stats.nInvalid += 1;
        return 0;
    }

    uint16_t const deviceType = (frame.data[0] << 8) | frame.data[1];

    switch (deviceType)
    {
    case DeviceType::type741:
    case DeviceType::type742:
    case DeviceType::type743:
    case DeviceType::type941:
    case DeviceType::type942:
    case DeviceType::type943:
    break;
    default:
    {
        LM(LE, "Unsupported device type %02X:%02X", frame.data[0], frame.data[1]);
        return 0;
    }
    }
    return len;
}

int RsBusClient::sendDataReq()
{
    auto& item   = getCurrent();
    auto& device = item.device;
    auto& prms   = item.prms;

    if (prms.func != RDP)
    {
        LM(LE, "Unsupported function code=%02X", prms.func);
        return -1;
    }

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    RsBusFrame frame;
    frame.nt      = device.addr;
    frame.rc      = prms.func;
    frame.data[0] = prms.addr;
    frame.data[1] = prms.addr >> 8;
    frame.data[2] = sizeof(float);
    frame.data[3] = 0x00;
    frame.qty     = 4;

    RsBusCodec codec(txBuf, frame);
    if (!codec.encode())
    {
        return -1;
    }

    stats.nRdp += 1;
    stats.nTx  += 1;
    return link->write(txBuf.cbegin(), txBuf.size());
}

int RsBusClient::recvDataRsp()
{
    auto& item = getCurrent();

    int len = rx.receive(&rawBuffer[0], rawBuffer.size());

    if (!len)
    {
        return len;
    }
    else if (len == RsBusRx::invalid)
    {
        stats.nInvalid += 1;
        return 0;
    }
    else if (len < 0)
    {
        stats.nError += 1;
        return len;
    }

    stats.nRx += 1;

    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decode())
    {
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
        return len;
    }

    if (frame.rc != RDP)
    {
        LM(LE, "Unexpected function code=%02X, data[0]=%02X", frame.rc, frame.data[0]);
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
        return len;
    }

    if (frame.qty != sizeof(float))
    {
        LM(LE, "Unexpected length of respond: %u", frame.qty);
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
        return len;
    }

    regs.setStatus(item.prms.id, RegAccessor::ready);

    float rsbusFloat{};
    memcpy(&rsbusFloat, frame.data, sizeof(float));
    float const valFloat = Utils::decodeRsBus(rsbusFloat);
    float const netFloat = Utils::reverse(valFloat);

    regs.setValue(item.prms.id, netFloat);

    stats.nRsp += 1;

    getNext();

    return len;
}

unsigned int RsBusClient::period() const
{
    return gateParams.readPeriod;
}

void RsBusClient::reset()
{
    for (unsigned i = 0; i < storage.getNumItems(); ++i)
    {
        auto& item = storage.getItem(i);
        regs.setStatus(item.prms.id, RegAccessor::timeout);
    }
    currentParamId = 0;
    link->close();
}

void RsBusClient::timeout()
{
    auto& item = getCurrent();
    regs.setStatus(item.prms.id, RegAccessor::timeout);
    stats.nTimeout += 1;
}

GateReadItem const& RsBusClient::getNext()
{
    return storage.getItem((++currentParamId) % storage.getNumItems());
}

GateReadItem const& RsBusClient::getCurrent() const
{
    return storage.getItem(currentParamId % storage.getNumItems());
}

}
}