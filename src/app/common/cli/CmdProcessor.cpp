#include "CmdProcessor.hpp"
#include "cmds/CmdExit.hpp"
#include "cmds/CmdShutdown.hpp"
#include "cmds/CmdInfo.hpp"
#include "cmds/CmdLog.hpp"
#include "cmds/CmdParamParser.hpp"
#include "Cli.hpp"
#include "utils/Utils.hpp"
#include <cstring>

namespace sg
{
namespace cli
{

namespace
{
unsigned int getHash(char const* name)
{
    return Utils::adler32(name, std::strlen(name));
}
}

CmdProcessor::CmdProcessor(Init const& init)
    : cli(init.cli)
{
    cmds[getHash(CmdExit::getName())]     = std::unique_ptr<Cmd>(new CmdExit(init.cli));
    cmds[getHash(CmdShutdown::getName())] = std::unique_ptr<CmdShutdown>(new CmdShutdown(init.cli));
    cmds[getHash(CmdInfo::getName())]     = std::unique_ptr<CmdInfo>(new CmdInfo);
    cmds[getHash(CmdLog::getName())]      = std::unique_ptr<CmdLog>(new CmdLog);
}

CmdProcessor::~CmdProcessor()
{
}

int CmdProcessor::receive(unsigned char* in, unsigned int len, WrapBuffer& txBuf)
{
    CmdParamParser cmdParser((char*)in, len);
    if (!cmdParser.parse())
    {
        return false;
    }

    unsigned int hash = Utils::adler32(cmdParser.cmdName(), strlen(cmdParser.cmdName()));
    if (auto& cmd = cmds[hash])
    {
        cmd->execute(cmdParser, txBuf);
        return 0;
    }
    else
    {
        cli.error();
    }
    return -1;
}

} // namespace cli
} // namespace sg
