configure_file(dox-config.in dox-config)
add_custom_target(doc-marty
    COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/dox-config 
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS doc-csl
)