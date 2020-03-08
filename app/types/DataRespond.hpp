#pragma once

namespace sg
{

struct DataRespond
{
    bool     done;
    bool     error;
    unsigned errorNum;
    unsigned valueType;
    bool     valueBool;
    unsigned valueShort;
    long     valueLong;
    float    valueFloat;
};

}