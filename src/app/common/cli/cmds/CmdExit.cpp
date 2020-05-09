#include "CmdExit.hpp"
#include "cli/Cli.hpp"

namespace sg
{
namespace cli
{

CmdExit::CmdExit(Cli& cli_)
    : cli(cli_)
{
}

CmdExit::~CmdExit()
{
}

char const* CmdExit::getName()
{
    return "exit";
}

void CmdExit::execute(CmdParamParser const&, WrapBuffer&)
{
    cli.exit();
}

} // namespace cli
} // namespace sg
