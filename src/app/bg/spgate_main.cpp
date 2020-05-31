#include <iostream>
#include <getopt.h>
#include <signal.h>
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
              << "\t-i,--cli_port NUMBER\tDefine command line interface port, 0 - disabled\n"
              << "\t-e,--emul NUMBER\tDefine emulation mode, 0 - disabled\n"
              << std::endl;
}

static volatile int done = 0;

void intHandler(int)
{
    done = 1;
}

int main(int argc, char** argv)
{
    signal(SIGINT, intHandler);

    struct ConfigItem
    {
        char const* defaultVal;
        bool present;
        char val[128];
    } items[] = 
    {
        {"../cfg/default.ini", false, ""},
        {"0", false, ""},
        {"0", false, ""},
    };

    int option_index = 0;
    ::option long_options[] = 
    {
        {"help",     no_argument      , 0,  'h'},
        {"config",   required_argument, 0,  'c'},
        {"cli_port", required_argument, 0,  'i'},
        {"emul",     required_argument, 0,  'e'},
        {0,        0, 0,  0 }
    };

    int c;
    while ((c = ::getopt_long(argc, argv, "hc:i:e:", long_options, &option_index)) != -1)
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
        case 'i':
            items[1].present = true;
            strncpy(items[1].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'e':
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


    //  CLI PORT
    char const* cliportArg = items[1].present ? items[1].val : items[1].defaultVal;
    unsigned cliportInt;
    if (sscanf(cliportArg, "%u", &cliportInt) != 1 || cliportInt > 0xffff)
    {
        showUsage(argc, argv);
        return 1;
    }


    // EMUL
    char const* emulArg = items[2].present ? items[2].val : items[2].defaultVal;
    unsigned emulInt;
    if (sscanf(emulArg, "%u", &emulInt) != 1)
    {
        showUsage(argc, argv);
        return 1;
    }

    auto& logger = sg::Logger::getInst();

    std::cout << PROJECT_NAME << " " << PROJECT_VER << "\n"
              << "Build: " << GIT_BUILD_INFO << "\n"
              << "config: " <<  configArg << "\n"
              << "cli_port: " << cliportInt << "\n"
              << "emul: " << emulInt << "\n"
              << "loglevel: " << logger.getLogLevelStr() << std::endl
              << "logoutput: " << logger.getLogOutputStr() << std::endl
              << "logfile: " << logger.getLogFile() << std::endl
              << std::endl;

    sg::BusGate::Init init{configArg, cliportInt, (emulInt != 0)};
    sg::BusGate bg{init};

    try
    {
        while (!done)
        {
            if (bg.tickInd())
            {
                break;
            }
            sg::Utils::nsleep(sg::TickUtils::getTickPeriod());
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "App catched exeption: " << e.what() << std::endl;
    }

    return 0;
}
