#pragma once

#include <cstring>
#include <algorithm>
#include "stdio.h"

namespace sg
{
    
class WrapBuffer
{
public:
    explicit WrapBuffer(unsigned char* p, unsigned l)
        : ptr(p)
        , len(l)
        , pos(0)
    {}

    unsigned char const* cbegin() const
    {
        return &ptr[0];
    }

    unsigned char* begin()
    {
        return &ptr[0];
    }

    bool read(unsigned char& ch)
    {
        if (pos < len)
        {
            ch = ptr[pos++];
            return true;
        }
        return false;
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

    void write(char* buf)
    {
        unsigned int length = std::strlen(buf);
        if (pos + length >= len)
        {
            return;
        }
        std::copy(buf, buf + length, &ptr[pos]);
        pos += length;
    }

    void writeBe(int ch)
    {
        ptr[pos++] = ch >> 8;
        ptr[pos++] = ch;
    }

    unsigned size() const
    {
        return pos;
    }

    unsigned capacity() const
    {
        return len;
    }

    void reset()
    {
        pos = 0;
    }
    
private:
    unsigned char* ptr;
    unsigned len;
    unsigned pos;
};
}