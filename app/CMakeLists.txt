
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fasynchronous-unwind-tables -fsanitize=undefined -fpie -fpic -fstack-protector -pipe -Werror=format-security")


add_library(app 
    utils/Logger.cpp
    utils/Utils.cpp
    utils/ParamParser.cpp
    sockets/LinkAcceptorRl.cpp
    modbus/ModbusServer.cpp
    modbus/ModbusClient.cpp
    SpGate.cpp
    FeProcessorSpNet.cpp
    FeProcessorM4Net.cpp
    ) 
    
target_include_directories(app PUBLIC ./types ./interfaces ./)

target_link_libraries(app platform)
