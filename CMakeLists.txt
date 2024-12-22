cmake_minimum_required(VERSION 3.28)

project(logparser)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

#TODO: branch those on MSVC
#add_compile_options(-Wall -Wextra -Wpedantic -fsanitize=address,undefined,leak)
#add_link_options(-fsanitize=address,undefined,leak)

add_executable(logparser src/main.cpp)
target_include_directories(logparser PUBLIC src)

add_executable(tests src/test.cpp)
target_include_directories(tests PUBLIC src)
target_link_libraries(tests -lgtest -lgtest_main)
