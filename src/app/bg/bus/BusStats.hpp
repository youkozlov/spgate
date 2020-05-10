#pragma once

#include <cstdint>
#include <cstring>

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

    BusStats()
    {
        std::memset(this, 0, sizeof(BusStats));
    }
};

}