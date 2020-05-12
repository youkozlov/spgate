TEMPLATE = app
TARGET = ttyg
CONFIG += release c++11 warn_on

DEFINES += QMAKE_VARIANT='"\\\"yes\\\""'
DEFINES += PROJECT_NAME='"\\\"spgate\\\""'
DEFINES += PROJECT_VER='"\\\"1.0.0\\\""'
DEFINES += GIT_BUILD_INFO='"\\\"$$system(git diff --quiet || echo dirty-)$$system(git rev-parse --abbrev-ref HEAD)-$$system(git rev-parse --short HEAD)\\\""'

DESTDIR = ../../../build/bin
OBJECTS_DIR = ../../../build

INCLUDEPATH += ../../../externals/rl \
               ../common

LIBS += -L../../../build/lib -lcommon -lrl

SOURCES +=  ttyg_main.cpp \
            TtyGate.cpp
