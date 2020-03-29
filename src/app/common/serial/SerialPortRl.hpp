#pragma once

#include "SerialPort.hpp"

class rlSerial;

namespace sg
{

struct SerialPortSetup;

class SerialPortRl : public SerialPort
{
public:
    explicit SerialPortRl(rlSerial&);

    int setup(SerialPortSetup const&) final;
    
    int send(void const*, size_t) final;
    
    int receive(void*, size_t) final;

private:
    rlSerial& serial;
};

}