#pragma once

#include "utils/Timer.hpp"

namespace sg
{

namespace rsbus
{

class RsBus;

class RsBusFsm
{
public:

    enum class State
    {
        init,
        idle,
        waitingForLinkLock,
        connect,
        sendStartSequence,
        sendSessionReq,
        recvSessionRsp,
        sendDataReq,
        recvDataRsp,
        disconnect,
        error
    };

    explicit RsBusFsm(RsBus&, unsigned recvTimeoutMs);

    void tickInd();

private:

    void init();
    void idle();
    void connect();
    void waitingForLinkLock();
    void sendStartSequence();
    void sendSessionReq();
    void recvSessionRsp();
    void sendDataReq();
    void recvDataRsp();
    void disconnect();
    void error();

    void changeState(State);
    char const* toString(State) const;

    RsBus& bus;
    State  state;
    int    tick;
    int    startSequenceCounter;
    Timer  sendStartSeqTimer;
    Timer  recvTimer;
    Timer  idleTimer;
    Timer  errorTimer;
};

}
}