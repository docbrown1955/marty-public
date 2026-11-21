set(API_SOURCES
    diracology.h
    diracology.cpp
)

add_library(marty-gamma-api OBJECT ${API_SOURCES})
set_property(TARGET marty-gamma-api PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(marty-group-api OBJECT group_theory/group_theory.h group_theory/group_theory.cpp)
set_property(TARGET marty-group-api PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(marty-modeling-api OBJECT modeling/modeling.h modeling/modeling.cpp)
set_property(TARGET marty-modeling-api PROPERTY POSITION_INDEPENDENT_CODE 1)
