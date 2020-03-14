#pragma once

namespace sg
{

class Link;

class SpBusRx
{
public:
    explicit SpBusRx(Link& link);

    int receive(unsigned char*, int);

public:
    Link& link;
};

}