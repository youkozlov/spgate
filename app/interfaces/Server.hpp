#pragma once

namespace sg
{

class Server
{
public:

    virtual ~Server() {}

    virtual char const* name() = 0;

    virtual int accept() = 0;

    virtual int process() = 0;

    virtual void reset() = 0;
};

}