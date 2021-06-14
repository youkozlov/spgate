
add_library(rl 
    rlcutil.cpp
    rlinifile.cpp
    rlsocket.cpp
    rlthread.cpp
    rlwthread.cpp
#    rlserial.cpp
)

target_include_directories(rl PUBLIC .)

