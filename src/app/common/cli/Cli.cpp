#include "Cli.hpp"
#include "sockets/LinkAcceptorRl.hpp"
#include "BusGate.hpp"
#include "TelnetServer.hpp"
#include "CmdProcessor.hpp"

namespace sg
{
namespace cli
{

Cli::Cli(Init const& init)
    : bg(init.bg)
{
    CmdProcessor::Init cmdInit{*this};
    cmdProc = std::unique_ptr<CmdProcessor>(new CmdProcessor(cmdInit));

    LinkAcceptorRl::Init acceptInit = {init.telnetAddr};
    linkAcceptor = std::unique_ptr<LinkAcceptor>(new LinkAcceptorRl(acceptInit));

    TelnetServer::Init telnetInit{*this, *linkAcceptor};
    telnet = std::unique_ptr<TelnetServer>(new TelnetServer(telnetInit));
}

Cli::~Cli()
{
}

void Cli::tickInd()
{
    telnet->tickInd();
}

int Cli::receive(unsigned char* in, unsigned int len, WrapBuffer& txBuf)
{
    return cmdProc->receive(in, len, txBuf);
}

void Cli::exit()
{
    telnet->close();
}

void Cli::shutdown()
{
    bg.shutdown();
}

void Cli::error()
{
}

ModbusStats const& Cli::getModbusStats() const
{
    return bg.getModbusStats();
}

BusStats const& Cli::getSpBusStats() const
{
    return bg.getSpBusStats();
}

BusStats const& Cli::getRsBusStats() const
{
    return bg.getRsBusStats();
}

} // namespace cli
} // namespace sg
