#pragma once

#include <memory>

#include "interfaces/LinkAcceptor.hpp"
#include "sockets/LinkRl.hpp"

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

    static constexpr int acceptTimeout = 500;
    LinkRl listner;
};
}