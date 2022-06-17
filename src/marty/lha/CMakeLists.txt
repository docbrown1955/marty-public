set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    lha.cpp
    lha.h
    lhaBlocks.h
    lhaData.h
    lhaData.cpp
)

add_library(marty-lha-obj OBJECT ${ALL_SOURCES})
set_property(TARGET marty-lha-obj PROPERTY POSITION_INDEPENDENT_CODE 1)
