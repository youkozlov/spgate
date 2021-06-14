#pragma once

#include "Bus.hpp"
#include "RsBus.hpp"
#include "RsBusFsm.hpp"
#include "RsBusRx.hpp"

#include "GateStorage.hpp"
#include "LinkLocker.hpp"

#include <memory>
#include <array>

namespace sg
{

class ParamParser;
class Link;
class RegAccessor;
struct GateParams;
struct BusStats;

namespace rsbus
{

class RsBusClient : public Bus, public RsBus
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        RegAccessor&       regs;
        BusStats&          stats;
        LinkLocker&        linkLocker;
    };
    
    explicit RsBusClient(Init const&);

    ~RsBusClient();

    void tickInd() final;

    int connect() final;

    void disconnect() final;
    
    int sendStartSequence() final;

    int sendSessionReq() final;

    int recvSessionRsp() final;

    int sendDataReq() final;

    Result recvDataRsp() final;

    unsigned int period() const final;

    void reset() final;

    Result timeout() final;

    bool tryLock() final;

private:
    int sendShortDataReq();
    Result recvShortDataRsp();
    int sendLongDataReq();
    Result recvLongDataRsp();

    GateReadItem const& getNext();
    GateReadItem const& getCurrent() const;

    GateParams const&     gateParams;
    GateStorage           storage;
    RegAccessor&          regs;
    BusStats&             stats;
    RsBusFsm              fsm;
    std::unique_ptr<Link> link;
    RsBusRx               rx;
    LinkLocker&           linkLocker;

    std::array<unsigned char, 256> rawBuffer;
    unsigned int                   currentParamId;
    unsigned                       deviceType;
    uint8_t                        msgId;
};

}
}