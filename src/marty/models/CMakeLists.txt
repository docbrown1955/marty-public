set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    qed.h qed.cpp
    qcd.h qcd.cpp
    sm.h sm.cpp
    thdm.h thdm.cpp
    gthdm.h gthdm.cpp
    mssm.h mssmData.h mssm.cpp
    pmssm.h pmssm.cpp
    nmfv.h nmfv.cpp

    pmssm_lem.h
    pmssm_lem/pmssm_content.cpp
    pmssm_lem/pmssm_data.h
    pmssm_lem/pmssm_interaction.cpp
    pmssm_lem/pmssm_kinetic.cpp
    pmssm_lem/pmssm_lem.h
    pmssm_lem/pmssm_mass.cpp
)

SET_SOURCE_FILES_PROPERTIES(
    pmssm_lem/pmssm_content.cpp
    pmssm_lem/pmssm_interaction.cpp
    pmssm_lem/pmssm_kinetic.cpp
    pmssm_lem/pmssm_mass.cpp
    PROPERTIES COMPILE_FLAGS -O0)

add_library(marty-models OBJECT ${ALL_SOURCES})
set_property(TARGET marty-models PROPERTY POSITION_INDEPENDENT_CODE 1)
