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

    bool isFscRcvd = false;

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

        if (isFscRcvd && buf[rxLen - 1] == FEC && sum == 0xff)
        {
            return rxLen;
        }
        else if (!isFscRcvd && buf[rxLen - 1] == FSC)
        {
            isFscRcvd = true;
        }
        else if (isFscRcvd)
        {
            sum += buf[rxLen - 1];
        }
    }

    return invalid;
}

}
}