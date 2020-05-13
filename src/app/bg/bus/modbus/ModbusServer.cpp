#include "ModbusServer.hpp"

#include "sockets/LinkRl.hpp"
#include "sockets/LinkAcceptor.hpp"

#include "utils/Utils.hpp"
#include "utils/WrapBuffer.hpp"
#include "utils/Logger.hpp"

namespace sg
{

ModbusServer::ModbusServer(Init const& init)
    : fsm(*this)
    , regs(init.regs)
    , acceptor(init.acceptor)
    , stats(init.stats)
{
    link = std::unique_ptr<LinkRl>(new LinkRl(-1));
}

ModbusServer::~ModbusServer()
{
}

void ModbusServer::tickInd()
{
    fsm.tickInd();
}

char const* ModbusServer::name()
{
    return "Modbus";
}

int ModbusServer::accept()
{
    int fd = acceptor.accept();
    if (fd < 0)
    {
        return 0;
    }
    link->setHandl(fd);
    stats.nAcceptConnections += 1;
    return 1;
}

int ModbusServer::process()
{
    int const len = link->read(&rawBuffer[0], mbAduHdrLen, 50);
    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        return len;
    }
    else if (len == mbAduHdrLen)
    {
        WrapBuffer msgBuf(&rawBuffer[0], rawBuffer.size());
        ModbusTcpAdu const adu = parseAdu(msgBuf);

        uint16_t const tail = adu.pktLen - 2;

        bool isValid = adu.pktLen > 2 && adu.pktLen < 256 && adu.protocolId == 0;

        if (isValid && link->read(&rawBuffer[mbAduHdrLen], tail, 50) == tail)
        {
            stats.nRx += 1;
            if (!processAdu(adu, msgBuf))
            {
                return -1;
            }
            return sendRespond(msgBuf) == false;
        }
        else
        {
            stats.nInvalid += 1;
            return -1;
        }
    }
    else
    {
        LM(LE, "Received packet with invalid: len=%d", len);
        stats.nInvalid += 1;
    }
    return -1;
}

void ModbusServer::reset()
{
    stats.nResetConnections += 1;
    link->close();
}

bool ModbusServer::sendRespond(WrapBuffer const& msgBuf)
{
    stats.nTx += 1;
    unsigned int txLen = link->write(msgBuf.cbegin(), msgBuf.size());
    if (txLen != msgBuf.size())
    {
        LM(LE, "Sended bytes less than expected: txLen=%d msgBuf=%d", txLen, msgBuf.size());
        return false;
    }
    return true;
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

    LM(LD, "processReadCmd: regs: %u:%u", startReg, numRegs);

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

}