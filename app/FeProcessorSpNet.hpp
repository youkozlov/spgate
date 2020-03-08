#pragma once

#include "interfaces/FeProcessor.hpp"
#include "utils/Buffer.hpp"
#include "DataRespond.hpp"

namespace sg
{

class SerialPort;
 
class FeProcessorSpNet : public FeProcessor
{
public:
    struct Init
    {
        SerialPort& port;
    };

    explicit FeProcessorSpNet(Init const&);

    ~FeProcessorSpNet();

    bool request(DataRequest const&) final;
    
    DataRespond respond() final;
    
private:
    
    SerialPort& port;
    Buffer buf;
};

}