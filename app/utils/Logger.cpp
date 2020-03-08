#include "Logger.hpp"
#include "stdio.h"
#include <ctime>

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
    }
    return "INVALID";
}

void Logger::dispatch(char const* msg, LogLevel lvl, char const* file, int line)
{
    printf("%lu %s %s %d: %s\n", time(0), toString(lvl), strrchr(file, '/') + 1, line, msg);
}

}