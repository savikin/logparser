cmake_minimum_required(VERSION 3.28)

project(logparser)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_executable(logparser src/main.cpp)
target_include_directories(logparser PUBLIC src)

add_executable(tests src/test.cpp)
target_include_directories(tests PUBLIC src)
target_link_libraries(tests -lgtest -lgtest_main)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  add_executable(logparser_sanitized src/main.cpp)
  target_include_directories(logparser_sanitized PUBLIC src)
  target_compile_options(logparser_sanitized PUBLIC -Wall -Wextra -Wpedantic -fsanitize=address,undefined,leak)
  target_link_options(logparser_sanitized PUBLIC -fsanitize=address,undefined,leak)

  add_executable(tests_sanitized src/test.cpp)
  target_include_directories(tests_sanitized PUBLIC src)
  target_link_libraries(tests_sanitized -lgtest -lgtest_main)
  target_compile_options(tests_sanitized PUBLIC -Wall -Wextra -Wpedantic -fsanitize=address,undefined,leak)
  target_link_options(tests_sanitized PUBLIC -fsanitize=address,undefined,leak)
endif()

