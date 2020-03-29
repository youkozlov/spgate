#pragma once

#include <array>

namespace sg
{

template<typename T>
class Buffer
{
public:
    Buffer() {}

    void reset()
    {
        std::fill(buf.begin(), buf.end(), 0);
    }

    unsigned int size() const
    {
        return capacity;
    }

    T operator[](unsigned int i) const
    {
        return buf[i];
    }

    T& operator[](unsigned int i)
    {
        return buf[i];
    }

    Buffer(Buffer const&) = delete;
    void operator=(Buffer const&) = delete;
    
private:
    static constexpr unsigned int capacity = 1024 * 16;
    std::array<T, capacity> buf{};
};

}