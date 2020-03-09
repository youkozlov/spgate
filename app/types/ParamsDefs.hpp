#pragma once

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
    unsigned int readPeriod;
};

struct GateParams
{
    unsigned int id;
    GateType     gateType;
    IpAddr       gateAddr;
    BusAddr      addr;
};

struct DeviceParams
{
    unsigned int id;
    unsigned int gateId;
    BusAddr      addr;
};

struct ParamParams
{
    unsigned int id;
    unsigned int deviceId;
    unsigned int func;
    unsigned int chan;
    ParamAddr    addr;
};

}