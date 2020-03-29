# spgate

This is a proxy which provides access to spbus(rsbus) devices via modbus-tcp protocol.
Main goal to support communication with devices like 961, 761(spbus) and devices like 941,741(rsbus).
Target platform is SMH4(PLC).

Application should configured with the *.ini file, where you have to define spbus(rsbus) related parameters and also to define channels and address of device technological parameters.

## Build and execution unit tests:

```html 
$ git clone https://github.com/youkozlov/spgate.git
$ cd spgate
$ git submodule update --init
$ mkdir build_cmake
$ cd build_cmake
$ cmake ..
$ make
$ bin/spgate_ut
```
