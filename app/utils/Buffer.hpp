#pragma once

#include <cstring>
#include "stdio.h"

namespace sg
{

class Buffer
{
    enum { capacity = 1024 };

public:
    Buffer() {}
    
    void reset()
    {
        pos = 0;
    }
    
    bool addByte(int ch)
    {
        if (pos >= capacity)
        {
            return false;
        }
        
        buf[pos] = ch;
        
        pos += 1;
        
        return true;
    }

    bool encode(int val)
    {
        if (pos + 16 >= capacity)
        {
            return false;
        }

        sprintf(&buf[pos], "%d", val);
        
        pos += strlen(&buf[pos]);
        
        return true;
    }
    
    char* get(int offset = 0)
    {
        return &buf[offset];
    }
    
    int getCapacity() const
    {
        return capacity;
    }

    int size() const
    {
        return pos;
    }
    
private:

    int pos;
    char buf[capacity];
};
}