#pragma once

#include "cstring"

namespace sg
{

class WrapBuffer;

const unsigned maxSpBusHeaderDataSize      = 27;
const unsigned maxSpBusDataSetItemSize     = 30;
const unsigned maxSpBusDataSetPointersSize = 4;

struct SpBusHeader
{
    unsigned char dad = 0xff;
    unsigned char sad;
    unsigned char fc;
    unsigned char size;
    char          data[maxSpBusHeaderDataSize + 1];
};

struct SpBusDataSetItem
{
    unsigned char size;
    char          param[maxSpBusDataSetItemSize + 1];
};

struct SpBusDataSetPointer
{
    SpBusDataSetItem chan;
    SpBusDataSetItem prm;
};

struct SpBusDataSetInfo
{
    SpBusDataSetItem value;
    SpBusDataSetItem unit;
    SpBusDataSetItem ts;
};

struct SpBusDataSet
{
    unsigned char       numPointers;
    SpBusDataSetPointer pointers[maxSpBusDataSetPointersSize];
    unsigned char       numInfos;
    SpBusDataSetInfo    infos[maxSpBusDataSetPointersSize];
};

struct SpBusFrame
{
    SpBusHeader  hdr;
    SpBusDataSet data;

    SpBusFrame()
    {
        std::memset(this, 0, sizeof(SpBusFrame));
    }
};

class SpBusCodec
{
public:

    explicit SpBusCodec(WrapBuffer&, SpBusFrame&);

    bool decode();

    bool encode();

private:
    enum class State
    {
        procCmd,
        procDad,
        procSad,
        procFunc,
        procDataHead,
        procDataSet,
        idle,
        done,
        error
    };

    enum class SubState
    {
        procPointer,
        procChan,
        procPrm,
        procInfo,
        procValue,
        procUnit,
        procTs,
    };

    void procCmd(unsigned char);
    void procDad(unsigned char);
    void procSad(unsigned char);
    void procFunc(unsigned char);
    void procDataHead(unsigned char);
    void procDataSet(unsigned char);

    void procPointer(unsigned char);
    void procChan(unsigned char);
    void procPrm(unsigned char);
    void procInfo(unsigned char);
    void procValue(unsigned char);
    void procUnit(unsigned char);
    void procTs(unsigned char);

    bool encodeRdp();
    bool encodeRsp();

    void changeState(State);
    void changeSubState(SubState);
    char const* toString(State) const;
    char const* toString(SubState) const;

    State       state;
    SubState    subs;
    WrapBuffer& buf;
    SpBusFrame& frame;
};

}