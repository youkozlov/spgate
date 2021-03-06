#pragma once

#include <cstdint>

namespace sg
{
class Utils
{
public:
    
    static void nsleep(unsigned long = 1000 * 1000);

    static unsigned int adler32(void const *, unsigned int);

    static int  crcode(unsigned char const*, int);
    
    template<typename T, unsigned int SIZE>
    static unsigned int size(T (&)[SIZE])
    {
        return SIZE;
    }

    static float   reverse(float const);

    static int32_t reverse(int32_t const);

    static float   encodeRsBus(float const);

    static float   decodeRsBus(float const);
};
}