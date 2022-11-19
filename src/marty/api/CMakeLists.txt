set(API_SOURCES
    gamma.h
    gamma.cpp
)

add_library(marty-gamma-api OBJECT ${API_SOURCES})
set_property(TARGET marty-gamma-api PROPERTY POSITION_INDEPENDENT_CODE 1)
