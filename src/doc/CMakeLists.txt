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

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doc/csl/html)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doc/marty/html)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doc/common/)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/.doc-stamp
    DEPENDS doc
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CSL_DOC_DIR}/html ${CMAKE_SOURCE_DIR}/docs/doc/csl/html
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${MARTY_DOC_DIR}/html ${CMAKE_SOURCE_DIR}/docs/doc/marty/html
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${COMMON_DOC_DIR} ${CMAKE_SOURCE_DIR}/docs/doc/common
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/.doc-stamp
)

add_custom_target(update-doc
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/.doc-stamp
)