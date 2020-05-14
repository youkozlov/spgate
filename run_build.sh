#!/bin/sh

COMPILE_PWD=/opt/compiler_platform
export PATH=$PATH:${COMPILE_PWD}/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux/bin
export CROSS_COMPILE=arm-linux-gnueabihf-

/opt/compiler_platform/compiled/bin/qmake -spec linux-TIarmv7-sgx-g++ -o Makefile spgate.pro

make -f Makefile clean

make -f Makefile
