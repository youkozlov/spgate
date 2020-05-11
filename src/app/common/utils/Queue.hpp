#pragma once

#include <array>

namespace sg
{

template<unsigned SIZE>
class Queue
{
public:
    Queue()
        : begin(0)
        , end(0)
        , sz(0)
    {}
    
    ~Queue()
    {}

    unsigned pop()
    {
        auto const& val = buf[begin % SIZE];
        begin += 1;
        sz -= 1;
        return val;
    }

    void push(unsigned val)
    {
        buf[end % SIZE] = val;
        end += 1;
        sz += 1;
    }

    unsigned size() const
    {
        return sz;
    }

    bool empty() const
    {
        return (sz == 0);
    }

    bool full() const
    {
        return (sz == SIZE);
    }

    void reset()
    {
        begin = end = sz = 0;
    }

private:
    unsigned begin;
    unsigned end;
    unsigned sz;
    std::array<unsigned, SIZE> buf;
};

} // namespace sg
