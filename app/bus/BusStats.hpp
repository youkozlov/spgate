#pragma once

#include <cstdint>

namespace sg
{

struct BusStats
{
    uint32_t nRx;
    uint32_t nTx;
    uint32_t nInvalid;
    uint32_t nError;
    uint32_t nTimeout;
    uint32_t nRdp;
    uint32_t nRsp;
};

}