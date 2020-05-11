#include "SerialPortRl.hpp"
#include "utils/Utils.hpp"
#include <stdexcept>

namespace sg
{

SerialPortRl::SerialPortRl(Init const& init)
    : startTimeout(init.startTimeout)
    , endTimeout(init.endTimeout)
{
    int result = serial.openDevice(
          init.port
        , init.speed
        , init.block
        , init.rtscts
        , init.bits
        , init.stopbits
        , init.parity
    );
    if (result != 0)
    {
        throw std::runtime_error("Can't open serial port");
    }
}

SerialPortRl::~SerialPortRl()
{
    serial.closeDevice();
}

int SerialPortRl::write(void const* buf, unsigned int len)
{
    auto* txBuf = static_cast<unsigned char const*>(buf);
    unsigned txLen = 0;
    while (txLen < len)
    {
        int result = serial.writeChar(txBuf[txLen++]);
        if (result < 0)
        {
            return result;
        }
    }
    return txLen;
}

int SerialPortRl::read(void* buf, unsigned int len)
{
    constexpr int errorRes = -1;
    constexpr int eofRes   = -2;
    constexpr int undefRes = -5;
    bool timerExpired = false;
    unsigned timeout = startTimeout;

    auto* rxBuf = static_cast<unsigned char*>(buf);
    unsigned rxLen = 0;

    while (1)
    {
        int result = serial.readChar();
        if (result == errorRes)
        {
            return errorRes;
        }
        else if ((result == eofRes) || (result == undefRes))
        {
            if (timerExpired)
            {
                break;
            }
            else
            {
                Utils::nsleep(1000 * 1000 * timeout);
            }
            timerExpired = true;
        }
        else
        {
            if (rxLen >= len)
            {
                break;
            }
            rxBuf[rxLen++] = result;
            timeout = endTimeout;
            timerExpired = false;
        }
    }
    return rxLen;
}

}