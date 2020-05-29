#include "ClientFsm.hpp"

#include "Client.hpp"

#include "utils/Logger.hpp"
#include "utils/TickUtils.hpp"
#include <algorithm>

namespace sg
{

ClientFsm::ClientFsm(Client& client_, unsigned recvTimeoutMs)
    : state(State::init)
    , client(client_)
    , tick(0)
    , recvTimer(recvTimeoutMs)
    , idleTimer(client.period())
    , errorTimer(recvTimeoutMs * 2)
    , timeoutTimer(recvTimeoutMs * 2)
{
}

ClientFsm::~ClientFsm()
{
}

void ClientFsm::tickInd()
{
    switch (state)
    {
    case State::init:
        init();
    break;
    case State::connect:
        connect();
    break;
    case State::idle:
        idle();
    break;
    case State::send:
        send();
    break;
    case State::receive:
        receive();
    break;
    case State::error:
        error();
    break;
    case State::timeout:
        timeout();
    break;
    }

    tick += 1;
}

void ClientFsm::init()
{
    if (tick % 128 == 0)
    {
        changeState(State::connect);
    }
}

void ClientFsm::connect()
{
    if (client.connect() < 0)
    {
        client.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else
    {
        changeState(State::send);
    }
}

void ClientFsm::idle()
{
    if (idleTimer.expired())
    {
        changeState(State::send);
    }
}

void ClientFsm::send()
{
    if (client.send() < 0)
    {
        client.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else
    {
        recvTimer.set();
        changeState(State::receive);
    }
}

void ClientFsm::receive()
{
    int result = client.receive();
    if (result < 0)
    {
        client.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else if (recvTimer.expired())
    {
        client.timeout();
        timeoutTimer.set();
        changeState(State::timeout);
    }
    else if (result)
    {
        idleTimer.set();
        changeState(State::idle);
    }
}

void ClientFsm::error()
{
    if (errorTimer.expired())
    {
        changeState(State::init);
    }
}

void ClientFsm::timeout()
{
    if (timeoutTimer.expired())
    {
        idleTimer.set();
        changeState(State::idle);
    }
}

void ClientFsm::changeState(State newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* ClientFsm::toString(State st) const
{
    switch (st)
    {
    case State::init:
        return "Init";
    case State::connect:
        return "Connect";
    case State::idle:
        return "Idle";
    case State::send:
        return "Send";
    case State::receive:
        return "Receive";
    case State::error:
        return "Error";
    case State::timeout:
        return "Timeout";
    default:
        return "Invalid";
    }
}

}