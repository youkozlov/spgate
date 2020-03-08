#pragma once

namespace sg
{
    
class WrapBuffer
{
public:
    explicit WrapBuffer(uint8_t* p, int l)
        : ptr(p)
        , len(l)
        , pos(0)
    {}

    uint8_t const* cbegin() const
    {
        return &ptr[0];
    }

    int read()
    {
        return ptr[pos++];
    }
    
    int readLe()
    {
        int ll = ptr[pos++];
        int hh = ptr[pos++];
        return (hh << 8) | ll;
    }

    int readBe()
    {
        int hh = ptr[pos++];
        int ll = ptr[pos++];
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

    int size() const
    {
        return pos;
    }

    void reset()
    {
        pos = 0;
    }
    
private:
    uint8_t* ptr;
    int len;
    int pos;
};
}