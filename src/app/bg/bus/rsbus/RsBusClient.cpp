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
    , fsm(*this, gateParams.readTimeout)
    , link(std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr)))
    , rx(*link)
    , linkLocker(init.linkLocker)
    , currentParamId(0)
    , deviceType(0xffff)
    , msgId(0)
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
    auto& item   = getCurrent();
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

    deviceType = (frame.data[0] << 8) | frame.data[1];

    switch (deviceType)
    {
    case DeviceType::type741:
    case DeviceType::type742:
    case DeviceType::type743:
    case DeviceType::type9411:
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
    switch (deviceType)
    {
    case DeviceType::type741:
    case DeviceType::type941:
        return sendShortDataReq();
    case DeviceType::type742:
    case DeviceType::type743:
    case DeviceType::type9411:
    case DeviceType::type942:
    case DeviceType::type943:
        return sendLongDataReq();
    default:
    {
        LM(LE, "Unsupported device");
        return -1;
    }
    }    
}

int RsBusClient::sendShortDataReq()
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

int RsBusClient::sendLongDataReq()
{
    auto& item   = getCurrent();
    auto& device = item.device;
    auto& prms   = item.prms;

    if (prms.func != RM4)
    {
        LM(LE, "Unsupported function code=%02X", prms.func);
        return -1;
    }

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    RsBusFrame frame;
    frame.nt  = device.addr;
    frame.rc  = prms.func;
    frame.id  = msgId;
    frame.atr = ATR;
    frame.tag = TAG_PNUM;
    frame.ch  = prms.chan;
    frame.prm = prms.addr;

    RsBusCodec codec(txBuf, frame);
    if (!codec.encodeReqLong())
    {
        return -1;
    }

    stats.nRdp += 1;
    stats.nTx  += 1;
    return link->write(txBuf.cbegin(), txBuf.size());
}

RsBus::Result RsBusClient::recvDataRsp()
{
    switch (deviceType)
    {
    case DeviceType::type741:
    case DeviceType::type941:
        return recvShortDataRsp();
    case DeviceType::type742:
    case DeviceType::type743:
    case DeviceType::type9411:
    case DeviceType::type942:
    case DeviceType::type943:
        return recvLongDataRsp();
    }    
}

RsBus::Result RsBusClient::recvShortDataRsp()
{
    auto& item = getCurrent();

    int len = rx.receive(&rawBuffer[0], rawBuffer.size());

    if (!len)
    {
        return Result::waitForData;
    }
    else if (len == RsBusRx::invalid)
    {
        stats.nInvalid += 1;
        return Result::fail;
    }
    else if (len < 0)
    {
        stats.nError += 1;
        return Result::fail;
    }

    stats.nRx += 1;

    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (codec.decode() && frame.rc == RDP && frame.qty == sizeof(float))
    {
        regs.setStatus(item.prms.id, RegAccessor::ready);
        float rsbusFloat{};
        memcpy(&rsbusFloat, frame.data, sizeof(float));
        float const valFloat = Utils::decodeRsBus(rsbusFloat);
        float const netFloat = (item.prms.type == ParamType::real) ? valFloat : Utils::reverse(valFloat);
        regs.setValue(item.prms.id, netFloat);
        stats.nRsp += 1;
    }
    else if (frame.rc != RDP)
    {
        LM(LE, "Unexpected function code=%02X", frame.rc);
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }
    else if (frame.qty != sizeof(float))
    {
        LM(LE, "Unexpected qty: %u", frame.qty);
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }
    else
    {
        LM(LE, "Unable to decode");
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }

    currentParamId += 1;
    if (currentParamId == storage.getNumItems())
    {
        currentParamId = 0;
        LM(LI, "All params processed");
        return Result::done;
    }
    return Result::progress;    
}

RsBus::Result RsBusClient::recvLongDataRsp()
{
    auto& item = getCurrent();

    int len = rx.receiveLong(&rawBuffer[0], rawBuffer.size());

    if (!len)
    {
        return Result::waitForData;
    }
    else if (len == RsBusRx::invalid)
    {
        stats.nInvalid += 1;
        return Result::fail;
    }
    else if (len < 0)
    {
        stats.nError += 1;
        return Result::fail;
    }

    stats.nRx += 1;

    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (codec.decodeRspLong() && frame.rc == RM4 && frame.tag == TAG_IEEFloat)
    {
        regs.setStatus(item.prms.id, RegAccessor::ready);
        float rsbusFloat{};
        memcpy(&rsbusFloat, frame.data, sizeof(float));
        float const netFloat = (item.prms.type == ParamType::real) ? rsbusFloat : Utils::reverse(rsbusFloat);
        regs.setValue(item.prms.id, netFloat);
        stats.nRsp += 1;
    }
    else if (frame.rc != RM4)
    {
        LM(LE, "Unexpected function code=%02X", frame.rc);
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }
    else if (frame.tag != TAG_IEEFloat)
    {
        LM(LE, "Unexpected tag of respond: %u", frame.tag);
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }
    else
    {
        LM(LE, "Unable to decode");
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }

    currentParamId += 1;
    if (currentParamId == storage.getNumItems())
    {
        currentParamId = 0;
        LM(LI, "All params processed");
        return Result::done;
    }
    return Result::progress;
}

void RsBusClient::disconnect()
{
    linkLocker.unlock();
    link->close();
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
    linkLocker.unlock();
    link->close();
}

RsBus::Result RsBusClient::timeout()
{
    LM(LI, "Process paramIdx=%u/%u timeout"
        , currentParamId
        , storage.getNumItems());

    auto& item = getCurrent();
    regs.setStatus(item.prms.id, RegAccessor::timeout);
    stats.nTimeout += 1;
    
    currentParamId += 1;
    if (currentParamId == storage.getNumItems())
    {
        currentParamId = 0;
        LM(LI, "All params processed");
        return Result::done;
    }
    return Result::progress;
}

bool RsBusClient::tryLock()
{
    return linkLocker.tryLock();
}

GateReadItem const& RsBusClient::getCurrent() const
{
    return storage.getItem(currentParamId % storage.getNumItems());
}

}
}
