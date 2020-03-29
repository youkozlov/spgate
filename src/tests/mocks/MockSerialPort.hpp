#pragma once

#include "SerialPort.hpp"
#include "SerialPortSetup.hpp"
#include "gmock/gmock.h"

namespace sg
{

class MockSerialPort : public SerialPort
{
public:

    MOCK_METHOD(int, setup, (SerialPortSetup const&));
    
    MOCK_METHOD(int, send, (void const*, size_t));

    MOCK_METHOD(int, receive, (void*, size_t));
};

}