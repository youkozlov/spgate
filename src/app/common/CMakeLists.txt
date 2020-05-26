
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fasynchronous-unwind-tables -fpie -fpic -fstack-protector -pipe -Werror=format-security")
#-fsanitize=address -fsanitize=leak -fsanitize=undefined

add_library(common

    utils/Utils.cpp
    utils/ParamParser.cpp
    utils/Timer.cpp

    sm/ClientFsm.cpp
    sm/ServerFsm.cpp

    sockets/LinkAcceptorRl.cpp
    sockets/LinkPool.cpp

    serial/SerialPortRl.cpp

    )

target_include_directories(common PUBLIC .)

target_link_libraries(common rl)
