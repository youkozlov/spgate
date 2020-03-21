#pragma once

namespace sg
{

class Link;

class SpBusRx
{
public:
    explicit SpBusRx(Link& link);

    int receive(unsigned char*, unsigned int);

public:
    Link& link;
};

}