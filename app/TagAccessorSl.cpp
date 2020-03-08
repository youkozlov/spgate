#include "TagAccessorSl.hpp"
#include "shm.hpp"

namespace sg
{

TagAccessorSl::TagAccessorSl(Shm& s)
    : shm(s)
{
}

int TagAccessorSl::getType(const char* name)
{
    return shm.getType(name);
}

char TagAccessorSl::getBool(const char* name)
{
    return shm.getBool(name);
}

short TagAccessorSl::getShort(const char* name)
{
    return shm.getShort(name);
}

long TagAccessorSl::getLong(const char* name)
{
    return shm.getLong(name);
}

float TagAccessorSl::getFloat(const char* name)
{
    return shm.getFloat(name);
}

int TagAccessorSl::setBool(const char* name, char val)
{
    return shm.setBool(name, val);
}

int TagAccessorSl::setShort(const char* name, short val)
{
    return shm.setShort(name, val);
}

int TagAccessorSl::setLong(const char* name, long val)
{
    return shm.setLong(name, val);
}

int TagAccessorSl::setFloat(const char* name, float val)
{
    return shm.setFloat(name, val);
}

}