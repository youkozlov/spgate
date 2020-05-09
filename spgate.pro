######################################################################
# Automatically generated by qmake (3.0) ?? ????. 29 18:23:50 2020
######################################################################

TEMPLATE = app
TARGET = spgate
CONFIG += release c++11 warn_on

INCLUDEPATH += . \
           externals/rl \
           src/app \
           src/app/common \
           src/app/common/bus

SOURCES += src/app/spgate_main.cpp \
           externals/rl/rlcutil.cpp \
           externals/rl/rlinifile.cpp \
           externals/rl/rlserial.cpp \
           externals/rl/rlsharedmemory.cpp \
           externals/rl/rlsocket.cpp \
           externals/rl/rlthread.cpp \
           externals/rl/rlwthread.cpp \
           externals/rl/shm.cpp \
           src/app/common/bus/modbus/ModbusServer.cpp \
           src/app/common/bus/modbus/ModbusClient.cpp \
           src/app/common/bus/spbus/SpBusClient.cpp \
           src/app/common/bus/spbus/SpBusCodec.cpp \
           src/app/common/bus/spbus/SpBusRx.cpp \
           src/app/common/bus/spbus/SpBusServer.cpp \
           src/app/common/bus/rsbus/RsBusServer.cpp \
           src/app/common/bus/rsbus/RsBusClient.cpp \
           src/app/common/bus/rsbus/RsBusFsm.cpp \
           src/app/common/bus/rsbus/RsBusCodec.cpp \
           src/app/common/bus/rsbus/RsBusRx.cpp \
           src/app/common/bus/GateStorage.cpp \
           src/app/common/bus/RegAccessor.cpp \
           src/app/common/cli/Cli.cpp \
           src/app/common/cli/TelnetServer.cpp \
           src/app/common/cli/TelnetPrinter.cpp \
           src/app/common/cli/CmdProcessor.cpp \
           src/app/common/cli/cmds/CmdParamParser.cpp \
           src/app/common/cli/cmds/CmdParamFormater.cpp \
           src/app/common/cli/cmds/CmdExit.cpp \
           src/app/common/cli/cmds/CmdShutdown.cpp \
           src/app/common/cli/cmds/CmdInfo.cpp \
           src/app/common/cli/cmds/CmdLog.cpp \
           src/app/common/cli/cmds/CmdArgLogLevel.cpp \
           src/app/common/cli/cmds/CmdStats.cpp \
           src/app/common/sm/ClientFsm.cpp \
           src/app/common/sm/ServerFsm.cpp \
           src/app/common/sockets/LinkAcceptorRl.cpp \
           src/app/common/utils/Logger.cpp \
           src/app/common/utils/Utils.cpp \
           src/app/common/utils/ParamParser.cpp \
           src/app/common/utils/Timer.cpp \
           src/app/common/BusGate.cpp
