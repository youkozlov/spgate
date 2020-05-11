#pragma once

#include <array>
#include <memory>
#include "utils/Queue.hpp"

namespace sg
{

using LinkId = int;

class Link;

class LinkPool
{
public:

    explicit LinkPool();

    ~LinkPool();

    bool alloc(LinkId&);

    bool free(LinkId);

    Link& get(LinkId);

    bool empty() const;

private:
    static constexpr int maxNumLinks = 3;
    Queue<maxNumLinks> idPool;
    std::array<std::unique_ptr<Link>, maxNumLinks> pool;
};

} // namespace sg
