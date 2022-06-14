cmake_minimum_required(VERSION 3.5)

project(marty LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(csl)
add_subdirectory(grafed)
add_subdirectory(marty)

add_library(marty INTERFACE)

target_link_libraries(marty INTERFACE csl grafed marty-core)
