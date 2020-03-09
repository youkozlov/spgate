#pragma once

namespace sg
{

class Gate
{
public:
    virtual ~Gate() {}
    
    virtual bool configure() = 0;

    virtual void tickInd() = 0;
};

}