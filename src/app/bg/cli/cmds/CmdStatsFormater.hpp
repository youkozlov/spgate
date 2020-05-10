#pragma once

#include "CmdArg.hpp"
#include <cstring>

namespace sg
{

class WrapBuffer;

namespace cli
{

class Cmd;

constexpr int maxLengthName = 32;
constexpr int maxNumStatsItems = 32;

struct Item
{
    char     name[maxLengthName];
    uint64_t value;
};

struct StatsItems
{
    char name[maxLengthName];
    Item items[maxNumStatsItems];
    int  numItems;

    StatsItems()
    {
        std::memset(this, 0, sizeof(StatsItems));
    }

    Item* next()
    {
        return &items[(numItems++) % maxNumStatsItems];
    }
};

class CmdStatsFormater
{
public:
    
    explicit CmdStatsFormater(Cmd const&, WrapBuffer&);

    ~CmdStatsFormater();

    void display(StatsItems const&);

private:

    void insertStaff(size_t);
    void insertDelimeter(size_t);
    void insertCRLR();

    Cmd const&  cmd;
    WrapBuffer& buf;
};

} // namespace cli
} // namespace sg
