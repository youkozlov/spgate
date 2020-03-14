#pragma once

namespace sg
{

class Server;

enum class ServerFsmState
{
    init,
    accept,
    process,
    error
};

class ServerFsm
{
public:

    explicit ServerFsm(Server&);

    ~ServerFsm();

    void tickInd();

private:
    void init();
    void accept();
    void process();
    void error();

    void changeState(ServerFsmState);
    char const* toString(ServerFsmState) const;

    ServerFsmState state;
    Server&        server;
    int            tick;
};
}