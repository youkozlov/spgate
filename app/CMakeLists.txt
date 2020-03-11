
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fasynchronous-unwind-tables -fpie -fpic -fstack-protector -pipe -Werror=format-security")
#-fsanitize=address -fsanitize=leak -fsanitize=undefined

add_library(app 
    utils/Logger.cpp
    utils/Utils.cpp
    utils/ParamParser.cpp
    sockets/LinkAcceptorRl.cpp
    modbus/ModbusServer.cpp
    modbus/ModbusClient.cpp
    gates/GateFsm.cpp
    gates/GateStorage.cpp
    gates/GateSpBus.cpp
    gates/GateM4Bus.cpp
    SpGate.cpp
    ) 
    
target_include_directories(app PUBLIC ./types ./interfaces ./)

target_link_libraries(app platform)
