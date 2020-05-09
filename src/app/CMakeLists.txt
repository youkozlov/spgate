add_subdirectory(common)

configure_file(SpGateConfig.hpp.in SpGateConfig.hpp)

add_executable(spgate
    spgate_main.cpp
    )

target_link_libraries(spgate common pthread)

target_include_directories(spgate PUBLIC
                           "${CMAKE_CURRENT_BINARY_DIR}"
                           )