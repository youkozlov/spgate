#include <iostream>
#include <getopt.h>

#include "BusGate.hpp"
#include "utils/TickUtils.hpp"
#include "utils/Utils.hpp"

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
              << "\t-l,--loglevel NUMBER\tDefine logging level: 0 - DEBUG, 1 - INFO, 2 - WARN, 3 - ERR\n"
              << "\t-o,--logout NUMBER\tDefine logging output: 0 - CON, 1 - FILE, 2 - DISABLED\n"
              << "\t-f,--logfile FILENAME\tDefine the logging file"
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
        {"3", false, ""},
        {"2", false, ""},
        {"/tmp/spgate.log", false, ""}
    };

    int option_index = 0;
    ::option long_options[] = 
    {
        {"help",     no_argument      , 0,  'h'},
        {"config",   required_argument, 0,  'c'},
        {"loglevel", required_argument, 0,  'l'},
        {"logout",   required_argument, 0,  'o'},
        {"logfile",  required_argument, 0,  'f'},
        {0,        0, 0,  0 }
    };

    int c;
    while ((c = ::getopt_long(argc, argv, "hc:", long_options, &option_index)) != -1)
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
        case 'o':
            items[2].present = true;
            strncpy(items[2].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'f':
            items[3].present = true;
            strncpy(items[3].val, ::optarg, sizeof(ConfigItem::val));
        break;
        default:
            showUsage(argc, argv);
            return 1;
        }
    }

    sg::BusGate::Init init{items[0].present ? items[0].val : items[0].defaultVal};
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