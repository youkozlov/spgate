#pragma once

#include <cstring>
#include <cstdint>

namespace sg
{
namespace rsbus
{

enum DeviceType : uint16_t
{
    type741  = 0x4729,
    type742  = 0x472A,
    type743  = 0x472B,
    type9411 = 0x9229,
    type941  = 0x5429,
    type942  = 0x542A,
    type943  = 0x542B
};

constexpr uint8_t FSC = 0x10;
constexpr uint8_t FEC = 0x16;

constexpr uint8_t SRC = 0x3F;
constexpr uint8_t RDP = 0x52;
constexpr uint8_t ERR = 0x21;

constexpr uint8_t FRM = 0x90;
constexpr uint8_t ATR = 0x00;
constexpr uint8_t RM4 = 0x72;
constexpr uint8_t TAG_IEEFloat = 0x43;
constexpr uint8_t TAG_PNUM = 0x4A;

constexpr uint8_t maxDataBlockLength  = 64;
constexpr uint8_t dataRequestLength   = 4;
constexpr uint8_t startSequenceLength = 16;

struct RsBusFrame
{
    uint8_t nt;
    uint8_t rc;
    uint8_t id;
    uint8_t atr;
    uint8_t tag;
    uint8_t ch;
    uint16_t prm;
    uint8_t qty;
    uint8_t data[maxDataBlockLength];

    RsBusFrame()
    {
        std::memset(this, 0, sizeof(RsBusFrame));
    }
};

}
}
