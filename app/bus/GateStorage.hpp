#pragma once

#include <vector>

namespace sg
{

class ParamParser;
struct GateParams;
struct DeviceParams;
struct ParamParams;

struct GateReadItem
{
    DeviceParams const& device;
    ParamParams const&  prms;
};

class GateStorage
{
public:
    explicit GateStorage(ParamParser const&);

    bool configure(GateParams const&);

    unsigned int getNumItems() const;

    GateReadItem const& getItem(unsigned int i) const;

private:
    ParamParser const& parser;
    std::vector<GateReadItem> items;
};

}