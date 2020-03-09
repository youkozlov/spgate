#include "ModbusServer.hpp"

#include "interfaces/Link.hpp"
#include "interfaces/LinkAcceptor.hpp"

#include "utils/Utils.hpp"
#include "utils/WrapBuffer.hpp"
#include "utils/Logger.hpp"

#include "ModbusBuffer.hpp"

namespace sg
{
    
ModbusServer::ModbusServer(Init const& init)
    : state(ModbusServerState::init)
    , regs(init.regs)
    , acceptor(init.acceptor)
    , stats(init.stats)
    , tick(0)
{
    rawBuffer.resize(rxBufferSize);
}

ModbusServer::~ModbusServer()
{
}

void ModbusServer::tickInd()
{
    switch (state)
    {
    case ModbusServerState::init:
        processInit();
    break;
    case ModbusServerState::idle:
        processIdle();
    break;
    case ModbusServerState::connect:
        processConnect();
    break;
    case ModbusServerState::run:
        processRun();
    break;
    case ModbusServerState::error:
        processError();
    break;
    }
    tick += 1;
    if (tick % 4096 == 0)
    {
        printStats();
    }
}

void ModbusServer::processInit()
{
    chageState(ModbusServerState::idle);
}

void ModbusServer::processIdle()
{
    if (tick % 128 == 0)
    {
        chageState(ModbusServerState::connect);
    }
}

void ModbusServer::processConnect()
{
    client = acceptor.accept();
    if (!client)
    {
        chageState(ModbusServerState::error);
        return;
    }
    
    chageState(ModbusServerState::run);
}

void ModbusServer::processRun()
{
    int const len = client->read(&rawBuffer[0], mbAduHdrLen, 1000);
    if (len == mbAduHdrLen)
    {
        WrapBuffer msgBuf(&rawBuffer[0], rawBuffer.size());
        ModbusTcpAdu const adu = parseAdu(msgBuf);

        uint16_t const tail = adu.pktLen - 2;

        if (client->read(&rawBuffer[mbAduHdrLen], tail, 1000) == tail)
        {
            stats.nRx += 1;
            if (!processAdu(adu, msgBuf))
            {
                return;
            }
            sendRespond(msgBuf);
        }
        else
        {
            stats.nInvalid += 1;
        }
    }
    else if (len < 0)
    {
        chageState(ModbusServerState::error);
    }
    else if (len) 
    {
        LM(LE, "Received packet with invalid: len=%d", len);
    }
}

bool ModbusServer::sendRespond(WrapBuffer const& msgBuf)
{
    stats.nTx += 1;   
    int txLen = client->write(msgBuf.cbegin(), msgBuf.size());
    if (txLen != msgBuf.size())
    {
        LM(LE, "Sended bytes less than expected: txLen=%d msgBuf=%d", txLen, msgBuf.size());
        return false;
    }
    return true;
}

void ModbusServer::processError()
{
    if (tick % 1024 == 0)
    {
        client.reset();
        chageState(ModbusServerState::idle);
    }
}

void ModbusServer::printStats()
{
    LM(LD, "ModbusStats: nRx=%u nTx=%u nInvalid=%u nError=%u nRd=%u nWr=%u nMultiWr=%u"
        , stats.nRx, stats.nTx, stats.nInvalid
        , stats.nError, stats.nRd, stats.nWr
        , stats.nMultiWr
        );
}

ModbusTcpAdu ModbusServer::parseAdu(WrapBuffer& msgBuf)
{
    ModbusTcpAdu adu{};
    
    adu.transactionId = msgBuf.readBe();
    adu.protocolId = msgBuf.readBe();
    adu.pktLen = msgBuf.readBe();
    adu.slaveAddr = msgBuf.read();
    adu.fc = msgBuf.read();

    LM(LD, "RX ADU:tr=%d prot=%d pktLen=%d addr=%d fc=%d"
        , adu.transactionId
        , adu.protocolId
        , adu.pktLen
        , adu.slaveAddr
        , adu.fc
        );
        
    return adu;
}

bool ModbusServer::processAdu(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    switch (adu.fc)
    {
    case 0x3:
    case 0x4:
        return processReadCmd(adu, buf);
    case 0x6:
        return processWriteCmd(adu, buf);
    case 0x10:
        return processMultiWriteCmd(adu, buf);
    default:
        return sendError(ModbusErorr::IllegalFunction, adu, buf);
    }
    return false;
}

bool ModbusServer::processReadCmd(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nRd += 1;
    
    uint16_t startReg = buf.readBe();
    uint16_t numRegs = buf.readBe();
    uint16_t numBytes = numRegs * 2;
    if (numRegs >= 127)
    {
        return sendError(ModbusErorr::IllegalDataValue, adu, buf);
    }
    if ((startReg + numRegs) >= regs.size())
    {
        return sendError(ModbusErorr::IllegalDataAddress, adu, buf);
    }
    buf.reset();
    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(1 + 1 + 1 + numBytes);
    buf.write(adu.slaveAddr);
    buf.write(adu.fc);
    buf.write(numBytes);
    for (int i = 0; i < numRegs; ++i, ++startReg)
    {
        buf.writeBe(regs[startReg]);
    }
    return true;
}

bool ModbusServer::processWriteCmd(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nWr += 1;

    uint16_t startReg = buf.readBe();
    uint16_t val = buf.readBe();
    if (startReg >= regs.size())
    {
        return sendError(ModbusErorr::IllegalDataAddress, adu, buf);
    }
    regs[startReg] = val;
    buf.reset();
    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(adu.pktLen);
    buf.write(adu.slaveAddr);
    buf.write(adu.fc);
    buf.writeBe(startReg);
    buf.writeBe(val);
    return true;
}

bool ModbusServer::processMultiWriteCmd(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nMultiWr += 1;

    uint16_t startReg = buf.readBe();
    uint16_t numRegs = buf.readBe();
    uint16_t numBytes = buf.read();
    if (numBytes != numRegs * 2)
    {
        return sendError(ModbusErorr::SlaveDeviceFailure, adu, buf);
    }
    if (startReg + numRegs >= regs.size())
    {
        return sendError(ModbusErorr::IllegalDataAddress, adu, buf);
    }
    for (int i = 0; i < numRegs; ++i, ++startReg)
    {
        regs[startReg] = buf.readBe();
    }
    buf.reset();
    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(1 + 1 + 2 + 2);
    buf.write(adu.slaveAddr);
    buf.write(adu.fc);
    buf.writeBe(startReg);
    buf.writeBe(numRegs);
    return true;
}

bool ModbusServer::sendError(int err, ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nError += 1;

    buf.reset();
    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(1 + 1 + 1);
    buf.write(adu.slaveAddr);
    buf.write(MB_ERR);
    buf.write(err);
    return true;
}

void ModbusServer::chageState(ModbusServerState newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* ModbusServer::toString(ModbusServerState st) const
{
    switch (st)
    {
    case ModbusServerState::init:
        return "Init";
    case ModbusServerState::idle:
        return "Idle";
    case ModbusServerState::connect:
        return "Connect";
    case ModbusServerState::run:
        return "Run";
    case ModbusServerState::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}