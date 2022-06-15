set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    pmssm_content.cpp
    pmssm_data.h
    pmssm_interaction.cpp
    pmssm_kinetic.cpp
    pmssm_lem.h
    pmssm_mass.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_library(marty-models OBJECT ${ALL_SOURCES})
