#include "CmdStatsFormater.hpp"
#include "Cmd.hpp"
#include "CmdArg.hpp"
#include "utils/WrapBuffer.hpp"
#include <cstdlib>

namespace sg
{
namespace cli
{

CmdStatsFormater::CmdStatsFormater(Cmd const& cmd_, WrapBuffer& buf_)
    : cmd(cmd_)
    , buf(buf_)
{
}

CmdStatsFormater::~CmdStatsFormater()
{
}

void CmdStatsFormater::insertCRLR()
{
    buf.write("\r\n", 2);
}

void CmdStatsFormater::insertDelimeter(size_t lineLen)
{
    for (size_t i = 0; i < lineLen; ++i)
    {
        buf.write('=');
    }
    insertCRLR();
}

void CmdStatsFormater::insertStaff(size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        buf.write(' ');
    }
}

void CmdStatsFormater::display(StatsItems const& stats)
{
    insertCRLR();

    buf.write(stats.name, std::strlen(stats.name));

    insertCRLR();

    char const* prmHeader = "ParamName";
    char const* valHeader = "Value";

    size_t prmMaxLen = std::strlen(prmHeader);
    size_t valMaxLen = std::strlen(valHeader);

    for (int i = 0; i < stats.numItems; ++i)
    {
        prmMaxLen = std::max(prmMaxLen, std::strlen(stats.items[i].name));
        valMaxLen = std::max(valMaxLen, std::to_string(stats.items[i].value).length());
    }

    prmMaxLen = (prmMaxLen + 4) & (~0x3);
    valMaxLen = (valMaxLen + 4) & (~0x3);

    size_t const lineLen = prmMaxLen + valMaxLen;

    insertDelimeter(lineLen);

    buf.write(prmHeader, std::strlen(prmHeader));
    insertStaff(prmMaxLen - std::strlen(prmHeader));

    buf.write(valHeader, std::strlen(valHeader));
    insertStaff(valMaxLen - std::strlen(valHeader));

    insertCRLR();

    insertDelimeter(lineLen);

    for (int i = 0; i < stats.numItems; ++i)
    {
        char const* name = stats.items[i].name;
        buf.write(name, std::strlen(name));
        insertStaff(prmMaxLen - std::strlen(name));

        char const* val = std::to_string(stats.items[i].value).c_str();
        buf.write(val, std::strlen(val));
        insertStaff(valMaxLen - std::strlen(val));

        insertCRLR();
    }

    insertDelimeter(lineLen);
}

} // namespace cli
} // namespace sg
