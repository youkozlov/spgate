#pragma once

#include <memory>

#include "interfaces/LinkAcceptor.hpp"

namespace sg
{

class Link;

class LinkAcceptorRl : public LinkAcceptor
{
public:
    struct Init
    {
        int port;
    };

    explicit LinkAcceptorRl(Init const&);
    
    ~LinkAcceptorRl();
    
    std::unique_ptr<Link> accept() final;
    
private:
    std::unique_ptr<Link> listner;
};
}