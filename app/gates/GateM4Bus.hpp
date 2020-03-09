#pragma once

#include "interfaces/Gate.hpp"
#include "types/DataRequest.hpp"
#include "types/DataRespond.hpp"

namespace sg
{

class ParamParser;
class ModbusBuffer;
struct GateParams;

class GateM4Bus : public Gate
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        ModbusBuffer&      regs;
    };
    
    explicit GateM4Bus(Init const&);

    ~GateM4Bus();

    bool configure() final;

    void tickInd() final;

private:
    bool request(DataRequest const&);
    DataRespond respond();

    GateParams const& gateParams;
    ModbusBuffer&     regs;
};

}