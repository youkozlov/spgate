#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "ModbusDefs.hpp"

namespace sg
{

enum class ModbusServerState
{
    init,
    idle,
    connect,
    run,
    error
};

class WrapBuffer;
class ModbusBuffer;
class Link;
class LinkAcceptor;

class ModbusServer
{
public:
    struct Init
    {
        ModbusBuffer& regs;
        LinkAcceptor& acceptor;
        ModbusStats& stats;
    };
    explicit ModbusServer(Init const&);
    
    ~ModbusServer();
    
    void tickInd();
    
private:

    void processInit();
    void processIdle();
    void processConnect();
    void processRun();
    void processError();
    void printStats();
    void chageState(ModbusServerState);
    char const* toString(ModbusServerState) const;

    ModbusTcpAdu parseAdu(WrapBuffer&);
    bool processAdu(ModbusTcpAdu const&, WrapBuffer&);
    bool processReadCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool processWriteCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool processMultiWriteCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool sendError(int, ModbusTcpAdu const&, WrapBuffer&);
    bool sendRespond(WrapBuffer const&);

    ModbusServerState state;
    std::unique_ptr<Link> client;

    ModbusBuffer& regs;
    LinkAcceptor& acceptor;
    ModbusStats& stats;
    
    std::vector<uint8_t> rawBuffer;
    uint64_t tick;
};

}