#include "CmdShutdown.hpp"
#include "cli/Cli.hpp"

namespace sg
{
namespace cli
{

CmdShutdown::CmdShutdown(Cli& cli_)
    : cli(cli_)
{
}

CmdShutdown::~CmdShutdown()
{
}

char const* CmdShutdown::getName()
{
    return "shutdown";
}

void CmdShutdown::execute(CmdParamParser const&, WrapBuffer&)
{
    cli.shutdown();
}

} // namespace cli
} // namespace sg
