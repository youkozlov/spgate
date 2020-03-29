
add_library(rl 
    rlcutil.cpp
    rlinifile.cpp
    rlsocket.cpp
    rlthread.cpp
    rlwthread.cpp
)

target_include_directories(app PUBLIC .)

