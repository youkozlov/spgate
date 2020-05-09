#include "Utils.hpp"
#include "Logger.hpp"

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

unsigned int Utils::adler32(void const *buffer, unsigned int buflength)
{
    unsigned char const* buf = (unsigned char const*)buffer;

    unsigned int s1 = 1;
    unsigned int s2 = 0;
    for (unsigned int i = 0; i < buflength; ++i)
    {
       s1 = (s1 + buf[i]) % 65521;
       s2 = (s2 + s1) % 65521;
    }
    return (s2 << 16) | s1;
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

float Utils::reverse(float const input)
{
   float retVal;
   unsigned char *toConvert   = (unsigned char*) &input;
   unsigned char *returnValue = (unsigned char*) &retVal;

   returnValue[0] = toConvert[2];
   returnValue[1] = toConvert[3];
   returnValue[2] = toConvert[0];
   returnValue[3] = toConvert[1];

    LM(LD, "ReverseFloat: input=%f, %02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X"
        , input
        , toConvert[0]
        , toConvert[1]
        , toConvert[2]
        , toConvert[3]
        , returnValue[0]
        , returnValue[1]
        , returnValue[2]
        , returnValue[3]
        );

   return retVal;
}

int32_t Utils::reverse(int32_t const input)
{
   int32_t retVal;
   unsigned char *toConvert   = (unsigned char*) &input;
   unsigned char *returnValue = (unsigned char*) &retVal;

   returnValue[0] = toConvert[2];
   returnValue[1] = toConvert[3];
   returnValue[2] = toConvert[0];
   returnValue[3] = toConvert[1];

    LM(LD, "ReverseFixed: input=%d, %02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X"
        , input
        , toConvert[0]
        , toConvert[1]
        , toConvert[2]
        , toConvert[3]
        , returnValue[0]
        , returnValue[1]
        , returnValue[2]
        , returnValue[3]
        );

   return retVal;
}

float Utils::encodeRsBus(float const input)
{
    float retVal{};
    unsigned char* toConvert   = (unsigned char*)&input;
    unsigned char* returnValue = (unsigned char*)&retVal;

    uint16_t const mantissa = (toConvert[3] << 8) | toConvert[2];

    returnValue[0] = toConvert[0];
    returnValue[1] = toConvert[1];
    returnValue[2] = (toConvert[2] & 0x7F) | (toConvert[3] & 0x80);
    returnValue[3] = mantissa >> 7;

    return retVal;
}

float Utils::decodeRsBus(float const input)
{
    float retVal{};
    unsigned char* toConvert   = (unsigned char*)&input;
    unsigned char* returnValue = (unsigned char*)&retVal;

    returnValue[0] = toConvert[0];
    returnValue[1] = toConvert[1];
    returnValue[2] = (toConvert[2] & 0x7F) | (toConvert[3] << 7);
    returnValue[3] = (toConvert[3] >> 0x1) | (toConvert[2] & 0x80);

    return retVal;
}

}