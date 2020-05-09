#include "ModbusClient.hpp"

#include "sockets/LinkRl.hpp"

#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/WrapBuffer.hpp"

namespace sg
{
    
ModbusClient::ModbusClient(Init const& init)
    : state(ModbusClientState::init)
    , ipAddr(init.ipAddr)
    , regs(init.regs)
    , stats(init.stats)
    , tick(0)
{
    link = std::unique_ptr<Link>(new LinkRl(init.ipAddr));
}

ModbusClient::~ModbusClient()
{
}

bool ModbusClient::exec(ModbusRequest const& req)
{
    if (state != ModbusClientState::run)
    {
        LM(LW, "Request in unexpected state: %s", toString(state));
        return false;
    }
    
    storedAdu = ModbusTcpAdu();
    storedAdu.startReg = req.startReg;
    storedAdu.numRegs = req.numRegs;
    storedAdu.slaveAddr = req.slaveAddr;
    storedAdu.fc = req.fc;

    WrapBuffer msgBuf(&rawBuffer[0], rawBuffer.size());
    
    switch (storedAdu.fc)
    {
    case 0x3:
    case 0x4:
        execReadCmd(storedAdu, msgBuf);
        break;
    case 0x6:
        processWriteCmd(storedAdu, msgBuf);
        break;
    case 0x10:
        processMultiWriteCmd(storedAdu, msgBuf);
        break;
    default:
        return false;
    }
    stats.nTx += 1;
    link->write(&rawBuffer[0], msgBuf.size());
    chageState(ModbusClientState::wait);
    return true;
}

void ModbusClient::tickInd()
{
    switch (state)
    {
    case ModbusClientState::init:
        processInit();
    break;
    case ModbusClientState::idle:
        processIdle();
    break;
    case ModbusClientState::connect:
        processConnect();
    break;
    case ModbusClientState::run:
        processRun();
    break;
    case ModbusClientState::wait:
        processWait();
    break;
    case ModbusClientState::error:
        processError();
    break;
    }
    tick += 1;
}

void ModbusClient::processInit()
{
    chageState(ModbusClientState::idle);
}

void ModbusClient::processIdle()
{
    if (tick % 128 == 0)
    {
        chageState(ModbusClientState::connect);
    }
}

void ModbusClient::processConnect()
{
    if (link->connect() < 0)
    {
        chageState(ModbusClientState::error);
        return;
    }    
    chageState(ModbusClientState::run);
}

void ModbusClient::processRun()
{
}

void ModbusClient::processWait()
{
    int const len = link->read(&rawBuffer[0], mbAduHdrLen, 1000);
    if (len == mbAduHdrLen)
    {
        WrapBuffer msgBuf(&rawBuffer[0], rawBuffer.size());
        ModbusTcpAdu const adu = parseAdu(msgBuf);
        uint16_t const tail = adu.pktLen - 2;
        if (link->read(&rawBuffer[mbAduHdrLen], tail, 1000) == tail)
        {
            stats.nRx += 1;
            processAdu(adu, msgBuf);
        }
        else
        {
            stats.nInvalid += 1;
            LM(LE, "Received packet is invalid: pktLen=%d", adu.pktLen);
        }
        chageState(ModbusClientState::run);
    }
    else if (len < 0)
    {
        chageState(ModbusClientState::error);
    }
    else if (len) 
    {
        LM(LE, "Received packet with invalid: len=%d", len);
    }
}

void ModbusClient::processError()
{
    if (tick % 1024 == 0)
    {
        link->close();
        chageState(ModbusClientState::idle);
    }
}

ModbusTcpAdu ModbusClient::parseAdu(WrapBuffer& msgBuf)
{
    ModbusTcpAdu adu;
    
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

bool ModbusClient::processAdu(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    if (storedAdu.transactionId != adu.transactionId)
    {
        LM(LE, "Unexpected transactionId, stored=%d, given=%d", storedAdu.transactionId, adu.transactionId);
        return false;
    }
    switch (adu.fc)
    {
    case 0x3:
    case 0x4:
        return processReadRsp(adu, buf);
    break;
    }
    return false;
}

bool ModbusClient::execReadCmd(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nRd += 1;

    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(1 + 1 + 2 + 2);
    buf.write(adu.slaveAddr);
    buf.write(adu.fc);
    buf.writeBe(adu.startReg);
    buf.writeBe(adu.numRegs);
    return true;
}

bool ModbusClient::processReadRsp(ModbusTcpAdu const&, WrapBuffer& buf)
{
    uint16_t numBytes = buf.read();
    if (storedAdu.numRegs * 2 != numBytes)
    {
        LM(LE, "Unexpected number of bytes, stored=%d, given=%d", (storedAdu.numRegs * 2), numBytes);
        return false;
    }
    uint16_t startReg = storedAdu.startReg;
    for (int i = 0; i < storedAdu.numRegs; ++i, ++startReg)
    {
        regs[startReg] = buf.readBe();
    }
    return true;
}

bool ModbusClient::processWriteCmd(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nWr += 1;

    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(1 + 1 + 2 + 2);
    buf.write(adu.slaveAddr);
    buf.write(adu.fc);
    buf.writeBe(adu.startReg);
    buf.writeBe(regs[adu.startReg]);
    return true;
}

bool ModbusClient::processMultiWriteCmd(ModbusTcpAdu const& adu, WrapBuffer& buf)
{
    stats.nMultiWr += 1;

    uint16_t startReg = adu.startReg;
    buf.writeBe(adu.transactionId);
    buf.writeBe(adu.protocolId);
    buf.writeBe(1 + 1 + 2 + 2 + 1 + adu.numRegs * 2);
    buf.write(adu.slaveAddr);
    buf.write(adu.fc);
    buf.writeBe(startReg);
    buf.writeBe(adu.numRegs);
    buf.write(adu.numRegs * 2);
    for (int i = 0; i < adu.numRegs; ++i, ++startReg)
    {
        buf.writeBe(regs[startReg]);
    }
    return true;
}

ModbusClientState ModbusClient::getState() const
{
    return state;
}

void ModbusClient::chageState(ModbusClientState newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* ModbusClient::toString(ModbusClientState st) const
{
    switch (st)
    {
    case ModbusClientState::init:
        return "Init";
    case ModbusClientState::idle:
        return "Idle";
    case ModbusClientState::connect:
        return "Connect";
    case ModbusClientState::run:
        return "Run";
    case ModbusClientState::wait:
        return "Wait";
    case ModbusClientState::error:
        return "Error";
    default:
        return "Invalid";
    }
}
}