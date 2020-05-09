#pragma once

#include <memory>
#include <array>

#include "utils/ParamParser.hpp"
#include "utils/Buffer.hpp"

#include "modbus/ModbusDefs.hpp"
#include "BusStats.hpp"
#include "RegAccessor.hpp"

namespace sg
{

class LinkAcceptorRl;
class ModbusServer;
class Bus;

namespace cli
{
class Cli;
}

enum class BusGateState
{
    init,
    run,
    error,
    shutdown
};

class BusGate
{
public:
    struct Init
    {
        char const* iniFileName;
    };
    
    explicit BusGate(Init const&);
    
    ~BusGate();
    
    int tickInd();

    void shutdown();

    BusGateState getState() const;

private:
    bool validateTags();
    void processInit();
    void processRun();
    void processError();
    void chageState(BusGateState);
    char const* toString(BusGateState) const;

    bool createCli();
    bool createModbus();
    bool createGates();

    char const*                     iniFileName;
    BusGateState                    state;
    ParamParser                     parser;
    
    ModbusStats                     modbusStats{};

    Buffer<uint16_t>                modbusRegs;
    RegAccessor                     regAccessor;
    std::unique_ptr<cli::Cli>       cli;
    std::unique_ptr<LinkAcceptorRl> linkAcceptor;
    std::unique_ptr<ModbusServer>   modbus;
    BusStats                                      spbusStats{};
    BusStats                                      rsbusStats{};
    std::array<std::unique_ptr<Bus>, maxNumGates> gates;

};

}