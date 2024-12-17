cmake_minimum_required(VERSION 3.28)

project(logparser)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

add_executable(logparser src/main.cpp)
target_include_directories(logparser PUBLIC src)
