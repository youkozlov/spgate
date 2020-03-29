#pragma once

#include "TagAccessor.hpp"
#include "gmock/gmock.h"

namespace sg
{

class MockTagAccessor : public TagAccessor
{
public:
    MOCK_METHOD(int, getType, (const char*));
    MOCK_METHOD(char, getBool, (const char*));    
    MOCK_METHOD(short, getShort, (const char*));    
    MOCK_METHOD(long, getLong, (const char*));    
    MOCK_METHOD(float, getFloat, (const char*));    
    MOCK_METHOD(int, setBool, (const char*, char));    
    MOCK_METHOD(int, setShort, (const char*, short));    
    MOCK_METHOD(int, setLong, (const char*, long));    
    MOCK_METHOD(int, setFloat, (const char*, float));    
};

}