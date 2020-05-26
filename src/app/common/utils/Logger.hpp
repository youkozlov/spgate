#pragma once

#include <cstring>
#include <cstdio>
#include "stdio.h"
#include "stdlib.h"
#include <ctime>
#include <stdexcept>


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

namespace
{

#define DEFAULT_LOG_OUTPUT  CON
#define DEFAULT_LOG_LEVEL   NA
#define DEFAULT_LOG_FOLDER  "/tmp"
#define DEFAULT_LOG_APPNAME "spgate"

char const* toString(LogLevel lvl)
{
    switch(lvl)
    {
    case LogLevel::LD:
        return "DEBUG";
    case LogLevel::LI:
        return "INFO";
    case LogLevel::LW:
        return "WARNING";
    case LogLevel::LE:
        return "ERROR";
    case LogLevel::NA:
        return "NA";
    }
    return "INVALID";
}

char const* toString(LogOutput lout)
{
    switch(lout)
    {
    case LogOutput::CON:
        return "CON";
    case LogOutput::FILE:
        return "FILE";
    case LogOutput::NA:
        return "NA";
    }
    return "INVALID";
}

LogOutput getLogOutput()
{
    char const* env = ::getenv("SG_LOGOUT");
    if (!env)
    {
        return LogOutput::DEFAULT_LOG_OUTPUT;
    }
    else
    {
        if (!strcmp(env, "CON"))
        {
            return LogOutput::CON;
        }
        else if (!strcmp(env, "FILE"))
        {
            return LogOutput::FILE;
        }
        else
        {
            return LogOutput::DEFAULT_LOG_OUTPUT;
        }
    }
}

LogLevel getLogLevel()
{
    char const* env = ::getenv("SG_LOGLVL");
    if (!env)
    {
        return LogLevel::DEFAULT_LOG_LEVEL;
    }
    else
    {
        if (!strcmp(env, "LD"))
        {
            return LogLevel::LD;
        }
        else if (!strcmp(env, "LI"))
        {
            return LogLevel::LI;
        }
        else if (!strcmp(env, "LW"))
        {
            return LogLevel::LW;
        }
        else if (!strcmp(env, "LE"))
        {
            return LogLevel::LE;
        }
        else
        {
            return LogLevel::DEFAULT_LOG_LEVEL;
        }
    }
}

char const* getLogFolder()
{
    char const* env = ::getenv("SG_LOGFOLDER");
    if (!env)
    {
        return DEFAULT_LOG_FOLDER;
    }
    else
    {
        return env;
    }
}

char const* getAppName()
{
    char const* env = ::getenv("_");
    if (!env)
    {
        return DEFAULT_LOG_APPNAME;
    }
    else
    {
        auto* pos = std::strrchr(env, '/');
        return pos ? pos + 1 : env;
    }
}
}

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

    static Logger& getInst()
    {
        static Logger inst;
        return inst;
    }

    void dispatch(char const* msg, LogLevel lvl, char const* file, int line)
    {
        char strTimeInfo[80];
        time_t rawtime;
        time (&rawtime);
        struct tm * timeinfo = localtime(&rawtime);
        strftime(strTimeInfo,sizeof(strTimeInfo),"%d-%m-%Y %H:%M:%S",timeinfo);

        switch (logOutput)
        {
        case LogOutput::CON:
            printf("%s %s %s %d: %s\n", strTimeInfo, toString(lvl), strrchr(file, '/') + 1, line, msg);
        break;
        case LogOutput::FILE:
            fprintf(fp, "%s %s %s %d: %s\n", strTimeInfo, toString(lvl), strrchr(file, '/') + 1, line, msg);
        break;
        default:
        break;
        }
    }

    LogLevel currentLogLevel() const
    {
        return logLevel;
    }

    void setLogLevel(LogLevel lvl)
    {
        logLevel = lvl;
    }

    char const* getLogLevelStr() const
    {
        return toString(logLevel);
    }

    char const* getLogOutputStr() const
    {
        return toString(logOutput);
    }

    char const* getLogFile() const
    {
        return logFile;
    }

private:
    Logger()
        : fp(nullptr)
        , logOutput(getLogOutput())
        , logLevel(logOutput == LogOutput::NA ? LogLevel::NA : getLogLevel())
    {
        fillLogFileName();
        if (logOutput == LogOutput::FILE)
        {
            fp = ::fopen(logFile,"w");
            if(!fp)
            {
                throw std::runtime_error("Can't open log file");
            }
        }
    }

    ~Logger()
    {
        if (logOutput == LogOutput::FILE)
        {
            ::fclose(fp);
        }
    }

    void fillLogFileName()
    {
        unsigned const logFileNameLen = std::strlen(getLogFolder()) + std::strlen(getAppName()) + 10;
        if (logFileNameLen > sizeof(logFile))
        {
            throw std::runtime_error("Too huge log file name");
        }
        sprintf(logFile, "%s/%s.log", getLogFolder(), getAppName());
    }


    Logger(Logger const&)          = delete;
    void operator=(Logger const&)  = delete;

    ::FILE *fp;
    const LogOutput logOutput;
    LogLevel        logLevel;
    char            logFile[128];
};

}