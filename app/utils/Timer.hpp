#pragma once

namespace sg
{

class Timer
{
public:

    Timer(unsigned int);

    void set();

    void reset();

    bool isExpired();

private:
    unsigned long       counter;
    unsigned long const period;
};

}