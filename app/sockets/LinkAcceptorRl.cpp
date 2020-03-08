#include "LinkAcceptorRl.hpp"
#include "LinkRl.hpp"

namespace sg
{

LinkAcceptorRl::LinkAcceptorRl(Init const& init)
{
    listner = std::unique_ptr<Link>(new LinkRl("", init.port, 0));
}

LinkAcceptorRl::~LinkAcceptorRl()
{
}

std::unique_ptr<Link> LinkAcceptorRl::accept()
{
    int fd = listner->connect();

    if (fd < 0)
    {
        return nullptr;
    }

    return std::unique_ptr<Link>(new LinkRl(fd));
}

}