# Distributed under the OSI-approved BSD 3-Clause License.

#[=======================================================================[.rst:
Pytest
----------

This module defines functions to help integrate Pytest scripts into CMake.

.. command:: pytest_discover_tests

  Automatically add tests with CTest by querying the compiled test executable
  for available tests::

    pytest_discover_tests([EXTRA_ARGS arg1...]
                         [WORKING_DIRECTORY dir]
                         [TEST_PREFIX prefix]
                         [TEST_SUFFIX suffix]
                         [XML_OUTPUT_DIR dir]
    )

  ``pytest_discover_tests()`` will collect all pytest test cases and register
  commands that execute them through CTest.

  The main advantage of using this instead of just calling pytest directly is
  that you can synchronize filtering and other configuration within CMake. The
  only disadvantage of using this however is that pytest does test collection
  whether or not test filtering is used, so this can add significant overhead.

  The options are:

  ``EXTRA_ARGS arg1...``
    Any extra arguments to pass on the command line to pytest

  ``COLLECTION_ARGS arg1...``
    Extra arguments to pass to pytest during test case collection

  ``EXECUTION_ARGS arg1...``
    Extra arguments to pass to pytest during test case execution

  ``WORKING_DIRECTORY dir``
    Specifies the directory in which to run test case collection and test case
    execution.  If this
    option is not provided, the current source directory is used.

  ``TEST_PREFIX prefix``
    Specifies a ``prefix`` to be prepended to the name of each discovered test
    case.  This can be useful when using multiple test folders and there are
    potentially clashing test names, it can also make test filtering in presets
    easier. If this option is not provided then the prefix is empty.

  ``TEST_SUFFIX suffix``
    Similar to ``TEST_PREFIX`` except the ``suffix`` is appended to the name of
    every discovered test case.  Both ``TEST_PREFIX`` and ``TEST_SUFFIX`` may
    be specified.

  ``XML_OUTPUT_DIR dir``

    If specified, the parameter is passed along with ``--junitxml=<test name>``
    to test executable. The actual file name is the same as the test target,
    including prefix and suffix.

#]=======================================================================]

cmake_policy(PUSH)

#------------------------------------------------------------------------------
function(pytest_discover_tests)

cmake_parse_arguments(
    ""
    ""
    "TEST_PREFIX;TEST_SUFFIX;WORKING_DIRECTORY;XML_OUTPUT_DIR"
    "EXTRA_ARGS;COLLECTION_ARGS;EXECUTION_ARGS"
    ${ARGN}
  )

  # Default settings
  if(NOT _WORKING_DIRECTORY)
    set(_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()

  # https://docs.pytest.org/en/6.2.x/example/pythoncollection.html#finding-out-what-is-collected
  set(pytest_collection_args --collect-only -q ${_EXTRA_ARGS} ${_COLLECTION_ARGS})
  set(pytest_execution_args ${_EXTRA_ARGS} ${_EXECUTION_ARGS})
  set(pytest_base_command pytest)

  execute_process(
    COMMAND ${pytest_base_command} ${pytest_collection_args}
    WORKING_DIRECTORY ${_WORKING_DIRECTORY}
    OUTPUT_VARIABLE pytest_collect_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE pytest_collect_result
  )
  if(NOT ${pytest_collect_result} EQUAL 0)
    message(WARNING "Error running Pytest discovery, tests for the python API will not be ran...")
    return()
  endif()

  # Splits lines into a list variable
  string(REPLACE [[;]] [[\;]] pytest_collect_output "${pytest_collect_output}")
  string(REPLACE "\n" ";" pytest_collect_output "${pytest_collect_output}")

  set(pytest_testcase_regex "([^:]+)(::([^:]+))?::([^:]+)")

  foreach(test_case ${pytest_collect_output})
    string(REGEX MATCHALL ${pytest_testcase_regex} test_case_match "${test_case}")

    if(test_case_match)
      set(test_case_file ${CMAKE_MATCH_1})
      set(test_case_class ${CMAKE_MATCH_3})
      set(test_case_func ${CMAKE_MATCH_4})
      
      if(test_case_class)
        set(test_case_name "${_TEST_PREFIX}${test_case_class}.${test_case_func}${_TEST_SUFFIX}")
      else()
        set(test_case_name "${_TEST_PREFIX}${test_case_func}${_TEST_SUFFIX}")
      endif()

      if(_XML_OUTPUT_DIR)
        set(pytest_local_args --junitxml=${_XML_OUTPUT_DIR}/${test_case_name}.xml)
      endif()

      add_test(
        NAME "${test_case_name}"
        COMMAND ${pytest_base_command}
                ${test_case} ${pytest_execution_args} ${pytest_local_args}
        WORKING_DIRECTORY ${_WORKING_DIRECTORY}
      )
      # Pytest Exit code 5 means all tests were deselected
      set_tests_properties("${test_case_name}" PROPERTIES SKIP_RETURN_CODE 5)
    endif()

  endforeach()

endfunction()

#------------------------------------------------------------------------------

cmake_policy(POP)