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

SET_SOURCE_FILES_PROPERTIES(
    pmssm_content.cpp
    pmssm_interaction.cpp
    pmssm_kinetic.cpp
    pmssm_mass.cpp
    PROPERTIES COMPILE_FLAGS -O0)
add_library(marty-models OBJECT ${ALL_SOURCES})
set_property(TARGET marty-models PROPERTY POSITION_INDEPENDENT_CODE 1)
