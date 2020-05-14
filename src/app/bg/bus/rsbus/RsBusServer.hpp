#pragma once

#include <memory>
#include <array>

#include "Bus.hpp"
#include "types/IpAddr.hpp"
#include "sm/Server.hpp"
#include "sm/ServerFsm.hpp"
#include "RsBusRx.hpp"
#include "utils/Buffer.hpp"

namespace sg
{

class Link;
class LinkAcceptor;

namespace rsbus
{

class RsBusServer;

class RsBusServerFsm
{
public:
    enum class State
    {
        init,
        recvStartSequence,
        recvSessionReq,
        recvDataReq,
    };
    explicit RsBusServerFsm(RsBusServer&);

    int  process();

    void reset();

private:
    int init();
    int recvStartSequence();
    int recvSessionReq();
    int recvDataReq();

    void changeState(State);
    char const* toString(State) const;

    RsBusServer& bus;
    State        state;
    unsigned int startSequenceCounter;
};




class RsBusServer : public Bus, public Server
{
public:
    struct Init
    {
        Buffer<float>& buffer;
        IpAddr const&  ipAddr;
    };

    explicit RsBusServer(Init const&);

    ~RsBusServer();

    void tickInd() final;

    char const* name() final;

    int accept() final;

    int process() final;

    void reset() final;

    int recvStartSequence();

    int recvSessionReq();

    int recvDataReq();

private:

    ServerFsm                       fsm;
    Buffer<float>&                  buffer;
    std::unique_ptr<LinkAcceptor>   acceptor;
    std::unique_ptr<Link>           link;
    RsBusRx                         rx;
    RsBusServerFsm                  busFsm;
    std::array<unsigned char, 1024> rawBuffer;
};

}
}