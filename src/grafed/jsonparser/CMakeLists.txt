cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    jsonObject.h
    jsonLoader.h
    jsonLoader.cpp
)

add_library(jsonparser OBJECT ${ALL_SOURCES})

set_property(TARGET jsonparser PROPERTY POSITION_INDEPENDENT_CODE 1)