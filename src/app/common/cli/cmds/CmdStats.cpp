#include "CmdStats.hpp"

namespace sg
{
namespace cli
{

CmdStats::CmdStats()
{
}

CmdStats::~CmdStats()
{
}

void CmdStats::execute(CmdParamParser const&, WrapBuffer&)
{
}

char const* CmdStats::getName()
{
    return "stats";
}

} // namespace cli
} // namespace sg
