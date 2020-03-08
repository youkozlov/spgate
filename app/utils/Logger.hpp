#pragma once

#include <cstring>

#define CURRENT_LOG_LEVEL LD

namespace sg
{

enum LogLevel
{
    LD,
    LI,
    LW,
    LE
};

#define LM(LM_LEVEL, ...) \
{ \
    if (LogLevel::LM_LEVEL >= LogLevel::CURRENT_LOG_LEVEL) \
    {\
        char msg[256] = {}; sprintf(msg, __VA_ARGS__);  Logger::dispatch(msg, LogLevel::LM_LEVEL, __FILE__, __LINE__); \
    }\
}

class Logger
{
public:
    static void dispatch(char const* msg, LogLevel lvl, char const* file, int line);
};

}