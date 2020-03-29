#pragma once

namespace sg
{

class TagAccessor
{
public:
    virtual ~TagAccessor() {}
    
    virtual int   getType(const char*) = 0;
    virtual char  getBool(const char*) = 0;    
    virtual short getShort(const char*) = 0;    
    virtual long  getLong(const char*) = 0;    
    virtual float getFloat(const char*) = 0;    
    virtual int   setBool(const char*, char) = 0;    
    virtual int   setShort(const char*, short) = 0;    
    virtual int   setLong(const char*, long) = 0;    
    virtual int   setFloat(const char*, float) = 0;    
};

}