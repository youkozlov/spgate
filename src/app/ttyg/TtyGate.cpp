#include "TtyGate.hpp"
#include "serial/SerialPortRl.hpp"
#include "serial/SerialPortFile.hpp"
#include "sockets/LinkAcceptorRl.hpp"
#include "sockets/LinkRl.hpp"
#include "utils/Utils.hpp"

namespace ttyg
{

TtyGate::TtyGate(Init const& init_)
    : init(init_)
{
    createAcceptor();
    createSerial();
}

TtyGate::~TtyGate()
{
}

void TtyGate::createAcceptor()
{
    sg::IpAddr addr{"", init.port};
    sg::LinkAcceptorRl::Init acceptInit = {addr};
    acceptor = std::unique_ptr<sg::LinkAcceptorRl>(new sg::LinkAcceptorRl(acceptInit));
}

void TtyGate::createSerial()
{
    switch (init.mode)
    {
    case Mode::real:
        createRealSerial();
        break;
    case Mode::stub:
        createStubSerial();
        break;
    }
}

void TtyGate::createRealSerial()
{
    sg::SerialPortRl::Init serialInit;
    serialInit.startTimeout = init.startTimeout;
    serialInit.endTimeout = init.endTimeout;
    serialInit.port = init.devName;
    serial = std::unique_ptr<sg::SerialPort>(new sg::SerialPortRl(serialInit));
}

void TtyGate::createStubSerial()
{
    sg::SerialPortFile::Init serialInit;
    serialInit.file = init.devName;
    serial = std::unique_ptr<sg::SerialPort>(new sg::SerialPortFile(serialInit));
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

    int rxLen = 0;

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
        if (rxLen + len >= rxBuf.size())
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
