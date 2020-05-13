#pragma once

#include <string>
#include <vector>
#include <functional>

#include "types/ParamsDefs.hpp"

class rlIniFile;

namespace sg
{

class ParamParser
{
public:

    ParamParser();

    bool parseFile(char const*);
    
    bool parseString(std::string const&);

    bool isTtygParsed() const;
    TtyGateParams const& getTtyg() const;

    bool isCommonParsed() const;
    CommonParams const& getCommon() const;

    GateParams const& getGate(unsigned int i) const;
    unsigned int getNumGates() const;
    
    DeviceParams const& getDevice(unsigned int i) const;
    unsigned int getNumDevices() const;

    ParamParams const& getParam(unsigned int i) const;
    unsigned int getNumParams() const;
    
private:

    enum NameType
    {
        mandatory,
        optional
    };

    void parseConfig(rlIniFile&);
    bool parseTtyg(rlIniFile&);
    bool parseCommon(rlIniFile&);
    bool parseGates(rlIniFile&, char const*);

    bool parseDevices(rlIniFile&, char const*);
    bool parseParams(rlIniFile&, char const*);
    bool findSection(rlIniFile&, char const*, NameType);
    bool parseName(rlIniFile&, char const*, char const*, NameType, std::function<bool(char const*)>);

    TtyGateParams             ttyg;
    bool                      ttygParsed;

    CommonParams              common;
    bool                      commonParsed;

    std::vector<GateParams>   gates;
    std::vector<DeviceParams> devices;
    std::vector<ParamParams>  params;
};

}