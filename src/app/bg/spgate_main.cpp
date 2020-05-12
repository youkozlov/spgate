#include <iostream>
#include <getopt.h>

#include "BusGate.hpp"
#include "utils/TickUtils.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"

#ifndef QMAKE_VARIANT
#include "SpGateConfig.hpp"
#endif

void showUsage(int, char** argv)
{
    std::cerr << PROJECT_NAME << " " << PROJECT_VER << "\n"
              << "Build: " << GIT_BUILD_INFO << "\n"
              << "Usage: " << argv[0] << "\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-c,--config FILENAME\tDefine the configuration file\n"
              << "\t-l,--loglevel NUMBER\tDefine logging level: 0 - DEBUG, 1 - INFO, 2 - WARN, 3 - ERR, 4 - NA\n"
              << "\t-i,--cli_port NUMBER\tDefine command line interface port, 0 - disabled\n"
              << std::endl;
}

int main(int argc, char** argv)
{
    struct ConfigItem
    {
        char const* defaultVal;
        bool present;
        char val[128];
    } items[] = 
    {
        {"../cfg/default.ini", false, ""},
        {"4", false, ""},
        {"0", false, ""},
    };

    int option_index = 0;
    ::option long_options[] = 
    {
        {"help",     no_argument      , 0,  'h'},
        {"config",   required_argument, 0,  'c'},
        {"loglevel", required_argument, 0,  'l'},
        {"cli_port", required_argument, 0,  'i'},
        {0,        0, 0,  0 }
    };

    int c;
    while ((c = ::getopt_long(argc, argv, "hc:l:i:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'h':
            showUsage(argc, argv);
            return 1;
        case 'c':
            items[0].present = true;
            strncpy(items[0].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'l':
            items[1].present = true;
            strncpy(items[1].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'i':
            items[2].present = true;
            strncpy(items[2].val, ::optarg, sizeof(ConfigItem::val));
        break;
        default:
            showUsage(argc, argv);
            return 1;
        }
    }

    //  CONFIGURATION FILE
    char const* configArg = items[0].present ? items[0].val : items[0].defaultVal;


    //  LOGLEVEL
    char const* loglevelArg = items[1].present ? items[1].val : items[1].defaultVal;
    unsigned loglevelInt;
    if (sscanf(loglevelArg, "%u", &loglevelInt) != 1 || loglevelInt > 4)
    {
        showUsage(argc, argv);
        return 1;
    }
    sg::Logger::getInst().setLogLevel(static_cast<sg::LogLevel>(loglevelInt));


    //  CLI PORT
    char const* cliportArg = items[2].present ? items[2].val : items[2].defaultVal;
    unsigned cliportInt;
    if (sscanf(cliportArg, "%u", &cliportInt) != 1 || cliportInt > 0xffff)
    {
        showUsage(argc, argv);
        return 1;
    }


    std::cout << PROJECT_NAME << " " << PROJECT_VER << "\n"
              << "Build: " << GIT_BUILD_INFO << "\n"
              << "config: " <<  configArg << "\n"
              << "loglevel: " << loglevelInt << "\n"
              << "cli_port: " << cliportInt << "\n"
              << std::endl;

    sg::BusGate::Init init{configArg, cliportInt};
    sg::BusGate bg{init};

    while (1)
    {
        if (bg.tickInd())
        {
            break;
        }
        sg::Utils::nsleep(sg::TickUtils::getTickPeriod());
    }

    return 0;
}