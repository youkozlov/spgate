#include "GateM4Bus.hpp"
#include "types/ParamsDefs.hpp"
#include "utils/WrapBuffer.hpp"

namespace sg
{

GateM4Bus::GateM4Bus(Init const& init)
    : gateParams(init.gateParams)
    , regs(init.regs)
{
}

GateM4Bus::~GateM4Bus()
{
}

bool GateM4Bus::configure()
{
    return false;
}

void GateM4Bus::tickInd()
{
}

int GateM4Bus::connect()
{
    return 0;
}

int GateM4Bus::send()
{
    return 0;
}

int GateM4Bus::receive()
{
    return 0;
}

unsigned int GateM4Bus::period()
{
    return gateParams.readPeriod;
}

void GateM4Bus::reset()
{
}

}