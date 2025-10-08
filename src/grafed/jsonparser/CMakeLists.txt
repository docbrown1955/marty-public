set(ALL_SOURCES
    jsonObject.h
    jsonLoader.h
    jsonLoader.cpp
)

add_library(jsonparser OBJECT ${ALL_SOURCES})

set_property(TARGET jsonparser PROPERTY POSITION_INDEPENDENT_CODE 1)