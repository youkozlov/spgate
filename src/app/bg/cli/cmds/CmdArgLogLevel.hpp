#pragma once

#include "CmdArg.hpp"

namespace sg
{
namespace cli
{
class CmdArgLogLevel : public CmdArg
{
public:
    explicit CmdArgLogLevel();

    ~CmdArgLogLevel();

    char const* name() const final;

    char const* description() const final;

    char const* value() final;

    bool setValue(char const*) final;

private:
    char buf[32];
};

} // namespace cli
} // namespace sg
