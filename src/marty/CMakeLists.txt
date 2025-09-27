add_subdirectory(core)
add_subdirectory(models)
add_subdirectory(sgl)
add_subdirectory(ufo)
add_subdirectory(lha)
add_subdirectory(api)

add_library(marty SHARED)
target_link_libraries(marty PUBLIC 
    csl-core 
    grafed-core 
    jsonparser 
    marty-lha-obj 
    marty-core 
    marty-ufo 
    sgl 
    marty-models 
    marty-gamma-api
    GSL::gsl
    GSL::gslcblas
    ${PROJECT_BINARY_DIR}/libooptools.so
)

add_library(marty-static STATIC)
target_link_libraries(marty-static PUBLIC 
    csl-core 
    grafed-core 
    jsonparser 
    marty-lha-obj 
    marty-core 
    marty-ufo 
    sgl 
    marty-models 
    marty-gamma-api
    GSL::gsl
    GSL::gslcblas
    ${PROJECT_BINARY_DIR}/libooptools.so
)