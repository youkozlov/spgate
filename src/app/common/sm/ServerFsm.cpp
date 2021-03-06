#include "ServerFsm.hpp"

#include "Server.hpp"
#include "utils/Logger.hpp"
#include "utils/TickUtils.hpp"

namespace sg
{

ServerFsm::ServerFsm(Server& s)
    : state(ServerFsmState::init)
    , server(s)
    , tick(0)
{
}

ServerFsm::~ServerFsm()
{
}

void ServerFsm::tickInd()
{
    switch (state)
    {
    case ServerFsmState::init:
        init();
    break;
    case ServerFsmState::accept:
        accept();
    break;
    case ServerFsmState::process:
        process();
    break;
    case ServerFsmState::error:
        error();
    break;
    }

    tick += 1;
}

void ServerFsm::close()
{
    if (ServerFsmState::process == state)
    {
        server.reset();
        changeState(ServerFsmState::error);
    }
}

void ServerFsm::init()
{
    if (tick % 4 == 0)
    {
        changeState(ServerFsmState::accept);
    }
}

void ServerFsm::accept()
{
    if (server.accept())
    {
        changeState(ServerFsmState::process);
    }
}

void ServerFsm::process()
{
    if (server.process())
    {
        server.reset();
        changeState(ServerFsmState::error);
    }
}

void ServerFsm::error()
{
    if (tick % 4 == 0)
    {
        changeState(ServerFsmState::init);
    }
}

void ServerFsm::changeState(ServerFsmState newSt)
{
    LM(LI, "%s change state: %s -> %s", server.name(), toString(state), toString(newSt));
    state = newSt;
}

char const* ServerFsm::toString(ServerFsmState st) const
{
    switch (st)
    {
    case ServerFsmState::init:
        return "Init";
    case ServerFsmState::accept:
        return "Accept";
    case ServerFsmState::process:
        return "Process";
    case ServerFsmState::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}