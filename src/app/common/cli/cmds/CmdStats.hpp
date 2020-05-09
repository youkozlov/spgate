#pragma once


#include "Cmd.hpp"
#include "CmdArg.hpp"


namespace sg
{
namespace cli
{

class CmdStats : public Cmd
{
public:
    explicit CmdStats();

    ~CmdStats();

    void execute(CmdParamParser const&, WrapBuffer&) final;

    static char const* getName();
};

} // namespace cli
} // namespace sg
