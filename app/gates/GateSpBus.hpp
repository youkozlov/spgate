#pragma once

#include <memory>
#include <array>

#include "types/DataRequest.hpp"
#include "types/DataRespond.hpp"

#include "interfaces/Gate.hpp"

#include "utils/Buffer.hpp"
#include "GateStorage.hpp"
#include "GateFsm.hpp"

namespace sg
{
 
class Link;
class ParamParser;
class ModbusBuffer;
struct GateParams;

class GateSpBus : public Gate
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        ModbusBuffer&      regs;
    };

    explicit GateSpBus(Init const&);

    ~GateSpBus();

    bool configure() final;

    void tickInd() final;

    int connect() final;

    int send() final;

    int receive() final;

    unsigned int period() final;

    void reset() final;

private:
    GateParams const&     gateParams;
    GateStorage           storage;
    ModbusBuffer&         regs;
    GateFsm               fsm;
    std::unique_ptr<Link> link;
    std::array<unsigned char, 128> rawBuffer;
    unsigned int          currentParamId;
};

}