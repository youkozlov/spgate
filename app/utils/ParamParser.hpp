#pragma once

#include <string>
#include <vector>
#include <functional>

class rlIniFile;

namespace sg
{

enum class GateType
{
    sps,
    m4
};

struct IpAddr
{
    char          addr[32];
    unsigned int  port;
};

using BusAddr = unsigned int;

struct CommonParams
{
    IpAddr       modbusAddr;
    unsigned int readPeriod;
};

struct GateParams
{
    GateType gateType;
    IpAddr   gateAddr;
    BusAddr  addr;
};

struct DeviceParams
{
    unsigned int gateId;
    BusAddr      addr;
};

struct ParamParams
{
    unsigned int deviceId;
    unsigned int func;
    unsigned int chan;
    BusAddr      addr;
};

class ParamParser
{
public:

    bool parseFile(char const*);
    
    bool parseString(std::string const&);

    CommonParams const& getCommon() const;

    GateParams const& getGate(int i) const;
    int getNumGates() const;
    
    DeviceParams const& getDevice(int i) const;
    int getNumDevices() const;
    
    ParamParams const& getParam(int i) const;
    int getNumParams() const;
    
private:

    enum NameType
    {
        mandatory,
        optional
    };
    
    void parseConfig(rlIniFile&);
    void parseCommon(rlIniFile&);
    bool parseGates(rlIniFile&, char const*);

    bool parseDevices(rlIniFile&, char const*);
    bool parseParams(rlIniFile&, char const*);
    bool findSection(rlIniFile&, char const*, NameType);
    bool parseName(rlIniFile&, char const*, char const*, NameType, std::function<bool(char const*)>);
 
    CommonParams              common;
    std::vector<GateParams>   gates;
    std::vector<DeviceParams> devices;
    std::vector<ParamParams>  params;
};

}