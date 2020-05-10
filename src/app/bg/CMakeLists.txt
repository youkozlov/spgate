
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fasynchronous-unwind-tables -fpie -fpic -fstack-protector -pipe -Werror=format-security")
#-fsanitize=address -fsanitize=leak -fsanitize=undefined

#
# bg configuration
#
add_library(bg
    bus/modbus/ModbusServer.cpp
    bus/modbus/ModbusClient.cpp
    bus/spbus/SpBusClient.cpp
    bus/spbus/SpBusCodec.cpp
    bus/spbus/SpBusRx.cpp
    bus/spbus/SpBusServer.cpp
    bus/rsbus/RsBusServer.cpp
    bus/rsbus/RsBusClient.cpp
    bus/rsbus/RsBusFsm.cpp
    bus/rsbus/RsBusCodec.cpp
    bus/rsbus/RsBusRx.cpp
    bus/GateStorage.cpp
    bus/RegAccessor.cpp

    cli/Cli.cpp
    cli/TelnetServer.cpp
    cli/TelnetPrinter.cpp
    cli/CmdProcessor.cpp
    cli/cmds/CmdParamParser.cpp
    cli/cmds/CmdParamFormater.cpp
    cli/cmds/CmdExit.cpp
    cli/cmds/CmdShutdown.cpp
    cli/cmds/CmdInfo.cpp
    cli/cmds/CmdLog.cpp
    cli/cmds/CmdArgLogLevel.cpp
    cli/cmds/CmdStats.cpp
    cli/cmds/CmdArgStatsType.cpp
    cli/cmds/CmdStatsFormater.cpp

    BusGate.cpp
    )
target_include_directories(bg PUBLIC . bus)
target_link_libraries(bg common)

#
# spgate configuration
#
add_executable(spgate spgate_main.cpp)
target_link_libraries(spgate bg common pthread)
target_include_directories(spgate PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/..")
