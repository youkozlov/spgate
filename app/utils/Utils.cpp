#include "Utils.hpp"
#include <time.h>
 
namespace sg
{
    
void Utils::nsleep(unsigned long ns)
{
    struct timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = ns; // 1мс
    nanosleep(&tv, NULL);
}

}