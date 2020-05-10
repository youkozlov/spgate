#pragma once

#include "CmdArg.hpp"

namespace sg
{
namespace cli
{

class CmdArgStatsType : public CmdArg
{
public:
    explicit CmdArgStatsType();

    ~CmdArgStatsType();

    char const* name() const final;

    char const* description() const final;

    char const* value() final;

    bool setValue(char const*) final;
};

} // namespace cli
} // namespace sg
