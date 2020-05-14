#include "CmdArgLogLevel.hpp"
#include <cstdlib>
#include "utils/Logger.hpp"

namespace sg
{
namespace cli
{
CmdArgLogLevel::CmdArgLogLevel()    
{
}

CmdArgLogLevel::~CmdArgLogLevel()
{
}

char const* CmdArgLogLevel::name() const
{
    return "level";
}

char const* CmdArgLogLevel::description() const
{
    return "Define logging level";
}

char const* CmdArgLogLevel::value()
{
    LogLevel lvl = Logger::getInst().currentLogLevel();
    sprintf(buf, "%u", lvl);
    return buf;
}

bool CmdArgLogLevel::setValue(char const* buf)
{
    LogLevel lvl;
    switch (std::atoi(buf))
    {
    case 0:
        lvl = LogLevel::LD;
    break;
    case 1:
        lvl = LogLevel::LI;
    break;
    case 2:
        lvl = LogLevel::LW;
    break;
    case 3:
        lvl = LogLevel::LE;
    break;
    case 4:
        lvl = LogLevel::NA;
    break;
    default:
        return false;
    }
    Logger::getInst().setLogLevel(lvl);
    return true;
}

} // namespace cli
} // namespace sg
