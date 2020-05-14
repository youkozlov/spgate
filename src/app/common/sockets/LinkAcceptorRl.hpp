#pragma once

#include <memory>

#include "LinkAcceptor.hpp"
#include "LinkRl.hpp"

namespace sg
{

class Link;
struct IpAddr;

class LinkAcceptorRl : public LinkAcceptor
{
public:
    struct Init
    {
        IpAddr const& ipAddr;
    };

    explicit LinkAcceptorRl(Init const&);
    
    ~LinkAcceptorRl();
    
    int accept() final;
    
private:

    static constexpr int acceptTimeout = 5;
    LinkRl listner;
};
}