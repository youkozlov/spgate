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

    int connect() final;

    int send() final;

    int receive() final;

    unsigned int period() final;

    void reset() final;

private:
    bool request(DataRequest const&);
    DataRespond respond();

    GateParams const& gateParams;
    ModbusBuffer&     regs;
};

}