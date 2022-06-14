cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
adjacencyMatrix.h
latexLink.cpp
latexLink.h
planargraph.cpp
planargraph.h
)

add_library(grafed-core ${ALL_SOURCES})
target_link_libraries(grafed-core grafed-jsonparser)
