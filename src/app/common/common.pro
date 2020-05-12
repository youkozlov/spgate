TEMPLATE = lib
TARGET = libcommon
CONFIG += staticlib release c++11 warn_on

DEFINES += QMAKE_VARIANT='"\\\"yes\\\""'
DEFINES += PROJECT_NAME='"\\\"spgate\\\""'
DEFINES += PROJECT_VER='"\\\"1.0.0\\\""'
DEFINES += GIT_BUILD_INFO='"\\\"$$system(git diff --quiet || echo dirty-)$$system(git rev-parse --abbrev-ref HEAD)-$$system(git rev-parse --short HEAD)\\\""'

DESTDIR = ../../../build/lib
OBJECTS_DIR = ../../../build

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
