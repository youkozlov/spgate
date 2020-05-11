#pragma once

namespace sg
{
class SerialPort
{
public:
    virtual ~SerialPort() {}

    virtual int write(void const*, unsigned int) = 0;
    
    virtual int read(void*, unsigned int) = 0;
};

} // namespace sg
