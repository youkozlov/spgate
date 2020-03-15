#pragma once

#include <array>

namespace sg
{

class ModbusBuffer
{
public:
    ModbusBuffer() {}

    void reset()
    {
        std::fill(buf.begin(), buf.end(), 0);
    }

    int size() const
    {
        return capacity;
    }

    uint16_t operator[](unsigned int i) const
    {
        return buf[i];
    }

    uint16_t& operator[](unsigned int i)
    {
        return buf[i];
    }

    ModbusBuffer(ModbusBuffer const&) = delete;
    void operator=(ModbusBuffer const&) = delete;
    
private:
    static constexpr int capacity = 1024 * 16;
    std::array<uint16_t, capacity> buf{};
};

}