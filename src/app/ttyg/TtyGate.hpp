#pragma once

#include <memory>
#include <array>
#include <list>

#include "sockets/LinkPool.hpp"

namespace sg
{
class SerialPort;
class LinkAcceptor;
class Link;
}

namespace ttyg
{

enum class Mode
{
    real,
    stub
};

class TtyGate
{
public:
    struct Init
    {
        Mode        mode;
        char const* devName;
        unsigned    startTimeout;
        unsigned    endTimeout;
        unsigned    port;
    };
    explicit TtyGate(Init const&);

    ~TtyGate();

    void tickInd();

private:
    void createAcceptor();
    void createSerial();
    void createRealSerial();
    void createStubSerial();

    int readFromLink(sg::Link&);
    void accept();
    void process();

    Init const                          init;
    std::unique_ptr<sg::SerialPort>     serial;
    std::unique_ptr<sg::LinkAcceptor>   acceptor;
    sg::LinkPool                        linkPool;
    std::list<sg::LinkId>               linkIds;
    std::array<unsigned char, 4096>     rxBuf;
};

} // namespace ttyg
