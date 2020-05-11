#include "LinkPool.hpp"
#include "LinkRl.hpp"

namespace sg
{

LinkPool::LinkPool()
{
    for(auto& it : pool)
    {
        it = std::unique_ptr<Link>(new LinkRl(-1));
    }
    for (LinkId id = 0; id < maxNumLinks; ++id)
    {
        idPool.push(id);
    }
}

LinkPool::~LinkPool()
{
}

bool LinkPool::alloc(LinkId& id)
{
    if (idPool.empty())
    {
        return false;
    }
    id = idPool.pop();
    return true;
}

bool LinkPool::free(LinkId id)
{
    if (idPool.full())
    {
        return false;
    }
    idPool.push(id);
    return true;
}

Link& LinkPool::get(LinkId id)
{
    return *(pool[id]);
}

bool LinkPool::empty() const
{
    return idPool.empty();
}

} // namespace sg
