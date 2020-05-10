#pragma once

#include "CmdArg.hpp"

namespace sg
{

class WrapBuffer;

namespace cli
{

class Cmd;

class CmdParamFormater
{
public:
    
    explicit CmdParamFormater(Cmd const&, WrapBuffer&);

    ~CmdParamFormater();

    void display(CmdArgVector const&);

private:

    void insertStaff(size_t);
    void insertDelimeter(size_t);
    void insertCRLR();

    Cmd const&  cmd;
    WrapBuffer& buf;
};

} // namespace cli
} // namespace sg
