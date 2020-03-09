#pragma once

#include <memory>

#include "types/DataRequest.hpp"
#include "types/DataRespond.hpp"

#include "interfaces/Gate.hpp"

#include "utils/Buffer.hpp"
#include "GateStorage.hpp"

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

private:

    bool request(DataRequest const&);
    DataRespond respond();

    GateParams const&     gateParams;
    GateStorage           storage;
    ModbusBuffer&         regs;
    std::unique_ptr<Link> link;
    Buffer                buf;
};

}