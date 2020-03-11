#pragma once

namespace sg
{

class Gate;

enum class GateFsmState
{
    init,
    connect,
    idle,
    send,
    receive,
    error
};

class GateFsm
{
public:

    explicit GateFsm(Gate&);

    ~GateFsm();

    void tickInd();

private:

    void init();
    void connect();
    void idle();
    void send();
    void receive();
    void error();

    void changeState(GateFsmState);
    char const* toString(GateFsmState) const;

    GateFsmState state;
    Gate&        gate;
    int          tick;
};
}