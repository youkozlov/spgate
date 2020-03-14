#pragma once

#include <memory>
#include <array>

#include "interfaces/Server.hpp"
#include "sm/ServerFsm.hpp"
#include "SpBusRx.hpp"

namespace sg
{

class Link;
class LinkAcceptor;

class ServerSpBus : public Server
{
public:
    struct Init
    {
        LinkAcceptor& acceptor;
    };

    explicit ServerSpBus(Init const&);

    ~ServerSpBus();

    void tickInd();

    char const* name() final;

    int accept() final;

    int process() final;

    void reset() final;

private:

    ServerFsm                       fsm;
    LinkAcceptor&                   acceptor;
    std::unique_ptr<Link>           link;
    SpBusRx                         rx;
    std::array<unsigned char, 1024> rawBuffer;

};

}