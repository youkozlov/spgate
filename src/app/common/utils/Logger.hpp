#pragma once

#include <cstring>
#include <cstdio>

namespace sg
{

enum LogOutput
{
    CON,
    FILE,
    DISABLED
};

enum LogLevel
{
    LD,
    LI,
    LW,
    LE,
    LN
};

#define LM(LM_LEVEL, ...) \
{ \
    if (LogLevel::LM_LEVEL >= Logger::getInst().currentLogLevel()) \
    {\
        char msg[256] = {};\
        sprintf(msg, __VA_ARGS__);\
        Logger::getInst().dispatch(msg, LogLevel::LM_LEVEL, __FILE__, __LINE__);\
    }\
}

class Logger
{
public:
    
    static Logger& getInst();
    
    void dispatch(char const* msg, LogLevel lvl, char const* file, int line);

    LogLevel currentLogLevel() const;

    void setLogLevel(LogLevel);

private:
    Logger();
    ~Logger();

    Logger(Logger const&)          = delete;
    void operator=(Logger const&)  = delete;

    ::FILE *fp;
    const LogOutput logOutput;
    LogLevel        logLevel;
    const char*     logFileName;
};

}