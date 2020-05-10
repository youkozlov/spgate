#include "GateStorage.hpp"
#include "utils/ParamParser.hpp"
#include "utils/Logger.hpp"

namespace sg
{

GateStorage::GateStorage(ParamParser const& p)
    : parser(p)
{
}

bool GateStorage::configure(GateParams const& gateParams)
{
    for (unsigned int i = 0; i < parser.getNumParams(); ++i)
    {
        auto& param = parser.getParam(i);

        if (param.deviceId >= parser.getNumDevices())
        {
            LM(LE, "Unexpected deviceId=%u for given paramId=%u, numDevices=%u",
                param.deviceId, param.id, parser.getNumDevices());
            return false;
        }

        auto& device = parser.getDevice(param.deviceId);
        if (device.gateId != gateParams.id)
        {
            continue;
        }

        GateReadItem item{device, param};

        items.push_back(item);
    }
    if (!items.size())
    {
        LM(LE, "There are no configured items for given gateId=%u, please check ini file", gateParams.id);
        return false;
    }
    return true;
}

unsigned int GateStorage::getNumItems() const
{
    return items.size();
}

GateReadItem const& GateStorage::getItem(unsigned int i) const
{
    return items[i];
}

}