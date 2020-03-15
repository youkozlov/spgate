#pragma once

#include <memory>
#include <array>

#include "sm/Client.hpp"
#include "sm/ClientFsm.hpp"

#include "GateStorage.hpp"
#include "SpBusRx.hpp"

namespace sg
{

class Link;
class ParamParser;
class ModbusBuffer;
struct GateParams;
struct SpBusFrame;
struct GateReadItemResult;

class SpBusClient : public Client
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        ModbusBuffer&      regs;
    };

    explicit SpBusClient(Init const&);

    ~SpBusClient();

    void tickInd() final;

    int connect() final;

    int send() final;

    int receive() final;

    unsigned int period() final;

    void reset() final;

private:
    
    GateReadItem const& getNext();
    GateReadItem const& getCurrent() const;
    int                 receiveFrame(SpBusFrame&);

    GateParams const&     gateParams;
    GateStorage           storage;
    ModbusBuffer&         regs;
    ClientFsm             fsm;
    std::unique_ptr<Link> link;
    SpBusRx               rx;

    std::array<unsigned char, 256> rawBuffer;
    unsigned int                   currentParamId;
};

}