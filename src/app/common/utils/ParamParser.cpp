#include "ParamParser.hpp"
#include "utils/Logger.hpp"
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

bool parseValue(unsigned int& prm, const char* str)
{
    if (sscanf(str, "0x%x", &prm) == 1)
    {
        return true;
    }
    else if (sscanf(str, "%u", &prm) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool parseReadPeriod(unsigned int& prm, const char* str)
{
    if (sscanf(str, "%u", &prm) != 1)
    {
        LM(LE, "Unexpected value: %s", str);
        return false;
    }
    if (prm < 50 || prm > 65535)
    {
        LM(LE, "Valid range is (50, 65535) but given param: %u", prm);
        return false;
    }
    return true;
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

bool parseValue(ParamType& prm, const char* str)
{
    if (!strcmp(str, "float"))
    {
        prm = ParamType::floatPoint;
    }
    else if (!strcmp(str, "fixed"))
    {
        prm = ParamType::fixedPoint;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

bool parseValue(SerialName& prm, const char* str)
{
    std::strncpy(prm, str, maxLenName);
    return true;
}

bool parseValue(SerialSpeed& prm, const char* str)
{
    if (!strcmp(str, "2400"))
    {
        prm = SerialSpeed::b2400;
    }
    else if (!strcmp(str, "4800"))
    {
        prm = SerialSpeed::b4800;
    }
    else if (!strcmp(str, "9600"))
    {
        prm = SerialSpeed::b9600;
    }
    else if (!strcmp(str, "19200"))
    {
        prm = SerialSpeed::b19200;
    }
    else if (!strcmp(str, "38400"))
    {
        prm = SerialSpeed::b38400;
    }
    else if (!strcmp(str, "57600"))
    {
        prm = SerialSpeed::b57600;
    }
    else if (!strcmp(str, "115200"))
    {
        prm = SerialSpeed::b115200;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

bool parseValue(SerialBlock& prm, const char* str)
{
    if (!strcmp(str, "on"))
    {
        prm = SerialBlock::on;
    }
    else if (!strcmp(str, "off"))
    {
        prm = SerialBlock::off;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

bool parseValue(SerialRtsCts& prm, const char* str)
{
    if (!strcmp(str, "on"))
    {
        prm = SerialRtsCts::on;
    }
    else if (!strcmp(str, "off"))
    {
        prm = SerialRtsCts::off;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

bool parseValue(SerialStopBits& prm, const char* str)
{
    if (!strcmp(str, "1"))
    {
        prm = SerialStopBits::b1;
    }
    else if (!strcmp(str, "2"))
    {
        prm = SerialStopBits::b2;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

bool parseValue(SerialBits& prm, const char* str)
{
    if (!strcmp(str, "7"))
    {
        prm = SerialBits::b7;
    }
    else if (!strcmp(str, "8"))
    {
        prm = SerialBits::b8;
    }
    else
    {
        LM(LE, "Unexpected value: %s, len: %zu", str, strlen(str));
        return false;
    }
    return true;
}

bool parseValue(SerialParity& prm, const char* str)
{
    if (!strcmp(str, "none"))
    {
        prm = SerialParity::none;
    }
    else if (!strcmp(str, "odd"))
    {
        prm = SerialParity::odd;
    }
    else if (!strcmp(str, "even"))
    {
        prm = SerialParity::even;
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
        LM(LE, "IniParser can't read file: %s", fileName);
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

ParamParser::ParamParser()
    : ttygParsed(false)
    , commonParsed(false)
{
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

bool ParamParser::isTtygParsed() const
{
    return ttygParsed;
}

TtyGateParams const& ParamParser::getTtyg() const
{
    return ttyg;
}

bool ParamParser::isCommonParsed() const
{
    return commonParsed;
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
    ttygParsed = parseTtyg(parser);

    commonParsed = parseCommon(parser);

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

bool ParamParser::parseTtyg(rlIniFile& parser)
{
    constexpr char ttygName[] = "ttyg";

    if (!findSection(parser, ttygName, optional))
    {
        return false;
    }

    parseName(parser, ttygName, "ip_addr", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.ipAddr, str);
    });

    parseName(parser, ttygName, "port", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.serialName, str);
    });

    parseName(parser, ttygName, "speed", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.speed, str);
    });

    parseName(parser, ttygName, "block", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.block, str);
    });

    parseName(parser, ttygName, "rts_cts", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.rtscts, str);
    });

    parseName(parser, ttygName, "bits", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.bits, str);
    });

    parseName(parser, ttygName, "stop_bits", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.stopbits, str);
    });

    parseName(parser, ttygName, "parity", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.serial.parity, str);
    });

    parseName(parser, ttygName, "start_rx_timeout", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.startRxTimeout, str);
    });

    parseName(parser, ttygName, "end_rx_timeout", mandatory, [this](const char* str)
    {
        return parseValue(ttyg.endRxTimeout, str);
    });

    return true;
}

bool ParamParser::parseCommon(rlIniFile& parser)
{
    if (!findSection(parser, "common", optional))
    {
        return false;
    }

    parseName(parser, "common", "modbus_addr", mandatory, [this](const char* str)
    {
        return parseValue(common.modbusAddr, str);
    });
    return true;
}

bool ParamParser::parseGates(rlIniFile& parser, char const* gateName)
{
    if (!findSection(parser, gateName, optional))
    {
        return false;
    }

    GateParams prms;

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
        return parseValue(prms.addr, str);
    });

    parseName(parser, gateName, "request_periodicity", mandatory, [&prms](const char* str)
    {
        return parseReadPeriod(prms.readPeriod, str);
    });

    parseName(parser, gateName, "request_timeout", mandatory, [&prms](const char* str)
    {
        return parseReadPeriod(prms.readTimeout, str);
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
    
    DeviceParams prms;

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
        return parseValue(prms.addr, str);
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
    
    ParamParams prms;

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
        return parseValue(prms.func, str);
    });

    parseName(parser, paramName, "chan", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.chan, str);
    });

    parseName(parser, paramName, "addr", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.addr, str);
    });

    parseName(parser, paramName, "type", mandatory, [&prms](const char* str)
    {
        return parseValue(prms.type, str);
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