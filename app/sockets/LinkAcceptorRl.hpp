#pragma once

#include <memory>

#include "interfaces/LinkAcceptor.hpp"

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
    
    std::unique_ptr<Link> accept() final;
    
private:
    std::unique_ptr<Link> listner;
};
}