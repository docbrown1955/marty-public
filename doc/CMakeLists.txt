set(COMMON_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/common)
set(CSL_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/csl)
set(MARTY_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/marty)

set(CSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/csl)
set(MARTY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/marty)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/common DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/csl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/marty DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(csl)
add_subdirectory(marty)

add_custom_target(doc 
    DEPENDS doc-marty doc-csl
)