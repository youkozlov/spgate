#pragma once

#include "Bus.hpp"
#include "RsBus.hpp"
#include "RsBusFsm.hpp"
#include "RsBusRx.hpp"

#include "GateStorage.hpp"

#include <memory>
#include <array>

namespace sg
{

class ParamParser;
class Link;
class RegAccessor;
struct GateParams;
struct BusStats;

class RsBusClient : public Bus, public RsBus
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        RegAccessor&       regs;
        BusStats&          stats;
    };
    
    explicit RsBusClient(Init const&);

    ~RsBusClient();

    void tickInd() final;

    int connect() final;

    int sendStartSequence() final;

    int sendSessionReq() final;

    int recvSessionRsp() final;

    int sendDataReq() final;

    int recvDataRsp() final;

    unsigned int period() const final;

    void reset() final;

    void timeout() final;

private:

    GateReadItem const& getNext();
    GateReadItem const& getCurrent() const;

    GateParams const&     gateParams;
    GateStorage           storage;
    RegAccessor&          regs;
    BusStats&             stats;
    RsBusFsm              fsm;
    std::unique_ptr<Link> link;
    RsBusRx               rx;

    std::array<unsigned char, 256> rawBuffer;
    unsigned int                   currentParamId;
};

}