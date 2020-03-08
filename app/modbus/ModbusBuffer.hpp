#pragma once

#include <vector>

namespace sg
{

class ModbusBuffer
{
public:
    explicit ModbusBuffer(int c = 1024 * 16)
        : capacity(c)
    {
        buf.resize(capacity);
    }

    void reset()
    {
        std::fill(buf.begin(), buf.end(), 0);
    }

    int size() const
    {
        return capacity;
    }

    uint16_t operator[](int i) const
    {
        return buf[i];
    }

    uint16_t& operator[](int i)
    {
        return buf[i];
    }

    ModbusBuffer(ModbusBuffer const&) = delete;
    void operator=(ModbusBuffer const&) = delete;
    
private:
    const int capacity;
    std::vector<uint16_t> buf;
};

}