#pragma once

#include <memory>
#include <array>
#include <list>

#include "utils/ParamParser.hpp"
#include "sockets/LinkPool.hpp"

namespace sg
{
class SerialPort;
class LinkAcceptor;
class Link;
}

namespace ttyg
{

class TtyGate
{
public:
    struct Init
    {
        char const* iniFileName;
    };
    explicit TtyGate(Init const&);

    ~TtyGate();

    void tickInd();

private:

    void parseConfig();
    void createAcceptor();
    void createSerial();

    int readFromLink(sg::Link&);
    void accept();
    void process();

    Init const                          init;
    sg::ParamParser                     parser;
    std::unique_ptr<sg::SerialPort>     serial;
    std::unique_ptr<sg::LinkAcceptor>   acceptor;
    sg::LinkPool                        linkPool;
    std::list<sg::LinkId>               linkIds;
    std::array<unsigned char, 4096>     rxBuf;
};

} // namespace ttyg
