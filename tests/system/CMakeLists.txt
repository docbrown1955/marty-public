function(ADD_SYSTEM_TEST DISPLAY_NAME NAME)
    set(CLEANUP_NAME "CLEANUP_${DISPLAY_NAME}")
    add_executable(${NAME}.x src/${NAME}.cpp)
    SETUP_MARTY_TEST_TARGET(${NAME}.x)
    add_test(NAME "${DISPLAY_NAME}"
        COMMAND ${NAME}.x
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME "${CLEANUP_NAME}"
        COMMAND bash -c "rm -rf libs/${NAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_property(TEST ${DISPLAY_NAME} PROPERTY 
        ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/src/marty/lha")
    set_tests_properties(${DISPLAY_NAME} PROPERTIES LABELS system)
    set_tests_properties(${CLEANUP_NAME} PROPERTIES LABELS system)
    set_tests_properties(${CLEANUP_NAME} PROPERTIES DEPENDS ${DISPLAY_NAME})
endfunction()

function(ADD_QUAD_SYSTEM_TEST DISPLAY_NAME NAME)
    if(NOT QUADRUPLE_ENABLED)
        message(WARNING
            "The system test \"${DISPLAY_NAME}\" cannot be compiled because the quadruple precision feature has been disabled."
        )
    else()
        ADD_SYSTEM_TEST(${DISPLAY_NAME} ${NAME})
    endif()
endfunction()

ADD_SYSTEM_TEST("lha-reader" test_lha_reader)
ADD_SYSTEM_TEST("basic-feynman-integrals" FItests)
ADD_SYSTEM_TEST("majorana-loops-simplifications" majoranaLoop)
ADD_SYSTEM_TEST("relative-sign-of-fermion-number-violating-interactions" testConjugation)
ADD_SYSTEM_TEST("simple-spectrum-generation" simpleSpectrum)
ADD_SYSTEM_TEST("auto-model-widths-computation_Standard-Model" testModelWidth)

ADD_SYSTEM_TEST("decay-widths_Standard-Model" widthsSM)
ADD_SYSTEM_TEST("muon-pair-production-lep_Standard-Model" Zmumu)
ADD_SYSTEM_TEST("top-pair-production-lhc_Standard-Model" gg_to_tt)
# Unstable test below, uncomment once stable
# ADD_SYSTEM_TEST("free-quarks-amplitudes" freeQuarkAmplitudes)
ADD_QUAD_SYSTEM_TEST("electro-weak-gauge-cancellations" EWGauge)

ADD_QUAD_SYSTEM_TEST("muon-magnetic-moment_Standard-Model" gm2SM)
ADD_QUAD_SYSTEM_TEST("muon-magnetic-moment_Super-Symmetry" gm2PMSSM)

ADD_SYSTEM_TEST("c7_Standard-Model" C7_SM)
ADD_QUAD_SYSTEM_TEST("c7_Super-Symmetry" C7_PMSSM)
ADD_SYSTEM_TEST("c8_Standard-Model" C8_SM)
ADD_QUAD_SYSTEM_TEST("c8_Super-Symmetry" C8_PMSSM)
ADD_QUAD_SYSTEM_TEST("c9_Standard-Model" C9_SM)
ADD_QUAD_SYSTEM_TEST("c9_Super-Symmetry" C9_MSSM)
ADD_QUAD_SYSTEM_TEST("wilson-coefficients_GTHDM-Model" wilsons_GTHDM)
ADD_SYSTEM_TEST("bs-mixing_Standard-Model" BsMixingSM)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-5-6" BsMixingVLQ_5_6)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-7-8" BsMixingVLQ_7_8)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-9-10" BsMixingVLQ_9_10)
ADD_SYSTEM_TEST("bs-mixing_VLQ-Model-10-11" BsMixingVLQ_10_11)

ADD_SYSTEM_TEST("lepto-genesis" NLepto)
ADD_QUAD_SYSTEM_TEST("baryo-genesis" Bgenesis)
ADD_SYSTEM_TEST("TIDM-Model" SM_2IDM)
ADD_SYSTEM_TEST("lepto-genesis_SU2-Triplet-Model" SM_triplet)
