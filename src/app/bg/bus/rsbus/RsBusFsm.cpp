#include "RsBusFsm.hpp"
#include "RsBus.hpp"
#include "RsBusDefs.hpp"
#include "utils/Logger.hpp"

namespace sg
{

namespace rsbus
{

constexpr unsigned int startSeqSendPeriodMs = 5;

RsBusFsm::RsBusFsm(RsBus& b, unsigned recvTimeoutMs)
    : bus(b)
    , state(State::init)
    , tick(0)
    , startSequenceCounter(0)
    , sendStartSeqTimer(startSeqSendPeriodMs)
    , recvTimer(recvTimeoutMs)
    , idleTimer(bus.period())
    , errorTimer(recvTimeoutMs * 2)
{
}

void RsBusFsm::tickInd()
{
    switch (state)
    {
    case State::init:
        init();
    break;
    case State::connect:
        connect();
    break;
    case State::waitingForLinkLock:
        waitingForLinkLock();
    break;
    case State::sendStartSequence:
        sendStartSequence();
    break;
    case State::sendSessionReq:
        sendSessionReq();
    break;
    case State::recvSessionRsp:
        recvSessionRsp();
    break;
    case State::idle:
        idle();
    break;
    case State::sendDataReq:
        sendDataReq();
    break;
    case State::recvDataRsp:
        recvDataRsp();
    break;
    case State::disconnect:
        disconnect();
    break;
    case State::error:
        error();
    break;
    }

    tick += 1;
}

void RsBusFsm::init()
{
    if (tick % 128 == 0)
    {
        changeState(State::idle);
    }
}

void RsBusFsm::idle()
{
    if (idleTimer.expired())
    {
        changeState(State::waitingForLinkLock);
    }
}

void RsBusFsm::waitingForLinkLock()
{
    if (bus.tryLock())
    {
        changeState(State::connect);
    }
}

void RsBusFsm::connect()
{
    if (bus.connect() < 0)
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else
    {
        startSequenceCounter = 0;
        sendStartSeqTimer.set();
        changeState(State::sendStartSequence);
    }
}

void RsBusFsm::sendStartSequence()
{
    if (not sendStartSeqTimer.expired())
    {
        return;
    }

    if (bus.sendStartSequence() < 0)
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else if (startSequenceCounter >= startSequenceLength)
    {
        changeState(State::sendSessionReq);
    }
    else
    {
        sendStartSeqTimer.set();
        startSequenceCounter += 1;
    }
}

void RsBusFsm::sendSessionReq()
{
    if (bus.sendSessionReq() < 0)
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else
    {
        recvTimer.set();
        changeState(State::recvSessionRsp);
    }
}

void RsBusFsm::recvSessionRsp()
{
    int result = bus.recvSessionRsp();
    if (result < 0)
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else if (recvTimer.expired())
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else if (result)
    {
        changeState(State::sendDataReq);
    }
}

void RsBusFsm::sendDataReq()
{
    if (bus.sendDataReq() < 0)
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else
    {
        recvTimer.set();
        changeState(State::recvDataRsp);
    }
}

void RsBusFsm::recvDataRsp()
{
    RsBus::Result result = bus.recvDataRsp();
    if (RsBus::Result::fail == result)
    {
        bus.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else if (RsBus::Result::done == result)
    {
        changeState(State::disconnect);
    }
    else if (RsBus::Result::progress == result)
    {
        changeState(State::sendDataReq);
    }
    else if (recvTimer.expired())
    {
        RsBus::Result tres = bus.timeout();
        if (RsBus::Result::progress == tres)
        {
            changeState(State::sendDataReq);
        }
        else if (RsBus::Result::done == tres)
        {
            changeState(State::disconnect);
        }
    }
}

void RsBusFsm::disconnect()
{
    bus.disconnect();
    idleTimer.set();
    changeState(State::idle);
}

void RsBusFsm::error()
{
    if (errorTimer.expired())
    {
        idleTimer.set();
        changeState(State::idle);
    }
}

void RsBusFsm::changeState(State newSt)
{
    LM(LD, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* RsBusFsm::toString(State st) const
{
    switch (st)
    {
    case State::init:
        return "Init";
    case State::idle:
        return "Idle";
    case State::connect:
        return "Connect";
    case State::waitingForLinkLock:
        return "WaitingForLinkLock";
    case State::sendStartSequence:
        return "SendStartSequence";
    case State::sendSessionReq:
        return "SendSessionReq";
    case State::recvSessionRsp:
        return "RecvSessionRsp";
    case State::sendDataReq:
        return "SendDataReq";
    case State::recvDataRsp:
        return "RecvDataRsp";
    case State::disconnect:
        return "Disconnect";
    case State::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}
}