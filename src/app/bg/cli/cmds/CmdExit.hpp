#pragma once

#include "Cmd.hpp"

namespace sg
{
namespace cli
{

class Cli;

class CmdExit : public Cmd
{
public:
    explicit CmdExit(Cli& cli);

    ~CmdExit();

    void execute(CmdParamParser const&, WrapBuffer&) final;

    static char const* getName();

private:
    Cli& cli;

};

} // namespace cli
} // namespace sg
