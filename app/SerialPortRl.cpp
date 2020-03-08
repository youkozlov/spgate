#include "SerialPortRl.hpp"

namespace sg
{

SerialPortRl::SerialPortRl(rlSerial& s)
    : serial(s)
{
}

int SerialPortRl::setup(SerialPortSetup const&)
{
    return 0;
}
    
int SerialPortRl::send(void const*, size_t)
{
    return 0;
}
    
int SerialPortRl::receive(void*, size_t)
{
    return 0;
}

}