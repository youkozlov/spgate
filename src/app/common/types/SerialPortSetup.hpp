#pragma once

namespace sg
{

struct SerialPortSetup
{
    unsigned port;
    unsigned speed;
    unsigned bits;
    unsigned stopBits;
    unsigned parity;
};

}