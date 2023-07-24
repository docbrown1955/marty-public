add_custom_target(doc-marty
    COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/dox-config 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS doc-csl
)