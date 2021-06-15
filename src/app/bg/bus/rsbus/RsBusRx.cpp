#include "RsBusRx.hpp"
#include "sockets/Link.hpp"
#include "RsBusDefs.hpp"
#include "utils/Logger.hpp"

namespace sg
{

namespace rsbus
{

RsBusRx::RsBusRx(Link& l)
    : link(l)
{
}

// 10Н
// NT
// 52Н
// ДампОЗУ (1...64) байт
// КС
// 16Н

int RsBusRx::receive(unsigned char* buf, unsigned int maxLen)
{
    unsigned int rxLen = 0;

    do
    {
        int len = link.read(buf, 1, 50);

        if (!len)
        {
            return len;
        }
        else if (len < 0)
        {
            return len;
        }
    }
    while (buf[0] != FSC);
    
    rxLen += 1;

    uint8_t sum = 0;

    while (1)
    {
        int len = link.read(&buf[rxLen], 1, 50);

        if (!len)
        {
            return len;
        }
        else if (len < 0)
        {
            return len;
        }

        rxLen += 1;
        
        if (rxLen >= maxLen)
        {
            LM(LE, "Received packet is too huge");
            return invalid;
        }

        if (buf[rxLen - 1] == FEC && sum == 0xff)
        {
            return rxLen;
        }
        else
        {
            sum += buf[rxLen - 1];
        }
    }

    return invalid;
}

int RsBusRx::receiveLong(unsigned char* buf, unsigned int maxLen)
{
    unsigned int rxLen = 0;

    unsigned timeoutCounter = 0;
    constexpr unsigned maxNumReadTimeouts = 1000;
    constexpr unsigned headerLen = 7;
    constexpr unsigned crcLen = 2;
    do
    {
        int result = link.read(&buf[rxLen], 1, 50);
        if (!result)
        {
            timeoutCounter += 1;
            if (timeoutCounter < maxNumReadTimeouts)
            {
                continue;
            }
            else if (rxLen)
            {
                LM(LE, "Timeout while receive");
                return invalid;
            }
            else
            {
                return 0;
            }
        }
        else if (result < 0)
        {
            LM(LE, "Function result(%d) is unexpected", result);
            return -1;
        }
        else
        {
            timeoutCounter = 0;
        }

        rxLen += 1;
        if (rxLen >= maxLen)
        {
            LM(LE, "Received packet is too huge");
            return invalid;
        }
    }
    while ( (rxLen < headerLen) || (rxLen < (headerLen + (buf[6] << 8) + buf[5] + crcLen)) );

    return rxLen;
}

}
}