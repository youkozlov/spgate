#pragma once

#include <cstring>

namespace sg
{

enum ModbusErorr
{
    NoError = 0,
    IllegalFunction = 1,
    IllegalDataAddress = 2,
    IllegalDataValue = 3,
    SlaveDeviceFailure = 4,
    Acknowledge = 5,
    SlaveDeviceBusy = 6,
    NegativeAcknowledge = 7,
    MemoryParityError = 8,
    GatewayPathUnavailable = 10,
    GatewayTargetDeviceFailedtoRespond = 11
};

struct ModbusTcpAdu
{
    uint16_t transactionId;
    uint16_t protocolId;
    uint16_t pktLen;
    uint16_t startReg;
    uint16_t numRegs;
    uint8_t  slaveAddr;
    uint8_t  fc;
};

struct ModbusRequest
{
    uint8_t  slaveAddr;
    uint8_t  fc;
    uint16_t startReg;
    uint16_t numRegs;
};

struct ModbusStats
{
    uint32_t nRx;
    uint32_t nTx;
    uint32_t nInvalid;
    uint32_t nError;
    uint32_t nRd;
    uint32_t nWr;
    uint32_t nMultiWr;

    ModbusStats()
    {
        memset(this, 0, sizeof(*this));
    }
};

inline bool operator==(ModbusStats const& a, ModbusStats const& b)
{
    return 
        a.nRx == b.nRx &&
        a.nTx == b.nTx &&
        a.nInvalid == b.nInvalid &&
        a.nError == b.nError &&
        a.nRd == b.nRd &&
        a.nWr == b.nWr &&
        a.nMultiWr == b.nMultiWr;
}

constexpr int MB_ERR = 0x8f;
constexpr int mbAduHdrLen = 8;
constexpr int rxBufferSize = 512;

}