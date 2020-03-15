#pragma once

#include <memory>
#include <array>

#include "utils/ParamParser.hpp"

#include "modbus/ModbusDefs.hpp"
#include "modbus/ModbusBuffer.hpp"

namespace sg
{

class TagAccessor;
class LinkAcceptorRl;
class ModbusServer;
class Client;

enum class BusGateState
{
    init,
    run,
    error
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
    
    void tickInd();

    BusGateState getState() const;

private:
    bool validateTags();
    bool initSerialPort();
    void processInit();
    void processRun();
    void processError();
    void chageState(BusGateState);
    char const* toString(BusGateState) const;

    bool createModbus();
    bool createGates();

    char const*                     iniFileName;
    BusGateState                     state;
    ParamParser                     parser;
    
    ModbusStats                     modbusStats{};
    ModbusBuffer                    modbusRegs;
    std::unique_ptr<LinkAcceptorRl> linkAcceptor;
    std::unique_ptr<ModbusServer>   modbus;

    std::array<std::unique_ptr<Client>, maxNumGates> gates;
};

}