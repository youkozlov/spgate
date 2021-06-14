TEMPLATE = app
TARGET = spgate
CONFIG += release c++11 warn_on

DESTDIR = ../../../build/bin
OBJECTS_DIR = ../../../build

include(../app.pri)

INCLUDEPATH += ../../../externals/rl \
               ../common \
               bus

SOURCES +=  spgate_main.cpp \
            bus/modbus/ModbusServer.cpp \
            bus/modbus/ModbusClient.cpp \
            bus/spbus/SpBusClient.cpp \
            bus/spbus/SpBusCodec.cpp \
            bus/spbus/SpBusRx.cpp \
            bus/spbus/SpBusServer.cpp \
            bus/rsbus/RsBusServer.cpp \
            bus/rsbus/RsBusClient.cpp \
            bus/rsbus/RsBusFsm.cpp \
            bus/rsbus/RsBusCodec.cpp \
            bus/rsbus/RsBusRx.cpp \
            bus/GateStorage.cpp \
            bus/RegAccessor.cpp \
            bus/LinkLocker.cpp \
            cli/Cli.cpp \
            cli/TelnetServer.cpp \
            cli/TelnetPrinter.cpp \
            cli/CmdProcessor.cpp \
            cli/cmds/CmdParamParser.cpp \
            cli/cmds/CmdParamFormater.cpp \
            cli/cmds/CmdExit.cpp \
            cli/cmds/CmdShutdown.cpp \
            cli/cmds/CmdInfo.cpp \
            cli/cmds/CmdLog.cpp \
            cli/cmds/CmdArgLogLevel.cpp \
            cli/cmds/CmdStats.cpp \
            cli/cmds/CmdArgStatsType.cpp \
            cli/cmds/CmdStatsFormater.cpp \
            BusGate.cpp

LIBS += -L../../../build/lib -lcommon -lrl

QMAKE_CLEAN += ./Makefile
