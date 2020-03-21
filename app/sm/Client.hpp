#pragma once

namespace sg
{

class Client
{
public:
    virtual ~Client() {}

    virtual int send() = 0;

    virtual int receive() = 0;

    virtual unsigned int period() const = 0;

    virtual int connect() = 0;

    virtual void reset() = 0;

    virtual void timeout() = 0;
};

}