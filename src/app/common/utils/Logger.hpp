#pragma once

#include <cstring>
#include <cstdio>

namespace sg
{

enum class LogOutput
{
    CON,
    FILE,
    NA
};

enum LogLevel
{
    LD,
    LI,
    LW,
    LE,
    NA
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

    char const* getLogLevelStr() const;

    char const* getLogOutputStr() const;

    char const* getLogFile() const;

private:
    Logger();
    ~Logger();

    void fillLogFileName();

    Logger(Logger const&)          = delete;
    void operator=(Logger const&)  = delete;

    ::FILE *fp;
    const LogOutput logOutput;
    LogLevel        logLevel;
    char            logFile[128];
};

}