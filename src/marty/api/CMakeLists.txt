set(API_SOURCES
    diracology.h
    diracology.cpp
)

add_library(marty-gamma-api OBJECT ${API_SOURCES})
set_property(TARGET marty-gamma-api PROPERTY POSITION_INDEPENDENT_CODE 1)