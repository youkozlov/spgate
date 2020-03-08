#pragma once

#include <cstddef>

namespace sg
{

struct SerialPortSetup;

class SerialPort
{
public:
    virtual ~SerialPort() {}
    
    virtual int setup(SerialPortSetup const&) = 0;
    
    virtual int send(void const*, size_t) = 0;
    
    virtual int receive(void*, size_t) = 0;
};

}