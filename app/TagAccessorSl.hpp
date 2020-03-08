#pragma once

#include "TagAccessor.hpp"

class Shm;

namespace sg
{

class TagAccessorSl : public TagAccessor
{
public:
    TagAccessorSl(Shm&);
    
    int   getType(const char*) final;
    char  getBool(const char*) final;    
    short getShort(const char*) final;    
    long  getLong(const char*) final;    
    float getFloat(const char*) final;    
    int   setBool(const char*, char) final;    
    int   setShort(const char*, short) final;    
    int   setLong(const char*, long) final;    
    int   setFloat(const char*, float) final;    

private:
    Shm& shm;
};
}