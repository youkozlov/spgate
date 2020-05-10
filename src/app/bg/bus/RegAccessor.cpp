#include "RegAccessor.hpp"
#include "utils/Logger.hpp"
#include <cstring>

namespace sg
{

constexpr unsigned int regsPerParam = 4;
constexpr unsigned int paramStatusOffset = 2;

RegAccessor::RegAccessor(Buffer<uint16_t>& r)
    : regs(r)
{
}

void RegAccessor::setValue(unsigned int paramId, float val)
{
    if (!validateParamId(paramId))
    {
        return;
    }
    auto& valReg = regs[paramId * regsPerParam];
    memcpy(&valReg, &val, sizeof(val));
    regs[paramId * regsPerParam + paramStatusOffset] = ready;
}

void RegAccessor::setValue(unsigned int paramId, int32_t val)
{
    if (!validateParamId(paramId))
    {
        return;
    }
    auto& valReg = regs[paramId * regsPerParam];
    memcpy(&valReg, &val, sizeof(val));
    regs[paramId * regsPerParam + paramStatusOffset] = ready;
}

void RegAccessor::setStatus(unsigned int paramId, Status st)
{
    if (!validateParamId(paramId))
    {
        return;
    }
    regs[paramId * regsPerParam + paramStatusOffset] = st;
}

bool RegAccessor::validateParamId(unsigned int paramId) const
{
    if ((paramId * regsPerParam + paramStatusOffset) >= regs.size())
    {
        LM(LE, "Invalid paramId: %u", paramId);
        return false;
    }
    return true;
}

}