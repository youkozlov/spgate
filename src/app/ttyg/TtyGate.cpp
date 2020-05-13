#include "TtyGate.hpp"
#include "serial/SerialPortRl.hpp"
#include "sockets/LinkAcceptorRl.hpp"
#include "sockets/LinkRl.hpp"
#include "utils/Utils.hpp"

namespace ttyg
{

TtyGate::TtyGate(Init const& init_)
    : init(init_)
{
    parseConfig();
    createAcceptor();
    createSerial();
}

TtyGate::~TtyGate()
{
}

void TtyGate::parseConfig()
{
    if (!parser.parseFile(init.iniFileName))
    {
        throw std::runtime_error("Can't parse configuration");
    }
    if (!parser.isTtygParsed())
    {
        throw std::runtime_error("Can't find ttyg section");
    }
}

void TtyGate::createAcceptor()
{
    auto& ttyg = parser.getTtyg();

    sg::LinkAcceptorRl::Init acceptInit = {ttyg.ipAddr};
    acceptor = std::unique_ptr<sg::LinkAcceptorRl>(new sg::LinkAcceptorRl(acceptInit));
}

void TtyGate::createSerial()
{
    auto& ttyg = parser.getTtyg();

    sg::SerialPortRl::Init serialInit;
    serialInit.port = ttyg.serial.serialName;
    serialInit.speed = static_cast<int>(ttyg.serial.speed);
    serialInit.block = static_cast<int>(ttyg.serial.block);
    serialInit.rtscts = static_cast<int>(ttyg.serial.rtscts);
    serialInit.bits = static_cast<int>(ttyg.serial.bits);
    serialInit.stopbits = static_cast<int>(ttyg.serial.stopbits);
    serialInit.parity = static_cast<int>(ttyg.serial.parity);
    serialInit.startTimeout = ttyg.startRxTimeout;
    serialInit.endTimeout = ttyg.endRxTimeout;
    serial = std::unique_ptr<sg::SerialPort>(new sg::SerialPortRl(serialInit));
}

void TtyGate::tickInd()
{
    accept();
    process();
}

void TtyGate::accept()
{
    if (linkPool.empty())
    {
        return;
    }
    int fd = acceptor->accept();
    if (fd < 0)
    {
        return;
    }
    sg::LinkId id;
    if (!linkPool.alloc(id))
    {
        throw std::runtime_error("Can't allocate link object");
    }
    linkPool.get(id).setHandl(fd);
    linkIds.push_back(id);
}

int TtyGate::readFromLink(sg::Link& link)
{
    constexpr int timeout = 50;

    unsigned rxLen = 0;

    while (1)
    {
        int len = link.read(&rxBuf[rxLen], 1, timeout);
        if (!len)
        {
            return rxLen;
        }
        else if (len < 0)
        {
            return len;
        }
        if (rxLen + 1 >= rxBuf.size())
        {
            return 0;
        }
        rxLen += len;
    }

    return rxLen;
}

void TtyGate::process()
{
    for (auto it = linkIds.begin(); it != linkIds.end(); it++)
    {
        sg::LinkId const id = *it;
        auto& link = linkPool.get(id);

        int rxLen = readFromLink(link);
        if (rxLen < 0)
        {
            linkIds.erase(it++);
            link.close();
            linkPool.free(id);
            continue;
        }
        else if (!rxLen)
        {
            continue;
        }

        if (serial->write(&rxBuf[0], rxLen) != rxLen)
        {
            continue;
        }

        sg::Utils::nsleep(1000 * 1000 * 300);

        rxLen = serial->read(&rxBuf[0], rxBuf.size());

        if (rxLen < 0)
        {
            throw std::runtime_error("Can't read data from serial port");
        }
        else if (!rxLen)
        {
            continue;
        }

        if (link.write(&rxBuf[0], rxLen) != rxLen)
        {
            linkIds.erase(it++);
            link.close();
            linkPool.free(id);
        }
    }
}

} // namespace ttyg
