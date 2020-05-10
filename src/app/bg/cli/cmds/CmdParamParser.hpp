#pragma once

#include <array>

namespace sg
{
namespace cli
{

class CmdParamParser
{
public:
    struct Item
    {
        char const* name;
        char const* param;
    };

    explicit CmdParamParser(char*, unsigned int);
    
    ~CmdParamParser();

    bool parse();

    char const* cmdName() const;

    unsigned int numArgs() const;

    CmdParamParser::Item const& arg(unsigned int) const;

    int find(char const*, bool needParam) const;

private:
    char* buf;
    unsigned int const len;
    
    unsigned int num;
    std::array<CmdParamParser::Item, 8> args;
};

} // namespace cli
} // namespace sg
