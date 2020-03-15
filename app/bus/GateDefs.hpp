#pragma once

namespace sg
{

struct GateReadItemResult
{
    enum Status
    {
        timeout,
        invalid,
        ready
    };
    float value;
    Status st;
};

}