cmake_minimum_required(VERSION 3.5)

add_library(marty-core INTERFACE)

add_subdirectory(core)
add_subdirectory(models)
add_subdirectory(sgl)

target_link_libraries(marty-core INTERFACE marty-calc sgl marty-models)
