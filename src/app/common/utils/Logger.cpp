#include "Logger.hpp"
#include "stdio.h"
#include "stdlib.h"
#include <ctime>

#define DEFAULT_LOG_OUTPUT CON
#define DEFAULT_LOG_LEVEL  LN
#define DEFAULT_LOG_FILE   "/tmp/spgate.log"

namespace sg
{

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
    case LogLevel::LN:
        return "DISABLED";
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

char const* getLogFileName()
{
    char const* env = ::getenv("SG_LOGFILE");
    if (!env)
    {
        return DEFAULT_LOG_FILE;
    }
    else
    {
        return env;
    }
}

Logger::Logger()
    : fp(nullptr)
    , logOutput(getLogOutput())
    , logLevel(logOutput == LogOutput::DISABLED ? LogLevel::LN : getLogLevel())
    , logFileName(getLogFileName())
{
    if (logOutput == LogOutput::FILE)
    {
        fp = ::fopen(logFileName,"w");
        if(!fp)
        {
            throw;
        }
    }
}

Logger::~Logger()
{
    if (logOutput == LogOutput::FILE)
    {
        ::fclose(fp);
    }
}

Logger& Logger::getInst()
{
    static Logger inst;
    return inst;
}

LogLevel Logger::currentLogLevel() const
{
    return logLevel;
}

void Logger::setLogLevel(LogLevel lvl)
{
    logLevel = lvl;
}

void Logger::dispatch(char const* msg, LogLevel lvl, char const* file, int line)
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

}