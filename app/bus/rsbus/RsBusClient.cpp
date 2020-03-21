#include "RsBusClient.hpp"
#include "RsBusCodec.hpp"

#include "types/ParamsDefs.hpp"

#include "BusStats.hpp"
#include "RegAccessor.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Logger.hpp"

#include "sockets/LinkRl.hpp"

namespace sg
{

RsBusClient::RsBusClient(Init const& init)
    : gateParams(init.gateParams)
    , storage(init.parser)
    , regs(init.regs)
    , stats(init.stats)
    , fsm(*this)
    , link(std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr)))
    , rx(*link)
{
    if (!storage.configure(gateParams))
    {
        throw("Configuration is invalid");
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
    RsBusFrame frame{};
    RsBusCodec codec(txBuf, frame);
    if (!codec.encodeStartSequence())
    {
        return -1;
    }

    stats.nTx += 1;
    return link->write(txBuf.cbegin(), txBuf.size());
}

int RsBusClient::sendSessionReq()
{
    auto& item   = getNext();
    auto& device = item.device;

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());

    RsBusFrame frame{};
    frame.nt = device.addr;

    RsBusCodec codec(txBuf, frame);
    if (!codec.encodeSessionReq())
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

    RsBusFrame frame{};

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decodeSessionRsp())
    {
        stats.nInvalid += 1;
        return 0;
    }

    if (frame.rc != 0x3F || frame.data[0] != 0x54
     || frame.data[1] != 0x29)
    {
        LM(LE, "Invalid content of session respond: %02X:%02X:%02X:%02X:%02X"
            , frame.nt
            , frame.rc
            , frame.data[0]
            , frame.data[1]
            , frame.data[2]);
        return 0;
    }
    return len;
}

int RsBusClient::sendDataReq()
{
    auto& item   = getCurrent();
    auto& device = item.device;
    auto& prms   = item.prms;

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    RsBusFrame frame{};
    frame.nt  = device.addr;
    frame.rc  = prms.func;
    frame.a1  = prms.addr >> 8;
    frame.a0  = prms.addr;
    frame.qty = sizeof(float);

    RsBusCodec codec(txBuf, frame);
    if (!codec.encodeDataReq())
    {
        return -1;
    }

    stats.nRdp += 1;

    stats.nTx += 1;
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

    RsBusFrame frame{};

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decodeDataRsp())
    {
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
        return len;
    }

    regs.setStatus(item.prms.id, RegAccessor::ready);

    // float const netFloat = Utils::reverse(floatValue);
    // memcpy(&valReg, &netFloat, sizeof(netFloat));

    LM(LE, "Invalid content of session respond: nt=%02X rc=%02X  %02X:%02X:%02X:%02X"
        , frame.nt
        , frame.rc
        , frame.data[0]
        , frame.data[1]
        , frame.data[2]
        , frame.data[3]);

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