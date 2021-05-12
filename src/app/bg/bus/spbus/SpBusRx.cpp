#include "SpBusRx.hpp"
#include "SpBusDefs.hpp"

#include "sockets/Link.hpp"
#include "utils/Logger.hpp"

namespace sg
{

SpBusRx::SpBusRx(Link& l)
    : link(l)
{
}

int SpBusRx::receive(unsigned char* buf, unsigned int maxLen)
{
    unsigned int rxLen = 0;

    buf[0] = 0;

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
    while (buf[0] != DLE);

    rxLen += 1;
    
    unsigned timeoutCounter = 0;
    constexpr unsigned maxNumReadTimeouts = 1000;
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
            else
            {
                LM(LE, "Timeout while receive");
                return invalid;
            }
        }
        else if (result < 0)
        {
            LM(LE, "Function result(%d) is unexpected", result);
            return invalid;
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
    while ((rxLen < 4) || not (buf[rxLen - 4] == DLE && buf[rxLen - 3] == ETX));

    return rxLen;
}

}