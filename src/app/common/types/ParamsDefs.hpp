#pragma once

#include <cstring>
#include "IpAddr.hpp"

namespace sg
{

constexpr unsigned int maxNumGates = 8;
constexpr unsigned int maxNumDevices = 8;
constexpr unsigned int maxNumParams = 128;

enum class GateType
{
    sps,
    m4
};

using BusAddr = unsigned int;
using ParamAddr = unsigned int;

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