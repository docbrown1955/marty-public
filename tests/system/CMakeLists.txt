# Always Release mode to speed up testing
# Comment to compile in debug mode
set(CMAKE_BUILD_TYPE Release)

function(ADD_SYSTEM_TEST DISPLAY_NAME NAME)
    set(CLEANUP_NAME "CLEANUP_${DISPLAY_NAME}")
    add_executable(${NAME}.x src/${NAME}.cpp)
    target_link_libraries(${NAME}.x marty)
    add_test(NAME "${DISPLAY_NAME}"
        COMMAND ${NAME}.x
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME "${CLEANUP_NAME}"
        COMMAND bash -c "rm -rf libs/${NAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties("${DISPLAY_NAME}"
        PROPERTIES FIXTURES_CLEANUP "${CLEANUP_NAME}")
endfunction()

ADD_SYSTEM_TEST("basic-feynman-integrals" FItests)
ADD_SYSTEM_TEST("majorana-loops-simplifications" majoranaLoop)
ADD_SYSTEM_TEST("relative-sign-of-fermion-number-violating-interactions" testConjugation)

ADD_SYSTEM_TEST("decay-widths_Standard-Model" widthsSM)
ADD_SYSTEM_TEST("muon-pair-production-lep_Standard-Model" Zmumu)
ADD_SYSTEM_TEST("top-pair-production-lhc_Standard-Model" gg_to_tt)
ADD_SYSTEM_TEST("electro-weak-gauge-cancellations" EWGauge)

ADD_SYSTEM_TEST("muon-magnetic-moment_Standard-Model" gm2SM)
ADD_SYSTEM_TEST("muon-magnetic-moment_Super-Symmetry" gm2PMSSM)

ADD_SYSTEM_TEST("c7_Standard-Model" C7_SM)
ADD_SYSTEM_TEST("c7_Super-Symmetry" C7_PMSSM)
ADD_SYSTEM_TEST("c8_Standard-Model" C8_SM)
ADD_SYSTEM_TEST("c8_Super-Symmetry" C8_PMSSM)
ADD_SYSTEM_TEST("c9_Standard-Model" C9_SM)
ADD_SYSTEM_TEST("c9_Super-Symmetry" C9_MSSM)
ADD_SYSTEM_TEST("wilson-coefficients_GTHDM-Model" wilsons_GTHDM)
ADD_SYSTEM_TEST("bs-mixing_Standard-Model" BsMixingSM)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-5-6" BsMixingVLQ_5_6)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-7-8" BsMixingVLQ_7_8)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-9-10" BsMixingVLQ_9_10)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-10-11" BsMixingVLQ_10_11)

ADD_SYSTEM_TEST("lepto-genesis" NLepto)
ADD_SYSTEM_TEST("baryo-genesis" Bgenesis)
ADD_SYSTEM_TEST("TIDM-Model" SM_2IDM)
ADD_SYSTEM_TEST("lepto-genesis_SU2-Triplet-Model" SM_triplet)
