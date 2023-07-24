add_subdirectory(csl)
add_subdirectory(marty)

add_custom_target(doc 
    DEPENDS doc-marty doc-csl
)