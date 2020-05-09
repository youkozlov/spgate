#include "CmdInfo.hpp"

namespace sg
{
namespace cli
{

CmdInfo::CmdInfo()
{
}

CmdInfo::~CmdInfo()
{
}

char const* CmdInfo::getName()
{
    return "info";
}

void CmdInfo::execute(CmdParamParser const&, WrapBuffer&)
{
}

} // namespace cli
} // namespace sg
