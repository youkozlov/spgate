#pragma once

#include <cstdint>
#include <memory>

#include "sm/Server.hpp"
#include "sm/ServerFsm.hpp"

#include "utils/Buffer.hpp"

#include "ModbusDefs.hpp"

namespace sg
{

class WrapBuffer;
class ModbusBuffer;
class Link;
class LinkAcceptor;

class ModbusServer : public Server
{
public:
    struct Init
    {
        Buffer<uint16_t>& regs;
        LinkAcceptor&     acceptor;
        ModbusStats&      stats;
    };
    explicit ModbusServer(Init const&);
    
    ~ModbusServer();
    
    void tickInd();

    char const* name() final;

    int accept() final;

    int process() final;

    void reset() final;

private:

    ModbusTcpAdu parseAdu(WrapBuffer&);
    bool processAdu(ModbusTcpAdu const&, WrapBuffer&);
    bool processReadCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool processWriteCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool processMultiWriteCmd(ModbusTcpAdu const&, WrapBuffer&);
    bool sendError(int, ModbusTcpAdu const&, WrapBuffer&);
    bool sendRespond(WrapBuffer const&);

    ServerFsm             fsm;
    std::unique_ptr<Link> link;
    Buffer<uint16_t>&     regs;
    LinkAcceptor&         acceptor;
    ModbusStats&          stats;
    
    std::array<unsigned char, rxBufferSize> rawBuffer;
    uint64_t tick;
};

}