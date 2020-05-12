#include <iostream>
#include <getopt.h>
#include <cstring>
#include "TtyGate.hpp"
#include "utils/Utils.hpp"
#include "utils/TickUtils.hpp"

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
              << "\t-s,--tty SERIALPORT\tDefine the serial port device, default /dev/tty0\n"
              << "\t-b,--start_timeout TIMEOUT\tDefine start serial port timeout, default 100ms\n"
              << "\t-e,--end_timeout TIMEOUT\tDefine end serial port timeout, default 300ms\n"
              << "\t-p,--port NUMBER\tDefine the tcp/ip port for incoming connections, default 9999\n"
              << "\t-m,--mode MODENAME\tDefine execution mode, default real\n"
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
        {"/dev/tty0", false, ""},
        {"100", false, ""},
        {"300", false, ""},
        {"9999", false, ""},
        {"real", false, ""},
    };

    int option_index = 0;
    ::option long_options[] = 
    {
        {"help",          no_argument      , 0,  'h'},
        {"tty",           required_argument, 0,  's'},
        {"start_timeout", required_argument, 0,  'b'},
        {"end_timeout",   required_argument, 0,  'e'},
        {"port",          required_argument, 0,  'p'},
        {"mode",          required_argument, 0,  'm'},
        {0,        0, 0,  0 }
    };

    int c;
    while ((c = ::getopt_long(argc, argv, "hs:b:e:p:m:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'h':
            showUsage(argc, argv);
            return 1;
        case 's':
            items[0].present = true;
            std::strncpy(items[0].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'b':
            items[1].present = true;
            std::strncpy(items[1].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'e':
            items[2].present = true;
            std::strncpy(items[2].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'p':
            items[3].present = true;
            std::strncpy(items[3].val, ::optarg, sizeof(ConfigItem::val));
        break;
        case 'm':
            items[4].present = true;
            std::strncpy(items[4].val, ::optarg, sizeof(ConfigItem::val));
        break;
        default:
            showUsage(argc, argv);
            return 1;
        }
    }
    char const* ttyArg = items[0].present ? items[0].val : items[0].defaultVal;

    char const* startTimeoutArg = items[1].present ? items[1].val : items[1].defaultVal;
    unsigned startTimeout;
    if (sscanf(startTimeoutArg, "%u", &startTimeout) != 1 || startTimeout > 0xffff)
    {
        showUsage(argc, argv);
        return 1;
    }

    char const* endTimeoutttyArg = items[2].present ? items[2].val : items[2].defaultVal;
    unsigned endTimeout;
    if (sscanf(endTimeoutttyArg, "%u", &endTimeout) != 1 || endTimeout > 0xffff)
    {
        showUsage(argc, argv);
        return 1;
    }

    char const* portArg = items[3].present ? items[3].val : items[3].defaultVal;
    unsigned portInt;
    if (sscanf(portArg, "%u", &portInt) != 1 || portInt > 0xffff)
    {
        showUsage(argc, argv);
        return 1;
    }
    
    char const* modeArg = items[4].present ? items[4].val : items[4].defaultVal;
    ttyg::Mode mode;
    if (0 == strcmp(modeArg, "real"))
    {
        mode = ttyg::Mode::real;
    }
    else if (0 == strcmp(modeArg, "stub"))
    {
        mode = ttyg::Mode::stub;
    }
    else
    {
        showUsage(argc, argv);
        return 1;
    }

    std::cout << PROJECT_NAME << " " << PROJECT_VER << "\n"
              << "Build: " << GIT_BUILD_INFO << "\n"
              << "tty: " <<  ttyArg << "\n"
              << "start_timeout: " << startTimeout << "\n"
              << "end_timeout: " << endTimeout << "\n"
              << "port: " << portInt << "\n"
              << "mode: " << modeArg << "\n"
              << std::endl;

    try
    {
        ttyg::TtyGate::Init init{mode, ttyArg, startTimeout, endTimeout, portInt};
        ttyg::TtyGate tg(init);
        while (1)
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
