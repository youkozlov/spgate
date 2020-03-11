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
class Gate;

enum class SpGateState
{
    init,
    run,
    error
};

class SpGate
{
public:
    struct Init
    {
        char const* iniFileName;
    };
    
    explicit SpGate(Init const&);
    
    ~SpGate();
    
    void tickInd();

    SpGateState getState() const;

private:
    bool validateTags();
    bool initSerialPort();
    void processInit();
    void processRun();
    void processError();
    void chageState(SpGateState);
    char const* toString(SpGateState) const;

    bool createModbus();
    bool createGates();

    char const*                     iniFileName;
    SpGateState                     state;
    ParamParser                     parser;
    
    ModbusStats                     modbusStats{};
    ModbusBuffer                    modbusRegs;
    std::unique_ptr<LinkAcceptorRl> linkAcceptor;
    std::unique_ptr<ModbusServer>   modbus;

    std::array<std::unique_ptr<Gate>, maxNumGates> gates;
};

}