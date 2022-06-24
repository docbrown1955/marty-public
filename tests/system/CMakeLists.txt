
function(ADD_SYSTEM_TEST DISPLAY_NAME NAME)
    set(CLEANUP_NAME "Cleanup -- ${DISPLAY_NAME}")
    add_executable(${NAME}.x src/${NAME}.cpp)
    target_link_libraries(${NAME}.x marty)
    add_test(NAME ${DISPLAY_NAME}
        COMMAND ${NAME}.x
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME ${CLEANUP_NAME}
        COMMAND bash -c "rm -rf libs/${NAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(${DISPLAY_NAME}
        PROPERTIES FIXTURES_CLEANUP ${CLEANUP_NAME})
endfunction()

ADD_SYSTEM_TEST("Basic Feynman Integrals" FItests)
ADD_SYSTEM_TEST("Majorana Loops Simplifications" majoranaLoop)
ADD_SYSTEM_TEST("Relative Sign of Fermion-Number Violating Interactions" testConjugation)

ADD_SYSTEM_TEST("Decay Widths (Standard Model)" widthsSM)
ADD_SYSTEM_TEST("Muon Pair Production LEP (Standard Model)" Zmumu)
ADD_SYSTEM_TEST("Top Pair Production LHC (Standard Model)" gg_to_tt)
ADD_SYSTEM_TEST("Electro-Weak Gauge Cancellations" EWGauge)

ADD_SYSTEM_TEST("Muon Magnetic Moment (Standard Model)" gm2SM)
ADD_SYSTEM_TEST("Muon Magnetic Moment (Super-Symmetry)" gm2PMSSM)

ADD_SYSTEM_TEST("C7 (Standard Model)" C7_SM)
ADD_SYSTEM_TEST("C7 (Super-Symmetry)" C7_PMSSM)
ADD_SYSTEM_TEST("C8 (Standard Model)" C8_SM)
ADD_SYSTEM_TEST("C8 (Super-Symmetry)" C8_PMSSM)
ADD_SYSTEM_TEST("C9 (Standard Model)" C9_SM)
ADD_SYSTEM_TEST("C9 (Super-Symmetry)" C9_MSSM)
ADD_SYSTEM_TEST("Wilson Coefficients (GTHDM Model)" wilsons_GTHDM)
ADD_SYSTEM_TEST("Bs-Mixing (Standard Model)" BsMixingSM)

ADD_SYSTEM_TEST("Lepto-Genesis" NLepto)
ADD_SYSTEM_TEST("Baryo-Genesis" Bgenesis)
ADD_SYSTEM_TEST("TIDM Model" SM_2IDM)
ADD_SYSTEM_TEST("Lepto-Genesis (SU(2) Triplet Model)" SM_triplet)
