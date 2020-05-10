#include "CmdParamParser.hpp"
#include <cstring>

namespace sg
{
namespace cli
{

CmdParamParser::CmdParamParser(char* buf_, unsigned int len_)
    : buf(buf_)
    , len(len_)
    , num(0)
{
}

CmdParamParser::~CmdParamParser()
{
}

char const* CmdParamParser::cmdName() const
{
    return buf;
}

unsigned int CmdParamParser::numArgs() const
{
    return num;
}

CmdParamParser::Item const& CmdParamParser::arg(unsigned int i) const
{
    return args[i];
}

int CmdParamParser::find(char const* name, bool needParam) const
{
    for (unsigned int i = 0; i < num; ++i)
    {
        if (strcmp(name, args[i].name) == 0)
        {
            return needParam ? (args[i].param ? i : -1) : i;
        }
    }
    return -1;
}

bool CmdParamParser::parse()
{
    char* pos = std::strchr(buf, ' ');

    if (!pos && len)
    {
        return true;
    }
    else if (pos && pos == buf)
    {
        return false;
    }

    *pos++ = 0x00;

    char* endPos = buf + len;

    while (pos < endPos && *pos == ' ')
    {
        pos += 1;
    }

    if (pos >= endPos)
    {
        return false;
    }

    char* namePtr = pos;

    while (pos < endPos && *pos != '=')
    {
        pos += 1;
    }

    *pos++ = 0x00;

    if (pos >= endPos)
    {
        args[num++] = CmdParamParser::Item{namePtr, nullptr};
        return true;
    }

    char* nameParam = pos;

    while (pos < endPos && *pos != ' ')
    {
        pos += 1;
    }

    *pos++ = 0x00;

    if (pos >= endPos)
    {
        args[num++] = CmdParamParser::Item{namePtr, nameParam};
        return true;
    }

    return false;
}

} // namespace cli
} // namespace sg
