#include "ParamParser.hpp"
#include "Logger.hpp"
#include "rlsocket.h"
#include "rlinifile.h"
#include <sstream>
#include <string>

namespace sg
{

bool parseSectionId(unsigned int& prm, const char* str)
{
    return sscanf(str, "%*[^0-9]%u", &prm) == 1;
}

bool parseValue(IpAddr& prm, const char* str)
{
    unsigned int a, b, c, d;
    if (sscanf(str, "%u.%u.%u.%u:%u", &a, &b, &c, &d, &prm.port) != 5)
    {
        return false;
    }
    if (a > 255 || b > 255 || c > 255 || d > 255 || prm.port > 65535)
    {
        return false;
    }
    sprintf(prm.addr, "%u.%u.%u.%u", a, b, c, d);
    return true;
}

bool parseValue(unsigned int& prm, const char* str, int base = 10)
{
    switch (base)
    {
    case 10:
        return sscanf(str, "%u", &prm) == 1;
    case 16:
        return sscanf(str, "%x", &prm) == 1;
    default:
        return false;
    }
}

bool parseValue(GateType& prm, const char* str)
{
    if (!strcmp(str, "sps"))
    {
        prm = GateType::sps;
    }
    else if (!strcmp(str, "m4"))
    {
        prm = GateType::m4;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

class rlStringSocket : public rlSocketInterface
{
public:
    explicit rlStringSocket(std::string const& s)
        : str(s)
        , ss(str)
    {}
    int readStr(char *buf, int) final
    {
        std::string line;
        std::getline(ss, line);
        std::copy(line.cbegin(), line.cend(), buf);
        buf[line.length()] = 0;
        return !ss.eof();
    }
private:
    std::string str;
    std::stringstream ss;
};

bool ParamParser::parseFile(char const* fileName)
{
    rlIniFile iniParser;
    if (iniParser.read(fileName))
    {
        LM(LE, "IniParser can't complete read");
        return false;
    }
    try
    {
        parseConfig(iniParser);
    }
    catch (const char*)
    {
        LM(LE, "Configuration error is catched, please check ini file");
        return false;
    }
    return true;
}

bool ParamParser::parseString(std::string const& data)
{
    rlStringSocket dataBuf(data);
    rlIniFile iniParser;
    if (iniParser.read(&dataBuf))
    {
        LM(LE, "IniParser can't complete read");
        return false;
    }
    try
    {
        parseConfig(iniParser);
    }
    catch (const char*)
    {
        LM(LE, "Configuration error is catched, please check ini file");
        return false;
    }
    return true;
}

CommonParams const& ParamParser::getCommon() const
{
    return common;
}

GateParams const& ParamParser::getGate(unsigned int i) const
{
    return gates[i];
}

unsigned int ParamParser::getNumGates() const
{
    return gates.size();
}

DeviceParams const& ParamParser::getDevice(unsigned int i) const
{
    return devices[i];
}

unsigned int ParamParser::getNumDevices() const
{
    return devices.size();
}

ParamParams const& ParamParser::getParam(unsigned int i) const
{
    return params[i];
}

unsigned int ParamParser::getNumParams() const
{
    return params.size();
}


void ParamParser::parseConfig(rlIniFile& parser)
{
    parseCommon(parser);

    char sectionName[16] = {};
    unsigned int i = 0;
    do
    {
        sprintf(sectionName, "gate%d", i++);
    }
    while (parseGates(parser, sectionName) && i < maxNumGates);
    
    i = 0;
    do
    {
        sprintf(sectionName, "device%d", i++);
    }
    while (parseDevices(parser, sectionName) && i < maxNumDevices);

    i = 0;
    do
    {
        sprintf(sectionName, "param%d", i++);
    }
    while (parseParams(parser, sectionName) && i < maxNumParams);

    LM(LI, "ParseConfig completed: gates=%u, devices=%u, params=%u"
        , getNumGates()
        , getNumDevices()
        , getNumParams());
}

void ParamParser::parseCommon(rlIniFile& parser)
{
    findSection(parser, "common", mandatory);
    
    parseName(parser, "common", "modbus_addr", mandatory, [this](const char* str)
    {
        return parseValue(common.modbusAddr, str);
    });
}

bool ParamParser::parseGates(rlIniFile& parser, char const* gateName)
{
    if (!findSection(parser, gateName, optional))
    {
        return false;
    }

    GateParams prms{};

    if (!parseSectionId(prms.id, gateName))
    {
        LM(LE, "Can't parse id of section [%s]", gateName);
        throw("");
    }

    parseName(parser, gateName, "gate_type", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.gateType, str);
    });

    parseName(parser, gateName, "gate_addr", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.gateAddr, str);
    });

    NameType const nameType = (prms.gateType == GateType::sps) ? mandatory : optional;

    parseName(parser, gateName, "addr", nameType, [&prms](const char* str)
    {
        return parseValue(prms.addr, str, 16);
    });

    parseName(parser, gateName, "read_period", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.readPeriod, str);
    });

    gates.push_back(prms);
    
    return true;
}

bool ParamParser::parseDevices(rlIniFile& parser, char const* deviceName)
{
    if (!findSection(parser, deviceName, optional))
    {
        return false;
    }
    
    DeviceParams prms{};

    if (!parseSectionId(prms.id, deviceName))
    {
        LM(LE, "Can't parse id of section [%s]", deviceName);
        throw("");
    }

    parseName(parser, deviceName, "gate_id", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.gateId, str);
    });

    parseName(parser, deviceName, "addr", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.addr, str, 16);
    });

    devices.push_back(prms);

    return true;
}

bool ParamParser::parseParams(rlIniFile& parser, char const* paramName)
{
    if (!findSection(parser, paramName, optional))
    {
        return false;
    }
    
    ParamParams prms{};

    if (!parseSectionId(prms.id, paramName))
    {
        LM(LE, "Can't parse id for section [%s]", paramName);
        throw("");
    }

    parseName(parser, paramName, "device_id", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.deviceId, str);
    });

    parseName(parser, paramName, "func", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.func, str, 16);
    });

    parseName(parser, paramName, "chan", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.chan, str);
    });

    parseName(parser, paramName, "addr", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.addr, str);
    });

    params.push_back(prms);
    
    return true;
}

bool ParamParser::findSection(rlIniFile& parser, char const* section, NameType type)
{
    parser.firstSection();
    while (const char* current = parser.nextSection())
    {
        if (!strcmp(current, section))
        {
            LM(LD, "[%s]", section);
            return true;
        }
    }
    if (type == NameType::mandatory)
    {
        LM(LE, "Can't find section [%s]", section);
        throw("");
    }
    return false;
}

bool ParamParser::parseName(rlIniFile& iniParser, char const* section, char const* name, NameType type, std::function<bool(char const*)> nameParser)
{
    bool isFirst = true;
    while (const char* prmsName = isFirst ? iniParser.firstName(section) : iniParser.nextName(section))
    {
        if (!strcmp(prmsName, name))
        {
            char const* text = iniParser.text(section, prmsName);
            if (nameParser(text))
            {
                LM(LD, "%s=%s", prmsName, text);
                return true;
            }
            LM(LE, "Can't parse name '%s' within section [%s]", name, section);
            throw("");
        }
        isFirst = false;
    }

    if (type == NameType::mandatory)
    {
        LM(LE, "Can't find name '%s' within section [%s]", name, section);
        throw("");
    }

    return false;
}

}