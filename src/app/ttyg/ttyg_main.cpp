#include <iostream>
#include <getopt.h>
#include <signal.h>
#include <cstring>
#include "TtyGate.hpp"
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
    };

    int option_index = 0;
    ::option long_options[] = 
    {
        {"help",     no_argument      , 0,  'h'},
        {"config",   required_argument, 0,  'c'},
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
        default:
            showUsage(argc, argv);
            return 1;
        }
    }

    //  CONFIGURATION FILE
    char const* configArg = items[0].present ? items[0].val : items[0].defaultVal;

    auto& logger = sg::Logger::getInst();

    std::cout << PROJECT_NAME << " " << PROJECT_VER << "\n"
              << "Build: " << GIT_BUILD_INFO << "\n"
              << "config: " << configArg << "\n"
              << "loglevel: " << logger.getLogLevelStr() << std::endl
              << "logoutput: " << logger.getLogOutputStr() << std::endl
              << "logfile: " << logger.getLogFile() << std::endl
              << std::endl;

    try
    {
        ttyg::TtyGate::Init init{configArg};
        ttyg::TtyGate tg(init);
        while (!done)
        {
            tg.tickInd();
            sg::Utils::nsleep(sg::TickUtils::getTickPeriod());
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "App catched exeption: " << e.what() << '\n';
    }
    return 0;
}
