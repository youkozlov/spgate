
add_executable(ttyg TtyGate.cpp ttyg_main.cpp)
target_link_libraries(ttyg common pthread)
target_include_directories(ttyg PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/..")
