#pragma once

#include <memory>
#include <array>

#include "Bus.hpp"
#include "sm/Client.hpp"
#include "sm/ClientFsm.hpp"

#include "GateStorage.hpp"
#include "SpBusRx.hpp"

namespace sg
{

class Link;
class ParamParser;
class RegAccessor;
struct GateParams;
struct SpBusFrame;
struct GateReadItemResult;
struct BusStats;

class SpBusClient : public Bus, public Client
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        RegAccessor&       regs;
        BusStats&          stats;
    };

    explicit SpBusClient(Init const&);

    ~SpBusClient();

    void tickInd() final;

    int connect() final;

    int send() final;

    Result receive() final;

    void disconnect() final;

    unsigned int period() const final;

    void reset() final;

    Result timeout() final;

private:

    GateReadItem const& getCurrent() const;
    int                 receiveFrame(SpBusFrame&);

    GateParams const&     gateParams;
    GateStorage           storage;
    RegAccessor&          regs;
    BusStats&             stats;
    ClientFsm             fsm;
    std::unique_ptr<Link> link;
    SpBusRx               rx;

    std::array<unsigned char, 256> rawBuffer;
    unsigned int                   currentParamId;
};

}