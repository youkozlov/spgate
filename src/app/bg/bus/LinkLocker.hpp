#pragma once

namespace sg
{

class LinkLocker
{
public:
    explicit LinkLocker()
        : busy(false)
    {}

    ~LinkLocker() {}

    bool isLocked() const
    {
        return busy;
    }
    
    bool tryLock()
    {
        if (not busy)
        {
            busy = true;
            return true;
        }
        return false;
    }
    
    void unlock()
    {
        busy = false;
    }
private:
    bool busy;
};

}