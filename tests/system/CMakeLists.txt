
function(ADD_SYSTEM_TEST NAME)
    add_executable(${NAME}.x src/${NAME}.cpp)
    target_link_libraries(${NAME}.x marty)
    add_test(NAME ${NAME}
        COMMAND ${NAME}.x
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME CLEANUP_${NAME}
        COMMAND bash -c "rm -rf libs/${NAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(${NAME}
        PROPERTIES FIXTURES_CLEANUP CLEANUP_${NAME})
endfunction()

# ADD_SYSTEM_TEST(Bgenesis)
# ADD_SYSTEM_TEST(C7_SM)
# ADD_SYSTEM_TEST(C7_PMSSM)
# ADD_SYSTEM_TEST(C8_PMSSM)
# ADD_SYSTEM_TEST(FItests)
# ADD_SYSTEM_TEST(gm2SM)
# ADD_SYSTEM_TEST(gm2PMSSM)
# ADD_SYSTEM_TEST(NLepto)
# ADD_SYSTEM_TEST(gg_to_tt)
# ADD_SYSTEM_TEST(Zmumu)
ADD_SYSTEM_TEST(widthsSM)

# ADD_SYSTEM_TEST(C8_SM)
# ADD_SYSTEM_TEST(BsMixingSM)