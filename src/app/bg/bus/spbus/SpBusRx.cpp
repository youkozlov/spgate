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

    int len = link.read(buf, 8, 50);

    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        return len;
    }
    rxLen += len;

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
    while (not (buf[rxLen - 4] == DLE && buf[rxLen - 3] == ETX));

    return rxLen;
}

}