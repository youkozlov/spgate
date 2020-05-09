#pragma once

#include <array>
#include <memory>
#include "sm/Server.hpp"
#include "sm/ServerFsm.hpp"
#include "TelnetPrinter.hpp"

namespace sg
{

class Link;
class LinkAcceptor;

namespace cli
{

class Cli;

class TelnetServer : public Server
{
public:
    struct Init
    {
        Cli&              cli;
        LinkAcceptor&     acceptor;
    };

    explicit TelnetServer(Init const&);

    ~TelnetServer();

    void tickInd();

    void close();

    int processLine(unsigned char*, unsigned int, WrapBuffer&);

private:

    char const* name() final;
    int accept() final;
    int process() final;
    void reset() final;

    ServerFsm             fsm;
    Cli&                  cli;
    LinkAcceptor&         acceptor;
    std::unique_ptr<Link> link;
    TelnetPrinter         printer;
};

} // namespace cli
} // namespace sg
