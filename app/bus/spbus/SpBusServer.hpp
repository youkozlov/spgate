#pragma once

#include <memory>
#include <array>

#include "sm/Server.hpp"
#include "sm/ServerFsm.hpp"
#include "SpBusRx.hpp"
#include "utils/Buffer.hpp"

namespace sg
{

class Link;
class LinkAcceptor;

class SpBusServer : public Server
{
public:
    struct Init
    {
        Buffer<float>& buffer;
        LinkAcceptor&  acceptor;
    };

    explicit SpBusServer(Init const&);

    ~SpBusServer();

    void tickInd();

    char const* name() final;

    int accept() final;

    int process() final;

    void reset() final;

private:

    ServerFsm                       fsm;
    Buffer<float>&                  buffer;
    LinkAcceptor&                   acceptor;
    std::unique_ptr<Link>           link;
    SpBusRx                         rx;
    std::array<unsigned char, 1024> rawBuffer;

};

}