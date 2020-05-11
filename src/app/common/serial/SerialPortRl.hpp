#pragma once

#include "SerialPort.hpp"
#include "rlserial.h"

namespace sg
{

class SerialPortRl : public SerialPort
{
public:
    struct Init
    {
        char const* port;
        int speed    = B9600;
        int block    = 0;
        int rtscts   = 0;
        int bits     = 8;
        int stopbits = 2;
        int parity   = rlSerial::NONE;
        unsigned startTimeout = 100;
        unsigned endTimeout = 300;
    };
    explicit SerialPortRl(Init const&);

    ~SerialPortRl();

    int write(void const*, unsigned int);
    
    int read(void*, unsigned int);

private:
    rlSerial serial;
    unsigned const startTimeout;
    unsigned const endTimeout;
};

}