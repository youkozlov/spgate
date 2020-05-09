#pragma once

#include "Cmd.hpp"

namespace sg
{
namespace cli
{

class Cli;

class CmdShutdown : public Cmd
{
public:
    explicit CmdShutdown(Cli& cli);

    ~CmdShutdown();

    void execute(CmdParamParser const&, WrapBuffer&) final;

    static char const* getName();

private:
    Cli& cli;

};

} // namespace cli
} // namespace sg
