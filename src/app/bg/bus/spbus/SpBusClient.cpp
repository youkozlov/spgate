#include "SpBusClient.hpp"
#include "sockets/LinkRl.hpp"
#include "types/ParamsDefs.hpp"

#include "RegAccessor.hpp"
#include "BusStats.hpp"
#include "SpBusDefs.hpp"
#include "SpBusCodec.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"

#include <stdexcept>

namespace sg
{

SpBusClient::SpBusClient(Init const& init)
    : gateParams(init.gateParams)
    , storage(init.parser)
    , regs(init.regs)
    , stats(init.stats)
    , fsm(*this, gateParams.readTimeout)
    , link(std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr)))
    , rx(*link)
    , linkLocker(init.linkLocker)
    , currentParamId(0)
{
    if (!storage.configure(gateParams))
    {
        throw std::runtime_error("Configuration is invalid");
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
    auto& item   = getCurrent();
    auto& device = item.device;
    auto& prms   = item.prms;

    SpBusFrame frame;

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

    stats.nRdp += 1;
    stats.nTx  += 1;

    LM(LI, "Process paramIdx=%u/%u send dad=%u sad=%u ch=%u addr=%u"
        , currentParamId
        , storage.getNumItems()
        , frame.hdr.dad
        , frame.hdr.sad
        , prms.chan
        , prms.addr);

    return link->write(txBuf.cbegin(), txBuf.size());
}

int SpBusClient::receiveFrame(SpBusFrame& frame)
{
    int len = rx.receive(&rawBuffer[0], rawBuffer.size());

    if (!len)
    {
        return len;
    }
    else if (len == SpBusRx::invalid)
    {
        stats.nInvalid += 1;
        return 0;
    }
    else if (len < 0)
    {
        return len;
    }

    WrapBuffer rxBuf(&rawBuffer[0], len);

    if (Utils::crcode(rxBuf.cbegin() + 2, len - 2))
    {
        stats.nInvalid += 1;
        return 0;
    }

    SpBusCodec codec(rxBuf, frame);

    if (!codec.decode())
    {
        stats.nInvalid += 1;
        return 0;
    }

    if (frame.hdr.fc != RSP)
    {
        stats.nInvalid += 1;
        LM(LE, "Unexpected function=%02X", frame.hdr.fc);
        return 0;
    }

    stats.nRx += 1;

    return len;
}

Client::Result SpBusClient::receive()
{
    auto& item = getCurrent();
    auto& prms = item.prms;

    float   floatValue;
    int32_t fixedValue;

    SpBusFrame frame;

    int len = receiveFrame(frame);

    if (!len)
    {
        return Result::waitForData;
    }
    else if (len < 0)
    {
        stats.nError += 1;
        return Result::fail;
    }
    else if (!frame.data.numInfos)
    {
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }
    else if (prms.type == ParamType::floatPoint
            && sscanf(frame.data.infos[0].value.param, "%f", &floatValue) == 1)
    {
        float const netFloat = Utils::reverse(floatValue);
        regs.setValue(item.prms.id, netFloat);
        stats.nRsp += 1;
    }
    else if (prms.type == ParamType::fixedPoint
            && sscanf(frame.data.infos[0].value.param, "%d", &fixedValue) == 1)
    {
        int32_t const netFixed = Utils::reverse(fixedValue);
        regs.setValue(item.prms.id, netFixed);
        stats.nRsp += 1;
    }
    else if (prms.type == ParamType::real
            && sscanf(frame.data.infos[0].value.param, "%f", &floatValue) == 1)
    {
        regs.setValue(item.prms.id, floatValue);
        stats.nRsp += 1;
    }
    else
    {
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }

    LM(LI, "Process paramIdx=%u/%u receive dad=%u sad=%u ch=%u addr=%u value=%s"
        , currentParamId
        , storage.getNumItems()
        , frame.hdr.dad
        , frame.hdr.sad
        , prms.chan
        , prms.addr
        , frame.data.infos[0].value.param);

    currentParamId += 1;
    if (currentParamId == storage.getNumItems())
    {
        currentParamId = 0;
        LM(LI, "All params processed");
        return Result::done;
    }
    return Result::progress;
}

void SpBusClient::disconnect()
{
    linkLocker.unlock();
    link->close();
}

unsigned int SpBusClient::period() const
{
    return gateParams.readPeriod;
}

void SpBusClient::reset()
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

Client::Result SpBusClient::timeout()
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

bool SpBusClient::tryLock()
{
    return linkLocker.tryLock();
}

GateReadItem const& SpBusClient::getCurrent() const
{
    return storage.getItem(currentParamId % storage.getNumItems());
}

}