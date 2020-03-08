#include "FeProcessorM4Net.hpp"

namespace sg
{

FeProcessorM4Net::FeProcessorM4Net(Init const& init)
    : port(init.port)
{
}

FeProcessorM4Net::~FeProcessorM4Net()
{
}
    
bool FeProcessorM4Net::request(DataRequest const&)
{
    return true;
}
    
DataRespond FeProcessorM4Net::respond()
{
    DataRespond rsp;
    return rsp;
}

}