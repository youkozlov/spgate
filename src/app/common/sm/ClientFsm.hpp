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
        send,
        receive,
        disconnect,
        error,
        timeout
    };

    void init();
    void connect();
    void idle();
    void send();
    void receive();
    void disconnect();
    void error();
    void timeout();

    void changeState(State);
    char const* toString(State) const;

    State   state;
    Client& client;
    int     tick;

    Timer  recvTimer;
    Timer  idleTimer;
    Timer  errorTimer;
    Timer  timeoutTimer;
};
}