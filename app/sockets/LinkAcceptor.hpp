#pragma once

#include <memory>

namespace sg
{

class Link;

class LinkAcceptor
{
public:
    
    virtual ~LinkAcceptor() {}
    
    virtual int accept() = 0;
};

}