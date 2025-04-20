cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    adjacencyMatrix.h
    latexLink.cpp
    latexLink.h
    planargraph.cpp
    planargraph.h
)

add_library(grafed-core OBJECT ${ALL_SOURCES})
set_property(TARGET grafed-core PROPERTY POSITION_INDEPENDENT_CODE 1)
