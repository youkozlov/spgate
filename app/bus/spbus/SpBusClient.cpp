#include "SpBusClient.hpp"
#include "sockets/LinkRl.hpp"
#include "types/ParamsDefs.hpp"

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
    auto& currentParam = getCurrent();
    auto& prms = currentParam.prms;

    float   floatValue;
    int32_t fixedValue;

    constexpr int regsPerParam = 4;
    auto& valReg = regs[prms.id * regsPerParam];
    auto& valSt  = regs[prms.id * regsPerParam + 2];

    SpBusFrame frame{};

    int len = receiveFrame(frame);

    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        valSt = GateReadItemResult::timeout;
    }
    else if (!frame.data.numInfos)
    {
        valSt = GateReadItemResult::invalid;
    }
    else if (prms.type == ParamType::floatPoint
            && sscanf(frame.data.infos[0].value.param, "%f", &floatValue) == 1)
    {
        valSt = GateReadItemResult::ready;

        float const netFloat = Utils::reverse(floatValue);
        memcpy(&valReg, &netFloat, sizeof(netFloat));
    }
    else if (prms.type == ParamType::fixedPoint
            && sscanf(frame.data.infos[0].value.param, "%d", &fixedValue) == 1)
    {
        valSt = GateReadItemResult::ready;

        int32_t const netFixed = Utils::reverse(fixedValue);
        memcpy(&valReg, &netFixed, sizeof(netFixed));
    }
    else
    {
        valSt = GateReadItemResult::invalid;
    }

    return len;
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