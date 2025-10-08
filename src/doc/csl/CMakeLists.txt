configure_file(Doxyfile.in Doxyfile)

add_custom_target(doc-csl
    COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)