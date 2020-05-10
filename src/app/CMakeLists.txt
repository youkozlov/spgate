add_subdirectory(common)

execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(SpGateConfig.hpp.in SpGateConfig.hpp)

add_executable(spgate
    spgate_main.cpp
    )

target_link_libraries(spgate common pthread)

target_include_directories(spgate PUBLIC
                           "${CMAKE_CURRENT_BINARY_DIR}"
                           )