#pragma once

#include <cstring>
#include "IpAddr.hpp"

namespace sg
{

constexpr unsigned int maxNumGates = 8;
constexpr unsigned int maxNumDevices = 8;
constexpr unsigned int maxNumParams = 128;
constexpr unsigned int maxLenName = 32;

enum class GateType
{
    sps,
    m4
};

using BusAddr = unsigned int;
using ParamAddr = unsigned int;

enum class SerialSpeed
{
    b2400   = 0000013,
    b4800   = 0000014,
    b9600   = 0000015,
    b19200  = 0000016,
    b38400  = 0000017,
    b57600  = 0010001,
    b115200 = 0010002
};

enum class SerialBlock
{
    off = 0,
    on  = 1
};

enum class SerialRtsCts
{
    off = 0,
    on  = 1
};

enum class SerialBits
{
    b7 = 7,
    b8 = 8
};

enum class SerialStopBits
{
    b1 = 1,
    b2 = 2
};

enum class SerialParity
{
    none = 0,
    odd  = 1,
    even = 2
};

using SerialName = char[maxLenName + 1];

struct SerialParams
{
    SerialName      serialName;
    SerialSpeed     speed;
    SerialBlock     block;
    SerialRtsCts    rtscts;
    SerialBits      bits;
    SerialStopBits  stopbits;
    SerialParity    parity;

    SerialParams()
    {
        std::memset(this, 0, sizeof(SerialParams));
        speed     = SerialSpeed::b9600;
        bits      = SerialBits::b8;
        stopbits  = SerialStopBits::b2;
        parity    = SerialParity::none;
    }
};


struct TtyGateParams
{
    IpAddr          ipAddr;
    SerialParams    serial;
    unsigned        startRxTimeout = 100;
    unsigned        endRxTimeout = 300;
};

struct CommonParams
{
    IpAddr       modbusAddr;
};

struct GateParams
{
    unsigned int id;
    GateType     gateType;
    IpAddr       gateAddr;
    BusAddr      addr;
    unsigned int readPeriod;
    unsigned int readTimeout;
    
    GateParams()
    {
        std::memset(this, 0, sizeof(GateParams));
    }
};

struct DeviceParams
{
    unsigned int id;
    unsigned int gateId;
    BusAddr      addr;

    DeviceParams()
    {
        std::memset(this, 0, sizeof(DeviceParams));
    }
};

enum class ParamType
{
    floatPoint,
    fixedPoint
};

struct ParamParams
{
    unsigned int id;
    unsigned int deviceId;
    unsigned int func;
    unsigned int chan;
    ParamAddr    addr;
    ParamType    type;

    ParamParams()
    {
        std::memset(this, 0, sizeof(ParamParams));
    }
};

}