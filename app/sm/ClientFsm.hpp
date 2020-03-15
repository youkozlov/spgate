#pragma once

namespace sg
{

class Client;

enum class ClientFsmState
{
    init,
    connect,
    idle,
    send,
    receive,
    error
};

class ClientFsm
{
public:

    explicit ClientFsm(Client&);

    ~ClientFsm();

    void tickInd();

private:

    void init();
    void connect();
    void idle();
    void send();
    void receive();
    void error();

    void changeState(ClientFsmState);
    char const* toString(ClientFsmState) const;

    ClientFsmState state;
    Client&        client;
    int            tick;
};
}