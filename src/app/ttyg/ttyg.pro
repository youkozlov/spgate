TEMPLATE = app
TARGET = ttyg
CONFIG += release c++11 warn_on

DESTDIR = ../../../build/bin
OBJECTS_DIR = ../../../build

include(../app.pri)

INCLUDEPATH += ../../../externals/rl \
               ../common

SOURCES +=  ttyg_main.cpp \
            TtyGate.cpp

LIBS += -L../../../build/lib -lcommon -lrl

QMAKE_CLEAN += ./Makefile
