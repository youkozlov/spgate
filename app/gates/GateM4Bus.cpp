#include "GateM4Bus.hpp"
#include "types/ParamsDefs.hpp"

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


bool GateM4Bus::request(DataRequest const&)
{
    return true;
}
    
DataRespond GateM4Bus::respond()
{
    DataRespond rsp;
    return rsp;
}

}