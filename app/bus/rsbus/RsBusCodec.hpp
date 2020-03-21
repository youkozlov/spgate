#pragma once

#include "RsBusDefs.hpp"

namespace sg
{

class WrapBuffer;
struct RsBusFrame;

class RsBusCodec
{
public:
    explicit RsBusCodec(WrapBuffer&, RsBusFrame&);

    bool encodeStartSequence();

    bool encodeSessionReq();

    bool encodeDataReq();

    bool decodeSessionRsp();

    bool decodeDataRsp();

private:
    WrapBuffer& buf;
    RsBusFrame& frame;
};
}