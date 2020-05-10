#include "CmdParamFormater.hpp"
#include "Cmd.hpp"
#include "CmdArg.hpp"
#include "utils/WrapBuffer.hpp"

namespace sg
{
namespace cli
{

CmdParamFormater::CmdParamFormater(Cmd const& cmd_, WrapBuffer& buf_)
    : cmd(cmd_)
    , buf(buf_)
{
}

CmdParamFormater::~CmdParamFormater()
{
}

void CmdParamFormater::insertCRLR()
{
    buf.write("\r\n", 2);
}

void CmdParamFormater::insertDelimeter(size_t lineLen)
{
    for (size_t i = 0; i < lineLen; ++i)
    {
        buf.write('=');
    }
    insertCRLR();
}

void CmdParamFormater::insertStaff(size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        buf.write(' ');
    }
}

void CmdParamFormater::display(CmdArgVector const& args)
{
    insertCRLR();

    char const* prmHeader = "ParamName";
    char const* valHeader = "Value";
    char const* desHeader = "Description";

    size_t prmMaxLen = std::strlen(prmHeader);
    size_t valMaxLen = std::strlen(valHeader);
    size_t desMaxLen = std::strlen(desHeader);

    for (auto& it : args)
    {
        prmMaxLen = std::max(prmMaxLen, std::strlen(it->name()));
        valMaxLen = std::max(valMaxLen, std::strlen(it->value()));
        desMaxLen = std::max(desMaxLen, std::strlen(it->description()));
    }

    prmMaxLen = (prmMaxLen + 4) & (~0x3);
    valMaxLen = (valMaxLen + 4) & (~0x3);
    desMaxLen = (desMaxLen + 4) & (~0x3);

    size_t const lineLen = prmMaxLen + valMaxLen + desMaxLen;

    insertDelimeter(lineLen);

    buf.write(prmHeader, std::strlen(prmHeader));
    insertStaff(prmMaxLen - std::strlen(prmHeader));

    buf.write(valHeader, std::strlen(valHeader));
    insertStaff(valMaxLen - std::strlen(valHeader));

    buf.write(desHeader, std::strlen(desHeader));
    insertStaff(desMaxLen - std::strlen(desHeader));

    insertCRLR();

    insertDelimeter(lineLen);

    for (auto& it : args)
    {
        char const* name = it->name();
        buf.write(name, std::strlen(name));
        insertStaff(prmMaxLen - std::strlen(name));

        char const* val = it->value();
        buf.write(val, std::strlen(val));
        insertStaff(valMaxLen - std::strlen(val));

        char const* desc = it->description();
        buf.write(desc, std::strlen(desc));
        insertStaff(desMaxLen - std::strlen(desc));

        insertCRLR();
    }

    insertDelimeter(lineLen);
}

} // namespace cli
} // namespace sg
