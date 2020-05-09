#pragma once

#include "Cmd.hpp"

namespace sg
{
namespace cli
{
class CmdInfo : public Cmd
{
public:
    explicit CmdInfo();
    ~CmdInfo();

    void execute(CmdParamParser const&, WrapBuffer&) final;

    static char const* getName();
};

} // namespace cli
} // namespace sg
