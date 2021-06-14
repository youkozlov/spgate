#pragma once

#include "utils/Timer.hpp"

namespace sg
{

class Client;

class ClientFsm
{
public:
    explicit ClientFsm(Client&, unsigned recvTimeoutMs);

    ~ClientFsm();

    void tickInd();

private:
    enum class State
    {
        init,
        connect,
        idle,
        waitingForLinkLock,
        send,
        receive,
        disconnect,
        error
    };

    void init();
    void connect();
    void idle();
    void waitingForLinkLock();
    void send();
    void receive();
    void disconnect();
    void error();

    void changeState(State);
    char const* toString(State) const;

    State   state;
    Client& client;
    int     tick;

    Timer  recvTimer;
    Timer  idleTimer;
    Timer  errorTimer;
};
}