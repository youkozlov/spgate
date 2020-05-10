#pragma once

namespace sg
{

class Bus
{
public:
    virtual ~Bus() {}

    virtual void tickInd() = 0;
};

}