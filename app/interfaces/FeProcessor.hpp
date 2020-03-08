#pragma once

namespace sg
{

struct DataRequest;
struct DataRespond;

class FeProcessor
{
public:
    virtual ~FeProcessor() {}
    
    virtual bool request(DataRequest const&) = 0;
    
    virtual DataRespond respond() = 0;
};

}