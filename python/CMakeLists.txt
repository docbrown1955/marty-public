set(IMPORT_MARTY_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/pymarty/_internal")
configure_file(
    "${IMPORT_MARTY_RELATIVE_PATH}/import_marty.py.in"
    "${IMPORT_MARTY_RELATIVE_PATH}/import_marty.py"
    @ONLY
)