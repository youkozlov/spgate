#pragma once

#include "Cmd.hpp"
#include "CmdArg.hpp"

namespace sg
{
namespace cli
{
class CmdLog : public Cmd
{
public:
    explicit CmdLog();
    ~CmdLog();

    void execute(CmdParamParser const&, WrapBuffer&) final;

    static char const* getName();

private:
    CmdArgVector args;

};

} // namespace cli
} // namespace sg
