#pragma once

#include "FeProcessor.hpp"
#include "DataRequest.hpp"
#include "DataRespond.hpp"
#include "gmock/gmock.h"

namespace sg
{

class MockFeProcessor : public FeProcessor
{
public:

    MOCK_METHOD(bool, request, (DataRequest const&));
    
    MOCK_METHOD(DataRespond, respond, ());
};

}