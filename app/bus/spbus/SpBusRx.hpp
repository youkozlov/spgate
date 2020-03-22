#pragma once

namespace sg
{

class Link;

class SpBusRx
{
public:
    enum Result
    {
        invalid = -9
    };

    explicit SpBusRx(Link& link);

    int receive(unsigned char*, unsigned int);

public:
    Link& link;
};

}