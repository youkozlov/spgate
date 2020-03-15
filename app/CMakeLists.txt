
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fasynchronous-unwind-tables -fpie -fpic -fstack-protector -pipe -Werror=format-security")
#-fsanitize=address -fsanitize=leak -fsanitize=undefined

add_library(app

    utils/Logger.cpp
    utils/Utils.cpp
    utils/ParamParser.cpp

    sm/ClientFsm.cpp
    sm/ServerFsm.cpp

    sockets/LinkAcceptorRl.cpp

    bus/modbus/ModbusServer.cpp
    bus/modbus/ModbusClient.cpp
    bus/spbus/SpBusClient.cpp
    bus/spbus/SpBusCodec.cpp
    bus/spbus/SpBusRx.cpp
    bus/spbus/SpBusServer.cpp
    bus/rsbus/RsBusClient.cpp
    bus/GateStorage.cpp

    BusGate.cpp
    ) 
    
target_include_directories(app PUBLIC . interfaces bus)

target_link_libraries(app platform)
