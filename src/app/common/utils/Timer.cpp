#include "Timer.hpp"
#include "utils/TickUtils.hpp"

namespace sg
{

Timer::Timer(unsigned int ms)
    : period(ms)
    , storedTime(std::chrono::steady_clock::now())
{
}

void Timer::set()
{
    storedTime = std::chrono::steady_clock::now();
}

bool Timer::expired()
{
    using Milliseconds = std::chrono::duration<int, std::milli>;
    auto currentTime = std::chrono::steady_clock::now();
    Milliseconds const duration = std::chrono::duration_cast<Milliseconds>(currentTime - storedTime);
    return (duration.count() > period);
}

}