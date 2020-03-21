#pragma once

#include "utils/Timer.hpp"

namespace sg
{

class RsBus;

class RsBusFsm
{
public:

    enum class State
    {
        init,
        connect,
        sendStartSequence,
        sendSessionReq,
        recvSessionRsp,
        idle,
        sendDataReq,
        recvDataRsp,
        error,
        timeout
    };

    explicit RsBusFsm(RsBus&);

    void tickInd();

private:

    void init();
    void connect();
    void sendStartSequence();
    void sendSessionReq();
    void recvSessionRsp();
    void idle();
    void sendDataReq();
    void recvDataRsp();
    void error();
    void timeout();

    void changeState(State);
    char const* toString(State) const;

    RsBus& bus;
    State  state;
    int    tick;
    Timer  recvTimer;
    Timer  idleTimer;
    Timer  errorTimer;
    Timer  timeoutTimer;
};

}