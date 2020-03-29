add_subdirectory(common)

add_executable(spgate
    spgate_main.cpp
    )

target_link_libraries(spgate common pthread)
