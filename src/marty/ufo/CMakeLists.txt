set(ALL_SOURCES
    charge.h
    colorcoupling.h
    diracmatrix.h
    electriccharge.cpp
    electriccharge.h
    field.h
    lorentzcoupling.h
    marty-ufo.h
    model_sqed.cpp
    model_sqed.h
    parsefactors.cpp
    parsefactors.h
    simplifiedvertex.cpp
    simplifiedvertex.h
    ufosmdata.h
    vertexinfo.h
)

add_library(marty-ufo OBJECT ${ALL_SOURCES})
set_property(TARGET marty-ufo PROPERTY POSITION_INDEPENDENT_CODE 1)
