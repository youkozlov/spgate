#pragma once

#include "interfaces/FeProcessor.hpp"
#include "DataRespond.hpp"

namespace sg
{
    
class SerialPort;
    
class FeProcessorM4Net : public FeProcessor
{
public:
    struct Init
    {
        SerialPort& port;
    };
    
    explicit FeProcessorM4Net(Init const&);

    ~FeProcessorM4Net();
    
    bool request(DataRequest const&) final;
    
    DataRespond respond() final;

private:
    SerialPort& port;
};

}