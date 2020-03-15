#include "RsBusClient.hpp"
#include "types/ParamsDefs.hpp"
#include "utils/WrapBuffer.hpp"

namespace sg
{

RsBusClient::RsBusClient(Init const& init)
    : gateParams(init.gateParams)
    , regs(init.regs)
{
}

RsBusClient::~RsBusClient()
{
}

void RsBusClient::tickInd()
{
}

int RsBusClient::connect()
{
    return 0;
}

int RsBusClient::send()
{
    return 0;
}

int RsBusClient::receive()
{
    return 0;
}

unsigned int RsBusClient::period()
{
    return gateParams.readPeriod;
}

void RsBusClient::reset()
{
}

}