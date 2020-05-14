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
        unsigned cliPort;
        bool isEmulMode;
    };
    
    explicit BusGate(Init const&);
    
    ~BusGate();
    
    int tickInd();

    void shutdown();

    BusGateState getState() const;

    ModbusStats const& getModbusStats() const;

    BusStats const& getSpBusStats() const;

    BusStats const& getRsBusStats() const;

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
    bool createGatesClient();
    bool createGatesServer();

    char const*                     iniFileName;
    BusGateState                    state;
    ParamParser                     parser;
    
    ModbusStats                     modbusStats;

    Buffer<uint16_t>                modbusRegs;
    RegAccessor                     regAccessor;
    std::unique_ptr<cli::Cli>       cli;
    std::unique_ptr<LinkAcceptorRl> linkAcceptor;
    std::unique_ptr<ModbusServer>   modbus;
    BusStats                                      spbusStats;
    BusStats                                      rsbusStats;
    std::array<std::unique_ptr<Bus>, maxNumGates> gates;
    unsigned const                                cliPort;
    bool const                                    isEmulMode;
    std::unique_ptr<Buffer<float>>                serverRegs;
};

}