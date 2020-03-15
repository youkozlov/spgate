#include "ClientFsm.hpp"

#include "Client.hpp"

#include "utils/Logger.hpp"

namespace sg
{

ClientFsm::ClientFsm(Client& g)
    : state(ClientFsmState::init)
    , client(g)
    , tick(0)
{
}

ClientFsm::~ClientFsm()
{
}

void ClientFsm::tickInd()
{
    switch (state)
    {
    case ClientFsmState::init:
        init();
    break;
    case ClientFsmState::connect:
        connect();
    break;
    case ClientFsmState::idle:
        idle();
    break;
    case ClientFsmState::send:
        send();
    break;
    case ClientFsmState::receive:
        receive();
    break;
    case ClientFsmState::error:
        error();
    break;
    }

    tick += 1;
}

void ClientFsm::init()
{
    if (tick % 128 == 0)
    {
        changeState(ClientFsmState::connect);
    }
}

void ClientFsm::connect()
{
    if (client.connect() < 0)
    {
        client.reset();
        changeState(ClientFsmState::error);
    }
    else
    {
        changeState(ClientFsmState::idle);
    }
}

void ClientFsm::idle()
{
    if (tick % client.period() == 0)
    {
        changeState(ClientFsmState::send);
    }
}

void ClientFsm::send()
{
    if (client.send() < 0)
    {
        client.reset();
        changeState(ClientFsmState::error);
    }
    else
    {
        changeState(ClientFsmState::receive);
    }
}

void ClientFsm::receive()
{
    int result = client.receive();
    if (result < 0)
    {
        client.reset();
        changeState(ClientFsmState::error);
    }
    else if (result)
    {
        changeState(ClientFsmState::idle);
    }
}

void ClientFsm::error()
{
    if (tick % 1024 == 0)
    {
        changeState(ClientFsmState::init);
    }
}

void ClientFsm::changeState(ClientFsmState newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* ClientFsm::toString(ClientFsmState st) const
{
    switch (st)
    {
    case ClientFsmState::init:
        return "Init";
    case ClientFsmState::connect:
        return "Connect";
    case ClientFsmState::idle:
        return "Idle";
    case ClientFsmState::send:
        return "Send";
    case ClientFsmState::receive:
        return "Receive";
    case ClientFsmState::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}