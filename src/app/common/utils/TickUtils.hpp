#pragma once

namespace sg
{

class TickUtils
{
public:
    static unsigned long getTickPeriod()
    {
        return tickPeriod;
    }
    static unsigned long getTicks(unsigned long periodMs)
    {
        return periodMs * 1000 * 1000 / tickPeriod;
    }
private:
    static constexpr unsigned long tickPeriod = 1000 * 1000;
};

}