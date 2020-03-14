#pragma once

namespace sg
{
class Utils
{
public:
    
    static void nsleep(unsigned long = 1000 * 1000);
    
    static int  crcode(unsigned char const*, int);
    
    template<typename T, unsigned int SIZE>
    static unsigned int size(T (&)[SIZE])
    {
        return SIZE;
    }
};
}