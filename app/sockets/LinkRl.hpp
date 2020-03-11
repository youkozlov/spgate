#pragma once

#include "types/IpAddr.hpp"
#include "interfaces/Link.hpp"
#include "rlsocket.h"

namespace sg
{
class LinkRl : public Link
{
public:
    explicit LinkRl(int fd)
        : socket(fd)
    {
    }

    explicit LinkRl(IpAddr const& ipAddr)
        : socket(ipAddr.addr, ipAddr.port, 1)
    {
    }

    explicit LinkRl(IpAddr const& ipAddr, int active)
        : socket(ipAddr.addr, ipAddr.port, active)
    {
    }
    
    int read(void *buf, int len, int timeout = 0) final
    {
        return socket.read(buf, len, timeout);
    }
    
    int write(void const* buf, int len) final
    {
        return socket.write(buf, len);
    }

    int connect() final
    {
        return socket.connect();
    }

    void close() final
    {
        socket.disconnect();
    }

    int select(int timeout) final
    {
        return socket.select(timeout);
    }
    
    void setHandl(int fd) final
    {
        socket.s = fd;
    }

    int getHandl() const
    {
        return socket.s;
    }

    LinkRl(LinkRl const&) = delete;
    void operator=(LinkRl const&) = delete;

private:
    rlSocket socket;
};
}