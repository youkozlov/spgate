#pragma once

#include "RsBusDefs.hpp"

namespace sg
{

class WrapBuffer;

namespace rsbus
{

struct RsBusFrame;

class RsBusCodec
{
public:

    explicit RsBusCodec(WrapBuffer&, RsBusFrame&);

    bool encode();
    bool encodeReqLong();
    bool encodeRspLong();

    bool decode();
    bool decodeRspLong();
    bool decodeReqLong();

private:
    WrapBuffer& buf;
    RsBusFrame& frame;
};

}
}