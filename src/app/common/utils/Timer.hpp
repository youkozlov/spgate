#pragma once

#include <chrono>

namespace sg
{

class Timer
{
public:

    Timer(unsigned int);

    void set();

    bool expired();

private:
    int const period;
    std::chrono::steady_clock::time_point storedTime;
};

}