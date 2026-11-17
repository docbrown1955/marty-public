# Tests

Three test suites, all wired into CTest (`ctest --output-on-failure`):

| Folder | Kind | CTest label |
|--------|------|-------------|
| `units/` | Google Test unit tests for CSL and small MARTY pieces | `units` |
| `integration/` | Google Test tests of full model computations and the gamma API | `integration` |
| `system/` | Physics programs compared with reference outputs | `system` |

Run one suite with `ctest -L units` (or `integration`, `system`).

`system/libsrc/` holds sources of example and generated libraries used by some system tests
(`C7_SM.cpp`, `C8_SM.cpp`, `ckm.*`, `example_bsmixing*.cpp`). They are linked against the libraries
that the system tests generate under `tests/system/libs/`, which the cleanup test removes.

The test targets link `marty` and are compiled with `MARTY_LIBRARY_PATH` and `MARTY_INCLUDE_PATH`
pointing at the build and source trees. Google Test v1.15.2 is fetched by CMake.

See [../docs/architecture/build-and-tests.md](../docs/architecture/build-and-tests.md) for how
to add a test. `CodeCoverage.cmake` is the helper used when configuring with `-DENABLE_COVERAGE=ON`.
