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

namespace sg
{

SpBusClient::SpBusClient(Init const& init)
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

    stats.nRdp += 1;

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

    return len;
}

int SpBusClient::receive()
{
    auto& item = getCurrent();
    auto& prms = item.prms;

    float   floatValue;
    int32_t fixedValue;

    SpBusFrame frame{};

    int len = receiveFrame(frame);

    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        stats.nError += 1;
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
    else
    {
        regs.setStatus(item.prms.id, RegAccessor::invalid);
        stats.nInvalid += 1;
    }

    return len;
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
    link->close();
}

void SpBusClient::timeout()
{
    auto& item = getCurrent();
    regs.setStatus(item.prms.id, RegAccessor::timeout);
    stats.nTimeout += 1;
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