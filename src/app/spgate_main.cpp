#include "BusGate.hpp"
#include "utils/TickUtils.hpp"
#include "utils/Utils.hpp"

int main(int, char**)
{   
    char const* defaultConfig = "../cfg/default.ini";

    sg::BusGate::Init init{defaultConfig};
    sg::BusGate bg{init};

    while (1)
    {
        bg.tickInd();
        sg::Utils::nsleep(sg::TickUtils::getTickPeriod());
    }
    return 0;
}