#include "Utils.hpp"
#include <time.h>
 
namespace sg
{
    
void Utils::nsleep(unsigned long ns)
{
    struct timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = ns; // 1мс
    nanosleep(&tv, NULL);
}

int Utils::crcode(unsigned char const* msg, int len)
{
    int crc = 0;
    while (len-- > 0)
    {
        crc = crc ^ (int)*msg++ << 8;
        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

}