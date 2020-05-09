#include "TelnetServer.hpp"

#include "TelnetDefs.hpp"
#include "sockets/LinkRl.hpp"
#include "sockets/LinkAcceptor.hpp"
#include "utils/Logger.hpp"
#include "utils/WrapBuffer.hpp"
#include "Cli.hpp"

namespace sg
{

namespace cli
{

TelnetServer::TelnetServer(Init const& init)
    : fsm(*this)
    , cli(init.cli)
    , acceptor(init.acceptor)
    , link(std::unique_ptr<LinkRl>(new LinkRl(-1)))
    , printer(*link, *this)
{
}

TelnetServer::~TelnetServer()
{
}

void TelnetServer::tickInd()
{
    fsm.tickInd();
}

void TelnetServer::close()
{
    fsm.close();
}

char const* TelnetServer::name()
{
    return "TelnetServer";
}

int TelnetServer::accept()
{
    int fd = acceptor.accept();
    if (fd < 0)
    {
        return 0;
    }
    link->setHandl(fd);

    if (printer.init())
    {
        return 0;
    }

    return 1;
}

int TelnetServer::process()
{
    return printer.process();
}

int TelnetServer::processLine(unsigned char* in, unsigned int len, WrapBuffer& txBuf)
{
    return cli.receive(in, len, txBuf);
}

void TelnetServer::reset()
{
    link->close();
}

} // namespace cli
} // namespace sg