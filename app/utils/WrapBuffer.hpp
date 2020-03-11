#pragma once

#include <cstring>
#include "stdio.h"


namespace sg
{
    
class WrapBuffer
{
public:
    explicit WrapBuffer(unsigned char* p, int l)
        : ptr(p)
        , len(l)
        , pos(0)
    {}

    unsigned char const* cbegin() const
    {
        return &ptr[0];
    }

    int read()
    {
        return ptr[pos++];
    }
    
    int readLe()
    {
        unsigned ll = ptr[pos++];
        unsigned hh = ptr[pos++];
        return (hh << 8) | ll;
    }

    int readBe()
    {
        unsigned hh = ptr[pos++];
        unsigned ll = ptr[pos++];
        return (hh << 8) | ll;
    }
    
    void write(int ch)
    {
        ptr[pos++] = ch;
    }

    void writeBe(int ch)
    {
        ptr[pos++] = ch >> 8;
        ptr[pos++] = ch;
    }

    void encode(int val)
    {
        if (pos + 16 >= len)
        {
            return;
        }
        sprintf((char*)(&ptr[pos]), "%d", val);
        pos += strlen((char*)(&ptr[pos]));
    }

    int size() const
    {
        return pos;
    }

    void reset()
    {
        pos = 0;
    }
    
private:
    unsigned char* ptr;
    int len;
    int pos;
};
}