
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fasynchronous-unwind-tables -fpie -fpic -fstack-protector -pipe -Werror=format-security")
#-fsanitize=address -fsanitize=leak -fsanitize=undefined

add_definitions( -DGTEST_HAS_PTHREAD=1 )

add_executable(app_ut
    BusGateTest.cpp
    SpBusClientTest.cpp
    ModbusServerTest.cpp
    ParamParserTest.cpp
    UtilsTest.cpp
    )

target_link_libraries(app_ut gmock gmock_main gtest gtest_main app)
