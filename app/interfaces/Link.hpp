#pragma once

namespace sg
{

class Link
{
public:
    virtual ~Link() {}
     
    virtual int read(void *, int, int) = 0;
    
    virtual int write(void const*, int) = 0;

    virtual int connect() = 0;
    
    virtual int select(int) = 0;
};

}