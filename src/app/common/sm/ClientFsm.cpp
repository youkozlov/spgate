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
    case State::disconnect:
        disconnect();
    break;
    case State::error:
        error();
    break;
    }

    tick += 1;
}

void ClientFsm::init()
{
    if (tick % 128 == 0)
    {
        idleTimer.set();
        changeState(State::idle);
    }
}

void ClientFsm::idle()
{
    if (idleTimer.expired())
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
    Client::Result result = client.receive();
    if (Client::Result::fail == result)
    {
        client.reset();
        errorTimer.set();
        changeState(State::error);
    }
    else if (Client::Result::done == result)
    {
        changeState(State::disconnect);
    }
    else if (Client::Result::progress == result)
    {
        changeState(State::send);
    }
    else if (recvTimer.expired())
    {
        Client::Result tres = client.timeout();
        if (Client::Result::progress == tres)
        {
            changeState(State::send);
        }
        else if (Client::Result::done == tres)
        {
            changeState(State::disconnect);
        }
    }
}

void ClientFsm::disconnect()
{
    client.disconnect();
    idleTimer.set();
    changeState(State::idle);
}

void ClientFsm::error()
{
    if (errorTimer.expired())
    {
        changeState(State::init);
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
    case State::disconnect:
        return "Disconnect";
    case State::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}