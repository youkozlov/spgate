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

    do
    {
        if (link.read(&buf[rxLen], 1, 50) != 1)
        {
            LM(LE, "Function result is unexpected");
            return invalid;
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