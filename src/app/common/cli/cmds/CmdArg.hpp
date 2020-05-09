#pragma once

#include <vector>
#include <memory>

namespace sg
{
namespace cli
{

class CmdArg
{
public:
    virtual ~CmdArg() {}

    virtual char const* name() const = 0;

    virtual char const* description() const = 0;

    virtual char const* value() = 0;

    virtual bool setValue(char const*) = 0;
};

using CmdArgVector = std::vector<std::unique_ptr<CmdArg>>;

} // namespace cli
} // namespace sg
