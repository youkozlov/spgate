#pragma once

namespace sg
{

class Client
{
public:
    enum class Result
    {
        waitForData = 0,
        progress,
        done,
        fail
    };

    virtual ~Client() {}

    virtual int send() = 0;

    virtual Result receive() = 0;

    virtual unsigned int period() const = 0;

    virtual int connect() = 0;

    virtual void disconnect() = 0;

    virtual void reset() = 0;

    virtual Result timeout() = 0;
};

}