
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEBUG -O0 -g -ggdb -Wextra -Wall -Wpedantic -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fasynchronous-unwind-tables -fsanitize=undefined -fpie -fpic -fstack-protector -pipe -Werror=format-security")

add_definitions( -DGTEST_HAS_PTHREAD=1 )

add_executable(app_ut
    SpGateTest.cpp
    SpNetTest.cpp
    ModbusServerTest.cpp
    ParamParserTest.cpp
    )

target_link_libraries(app_ut gmock gmock_main gtest gtest_main app)
