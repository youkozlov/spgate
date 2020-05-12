TEMPLATE = lib
TARGET = libcommon
CONFIG += staticlib release c++11 warn_on

DESTDIR = ../../../build/lib
OBJECTS_DIR = ../../../build

include(../app.pri)

INCLUDEPATH += ../../../externals/rl

SOURCES +=  sm/ClientFsm.cpp \
            sm/ServerFsm.cpp \
            sockets/LinkAcceptorRl.cpp \
            sockets/LinkPool.cpp \
            serial/SerialPortRl.cpp \
            serial/SerialPortFile.cpp \
            utils/Logger.cpp \
            utils/Utils.cpp \
            utils/ParamParser.cpp \
            utils/Timer.cpp

QMAKE_EXTENSION_STATICLIB = a
QMAKE_CLEAN += ./Makefile
