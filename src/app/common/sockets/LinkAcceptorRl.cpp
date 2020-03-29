#include "LinkAcceptorRl.hpp"
#include "types/IpAddr.hpp"

#include <sys/socket.h>

namespace sg
{

LinkAcceptorRl::LinkAcceptorRl(Init const& init)
    : listner(init.ipAddr, 3)
{
    listner.connect();
}

LinkAcceptorRl::~LinkAcceptorRl()
{
}

int LinkAcceptorRl::accept()
{
    if (listner.select(acceptTimeout))
    {
        return ::accept(listner.getHandl(), NULL, NULL);
    }
    return -2;
}

}