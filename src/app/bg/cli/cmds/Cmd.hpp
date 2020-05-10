#pragma once

namespace sg
{
class WrapBuffer;

namespace cli
{

class CmdArg;
class CmdParamParser;

class Cmd
{
public:
    virtual ~Cmd() {}

    virtual void execute(CmdParamParser const&, WrapBuffer&) = 0;
};

} // namespace cli
} // namespace sg
