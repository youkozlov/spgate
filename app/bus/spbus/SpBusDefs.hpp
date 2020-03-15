#pragma once

#include <cstdint>

namespace sg
{

const char DLE = 0x10;
const char SOH = 0x01;
const char ISI = 0x1F;
const char RDP = 0x1D;
const char RSP = 0x03;
const char STX = 0x02;
const char ETX = 0x03;
const char HT  = 0x09;
const char FF  = 0x0C;

struct SpBusStats
{
    uint32_t nRx;
    uint32_t nTx;
    uint32_t nInvalid;
    uint32_t nError;
    uint32_t nRdp;
    uint32_t nRsp;
};

}