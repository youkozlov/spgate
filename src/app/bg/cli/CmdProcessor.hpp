#pragma once

#include "cmds/Cmd.hpp"
#include <unordered_map>
#include <memory>

namespace sg
{

class WrapBuffer;

namespace cli
{

class Cli;

class CmdProcessor
{
public:
    struct Init
    {
        Cli& cli;
    };

    explicit CmdProcessor(Init const&);

    ~CmdProcessor();

    int receive(unsigned char*, unsigned int, WrapBuffer&);

private:
    unsigned int calcCmdHash(char const*);

    Cli& cli;
    std::unordered_map<unsigned int, std::unique_ptr<Cmd>> cmds;
};

} // namespace cli
} // namespace sg
