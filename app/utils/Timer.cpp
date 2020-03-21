#include "Timer.hpp"
#include "utils/TickUtils.hpp"

namespace sg
{

Timer::Timer(unsigned int ms)
    : counter(0)
    , period(TickUtils::getTicks(ms))
{
}

void Timer::set()
{
    counter = period;
}

void Timer::reset()
{
    counter = 0;
}

bool Timer::isExpired()
{
    return (!counter) || ((counter--) == 1);
}

}