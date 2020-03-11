#pragma once

namespace sg
{

class Gate
{
public:
    virtual ~Gate() {}
    
    virtual bool configure() = 0;

    virtual void tickInd() = 0;

    virtual int send() = 0;

    virtual int receive() = 0;

    virtual unsigned int period() = 0;

    virtual int connect() = 0;

    virtual void reset() = 0;

};

}