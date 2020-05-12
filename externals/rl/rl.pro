TEMPLATE = lib
TARGET = librl
CONFIG += static release c++11 warn_on
QMAKE_EXTENSION_STATICLIB = a

DESTDIR = ../../build/lib
OBJECTS_DIR = ../../build

SOURCES +=  rlcutil.cpp \
            rlinifile.cpp \
            rlserial.cpp \
            rlsocket.cpp \
            rlthread.cpp \
            rlwthread.cpp
