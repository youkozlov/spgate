#include "GateFsm.hpp"

#include "interfaces/Gate.hpp"

#include "utils/Logger.hpp"

namespace sg
{

GateFsm::GateFsm(Gate& g)
    : state(GateFsmState::init)
    , gate(g)
    , tick(0)
{
}

GateFsm::~GateFsm()
{
}

void GateFsm::tickInd()
{
    switch (state)
    {
    case GateFsmState::init:
        init();
    break;
    case GateFsmState::connect:
        connect();
    break;
    case GateFsmState::idle:
        idle();
    break;
    case GateFsmState::send:
        send();
    break;
    case GateFsmState::receive:
        receive();
    break;
    case GateFsmState::error:
        error();
    break;
    }

    tick += 1;
}

void GateFsm::init()
{
    if (tick % 128 == 0)
    {
        changeState(GateFsmState::connect);
    }
}

void GateFsm::connect()
{
    if (gate.connect() < 0)
    {
        gate.reset();
        changeState(GateFsmState::error);
    }
    else
    {
        changeState(GateFsmState::idle);
    }
}

void GateFsm::idle()
{
    if (tick % gate.period() == 0)
    {
        changeState(GateFsmState::send);
    }
}

void GateFsm::send()
{
    if (gate.send() < 0)
    {
        gate.reset();
        changeState(GateFsmState::error);
    }
    else
    {
        changeState(GateFsmState::receive);
    }
}

void GateFsm::receive()
{
    int result = gate.receive();
    if (result < 0)
    {
        gate.reset();
        changeState(GateFsmState::error);
    }
    else if (result)
    {
        changeState(GateFsmState::idle);
    }
}

void GateFsm::error()
{
    if (tick % 1024 == 0)
    {
        changeState(GateFsmState::init);
    }
}

void GateFsm::changeState(GateFsmState newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* GateFsm::toString(GateFsmState st) const
{
    switch (st)
    {
    case GateFsmState::init:
        return "Init";
    case GateFsmState::connect:
        return "Connect";
    case GateFsmState::idle:
        return "Idle";
    case GateFsmState::send:
        return "Send";
    case GateFsmState::receive:
        return "Receive";
    case GateFsmState::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}