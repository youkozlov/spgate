#include "CmdArgStatsType.hpp"
#include <cstring>

namespace sg
{
namespace cli
{

CmdArgStatsType::CmdArgStatsType()
{
}

CmdArgStatsType::~CmdArgStatsType()
{
}

char const* CmdArgStatsType::name() const
{
    return "type";
}

char const* CmdArgStatsType::description() const
{
    return "Define stats type";
}

char const* CmdArgStatsType::value()
{
    return "";
}

bool CmdArgStatsType::setValue(char const*)
{
    return true;
}

} // namespace cli
} // namespace sg
