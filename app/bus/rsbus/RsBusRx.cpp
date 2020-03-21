#include "RsBusRx.hpp"
#include "sockets/Link.hpp"
#include "RsBusDefs.hpp"
#include "utils/Logger.hpp"

namespace sg
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

    int len = link.read(buf, 3, 300);

    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        return len;
    }

    rxLen += len;

    uint16_t sum = 0;

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

        sum += buf[rxLen - 3];
    }
    while (not (buf[rxLen - 1] == FEC && buf[rxLen - 2] == static_cast<uint8_t>(~sum)));

    return rxLen;
}

}