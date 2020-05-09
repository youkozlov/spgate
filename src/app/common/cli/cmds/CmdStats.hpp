#pragma once


#include "Cmd.hpp"
#include "CmdArg.hpp"


namespace sg
{
namespace cli
{

class Cli;

class CmdStats : public Cmd
{
public:
    explicit CmdStats(Cli& cli);

    ~CmdStats();

    void execute(CmdParamParser const&, WrapBuffer&) final;

    static char const* getName();

private:
    Cli& cli;
    CmdArgVector args;
};

} // namespace cli
} // namespace sg
