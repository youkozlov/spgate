#pragma once

#include "SerialPort.hpp"
#include <iostream>
#include <fstream>


namespace sg
{

class SerialPortFile : public SerialPort
{
public:
    struct Init
    {
        char const* file;
    };
    explicit SerialPortFile(Init const&);

    ~SerialPortFile();

    int write(void const*, unsigned int);
    
    int read(void*, unsigned int);

private:
    std::ofstream file;

};

}