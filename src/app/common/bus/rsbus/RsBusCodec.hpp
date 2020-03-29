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
    enum class State
    {
    };

    explicit RsBusCodec(WrapBuffer&, RsBusFrame&);

    bool encode();

    bool decode();

private:
    WrapBuffer& buf;
    RsBusFrame& frame;
};

}
}