#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "types/IpAddr.hpp"
#include "ModbusDefs.hpp"

namespace sg
{

enum class ModbusClientState
{
    init,
    idle,
    connect,
    run,
    wait,
    error
};

class WrapBuffer;
class ModbusBuffer;
class Link;

class ModbusClient
{
public:
    struct Init
    {
        IpAddr const& ipAddr;
        ModbusBuffer& regs;
        ModbusStats& stats;
    };
    explicit ModbusClient(Init const&);
    
    ~ModbusClient();
    
    bool exec(ModbusRequest const&);
    
    void tickInd();

    ModbusClientState getState() const;
    
private:

    void processInit();
    void processIdle();
    void processConnect();
    void processRun();
    void processWait();
    void processError();
    void chageState(ModbusClientState);
    char const* toString(ModbusClientState) const;

    ModbusTcpAdu parseAdu(WrapBuffer&);
    bool processAdu(ModbusTcpAdu const&, WrapBuffer&);
    bool execReadCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool processReadRsp(ModbusTcpAdu const&, WrapBuffer&);
    bool processWriteCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool processMultiWriteCmd(ModbusTcpAdu const&, WrapBuffer&);
 
    ModbusClientState state;
    std::unique_ptr<Link> link;

    IpAddr const& ipAddr;
    ModbusBuffer& regs;
    ModbusStats& stats;
    
    std::vector<uint8_t> rawBuffer;
    uint64_t tick;
    ModbusTcpAdu storedAdu;
};

}