#pragma once

#include "sm/Client.hpp"

#include "utils/Buffer.hpp"

namespace sg
{

class ParamParser;
class ModbusBuffer;
struct GateParams;

class RsBusClient : public Client
{
public:
    struct Init
    {
        GateParams const&  gateParams;
        ParamParser const& parser;
        Buffer<uint16_t>&  regs;
    };
    
    explicit RsBusClient(Init const&);

    ~RsBusClient();

    void tickInd() final;

    int connect() final;

    int send() final;

    int receive() final;

    unsigned int period() final;

    void reset() final;

private:

    GateParams const& gateParams;
    Buffer<uint16_t>& regs;
};

}