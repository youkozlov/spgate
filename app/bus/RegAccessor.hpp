#pragma once

#include <cstdint>
#include "utils/Buffer.hpp"

namespace sg
{

class RegAccessor
{
public:
    enum Status
    {
        undef,
        timeout,
        invalid,
        ready
    };
    explicit RegAccessor(Buffer<uint16_t>&);

    void setValue(unsigned int, float);

    void setValue(unsigned int, int32_t);

    void setStatus(unsigned int, Status);

private:
    bool validateParamId(unsigned int) const;

    Buffer<uint16_t>& regs;
};

}