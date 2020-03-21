#pragma once

namespace sg
{

class Link;

class RsBusRx
{
public:
    enum Result
    {
        invalid = -9
    };
    explicit RsBusRx(Link& link);

    int receive(unsigned char*, unsigned int);

private:
    Link& link;
};

}