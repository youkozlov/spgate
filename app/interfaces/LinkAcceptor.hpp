#pragma once

#include <memory>

namespace sg
{

class Link;

class LinkAcceptor
{
public:
    
    virtual ~LinkAcceptor() {}
    
    virtual std::unique_ptr<Link> accept() = 0;
};

}