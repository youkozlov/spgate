#pragma once

#include <cstdint>

namespace sg
{

constexpr uint8_t FSC   = 0x10;
constexpr uint8_t FEC   = 0x16;
constexpr uint8_t RDRAM = 0x52;
constexpr uint8_t PAD   = 0x00;
constexpr uint8_t maxDataBlockLength = 64;
constexpr uint8_t dataRequestLength = 4;

struct RsBusFrame
{
    uint8_t nt;
    uint8_t rc;
    uint8_t a1;
    uint8_t a0;
    uint8_t qty;
    uint8_t data[maxDataBlockLength];
};

}