# Build system and test suites

## 1. Build overview

The project is a single CMake project (`CMakeLists.txt`, minimum CMake 3.22, C++23). The
top-level `src/CMakeLists.txt` adds these subprojects:

| Subdirectory | Produces |
|--------------|----------|
| `src/csl` | `csl` (shared), `csl-static` (static), `csl-core` (object library) |
| `src/marty` | `marty` (shared), `marty-static` (static), and object libraries for `core`, `models`, `sgl`, `ufo`, `lha`, `api` |
| `src/marty/lha` | `marty-lha` (shared), `marty-lha-static` |
| `src/marty-plugins/boltzmann-solver` | `marty-boltzmann-solver` (shared), `marty-boltzmann-solver-static` |
| `src/grafed` | `grafed-gui` (executable) |
| `src/doc` | Doxygen targets for CSL and MARTY |
| `examples` | one `<name>.x` executable per sample (`gettingStarted.x`, `sample2HDM.x`, ...) |
| `tests` | Google Test executables (`*.x`) and system-test executables |

Object libraries let the shared and static libraries share the same compiled code.

### Dependencies

Compile time:
- GSL and GSL CBLAS (`find_package(GSL REQUIRED)`).
- Qt 5 for GRAFED.
- Fortran compiler for LoopTools (`dep/LoopTools-2.16`, built by the `dep/` CMake).
- GNU compilers (`gcc`, `g++`, `gfortran`) of the same major version, at least 11. Quadruple
  precision is only enabled with GNU compilers.

Run time (for GRAFED and generated code):
- `latex`, `lualatex`, `dvipng` (LaTeX labels in diagrams) and `convert` (ImageMagick).
- Missing `lualatex`, `dvipng` or `convert` is a configure error.

Test-only: Google Test v1.15.2 is fetched with `FetchContent` in `tests/CMakeLists.txt`.

### Compile definitions

The build defines `MARTY_CXX`, `MARTY_CC`, `MARTY_FC` (the compilers used) and
`MARTY_INSTALL_PATH`. MARTY uses them at run time to compile the libraries it generates.
Build types: `Release` (default, `-O3`) and `Debug`. Coverage is a separate option,
`-DENABLE_COVERAGE=ON`, which adds gcov flags on top of the build type and enables
`tests/CodeCoverage.cmake`.

## 2. Typical commands

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<prefix>
make -j <n_jobs>
ctest -j <n_jobs> --output-on-failure
make install
```

Documentation: `make doc` builds the Doxygen pages for CSL and MARTY (`doc-csl` and `doc-marty`
build one each). `make update-doc` builds them and copies the HTML into `docs/doc/csl/html`,
`docs/doc/marty/html` and `docs/doc/common`, so those committed files change when it runs.

## 3. Test suites

Tests are declared in `tests/CMakeLists.txt`, which includes three subfolders. Every test target
links `marty` and is given `MARTY_LIBRARY_PATH` and `MARTY_INCLUDE_PATH`.

| Suite | Location | Mechanism | CTest label |
|-------|----------|-----------|-------------|
| Units | `tests/units/` | gtest executables, `SETUP_GTEST_TARGET` + `gtest_discover_tests` | `units` |
| Integration | `tests/integration/` | gtest executables (flavour groups, gauge couplings, gamma API) | `integration` |
| System | `tests/system/` | Physics programs (`src/*.cpp`) run from the source folder and compared with reference outputs in `data/output` | `system` |

Notes:

- **Units** cover CSL (numbers, literals, equality, the init sanitiser) and a few MARTY
  parameters (SM parameter definitions) and the Boltzmann-solver quantum numbers.
- **Integration** tests exercise full model calculations and the gamma API.
- **System** tests are full physics computations (Wilson coefficients, widths, Majorana loops,
  LHA reading). Each one is registered by `ADD_SYSTEM_TEST(name, file)`, runs from
  `tests/system`, and has a cleanup test that removes `libs/<name>`. Results are compared with
  `src/comparedata.h` within a tolerance. Quadruple-precision tests are only added when the
  feature is enabled.
- The generated test libraries and the coverage build are both driven from CTest labels, so
  `ctest -L units` (or `integration`, `system`) runs one suite.

## 4. Continuous integration

`.github/workflows/c-cpp.yml` runs one job per toolchain: Ubuntu 24.04 with GCC 14, Ubuntu 22.04
with GCC 11, and Ubuntu 24.04 with Clang 18 (libstdc++ from GCC 14). Each job
installs the dependencies, runs `cmake`, `make` and `ctest --output-on-failure`. The
`.devcontainer/` folder provides a Fedora 41 image (GCC 14) for development.

## 5. Adding a test

- Unit or integration test: add `src/<name>.cpp` next to the existing ones and call
  `SETUP_MARTY_TEST_TARGET` and `SETUP_GTEST_TARGET` (follow the pattern of
  `tests/integration/CMakeLists.txt`).
- System test: add the program under `tests/system/src/`, add a reference output file under
  `tests/system/data/output/`, and register it with `ADD_SYSTEM_TEST`.
