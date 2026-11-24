Status: READY
Updated: 2026-11-24
Author: orchestrator (consolidated from five read-only investigator reports; investigators cannot write files, so the orchestrator recorded their findings here and re-checked the lines below)

# Phase 6 investigation summary

Prerequisite: phase 5 accepted by the user on 2026-11-24 (decision given in the session, "accept_and_start"). No
acceptance record was found in the repository before that decision.

## Facts (verified by the orchestrator unless marked)

1. **The generator already emits a C layer.** Non-trivial, non-forced-C++ functions get `clib/c_<name>.h` and
   `clib/c_<name>.c` with `extern "C"` declarations. Complex results are returned by value as
   `ccomplex_return_t {creal_t real; creal_t imag;}`. `clib/ccommon.h`, `clib/cparams.h` and `clib/clib_<lib>.h` are
   generated too. Evidence: [librarygenerator.cpp:626-668](../../../../../src/csl/librarygenerator.cpp),
   [librarygenerator.cpp:950-966](../../../../../src/csl/librarygenerator.cpp),
   [librarygenerator.cpp:969-1000](../../../../../src/csl/librarygenerator.cpp),
   [libraryfunction.cpp:186-211](../../../../../src/csl/libraryfunction.cpp).
2. **The C layer is part of the shared object.** The generated Makefile builds `$(NAMELIB).so` and `$(NAMELIB).a`
   from `$(OBJ) $(COBJ)`, with `-fPIC`. Evidence: [libmakefile_data.h:41](../../../../../src/csl/libmakefile_data.h),
   [libmakefile_data.h:67-77](../../../../../src/csl/libmakefile_data.h),
   [libmakefile_data.h:85](../../../../../src/csl/libmakefile_data.h),
   [libmakefile_data.h:100-103](../../../../../src/csl/libmakefile_data.h). So WP6.0 item 2 is answered: a `.so`
   already exists.
3. **The plan's `marty_capi.cpp` (WP6.1) would duplicate item 1.** Functions with `forceCppFunc` and trivial functions
   have no C symbol ([librarygenerator.cpp:950, 962](../../../../../src/csl/librarygenerator.cpp)). The C layer is
   therefore partial, and metadata (names, parameters, defaults, result kinds) is still missing.
4. **Parameter struct.** Generated C functions take a pointer to the parameter struct (`param->name`), declared in
   `cparams.h`. The exact generated struct text was not inspected (not verified).
5. **Generated C++ signatures.** `updateSpectrum(param_t&)` for spectra, and `complex_t f(param_t const&)` or
   `double f(...)` per function ([librarygenerator.cpp:369-381](../../../../../src/csl/librarygenerator.cpp),
   [libraryfunction.cpp:216-244](../../../../../src/csl/libraryfunction.cpp)).
6. **`addFunction` overload.** `mty::Library::addFunction(name, expr, bool forceCppFunc = false)`
   ([mtylibrary.h:91-96](../../../../../src/marty/core/mtylibrary.h)). `gettingStarted` passes the string `"G"`, which
   converts to `true` (`forceCppFunc`), not to a group name ([gettingStarted.cpp:117](../../../../../examples/gettingStarted.cpp)).
   So its `squared_ampl` has no C symbol. This is a trap to avoid in the new API.
7. **Commented-out CMake shared-library code** is in `printCMakeLists()`, lines 795-847 of
   [librarygenerator.cpp](../../../../../src/csl/librarygenerator.cpp). Not used.
8. **Python extension.** `_pymarty` is built with `pybind11_add_module` and links `marty-static`
   ([python/CMakeLists.txt:20-24, 79-83](../../../../../python/CMakeLists.txt)) (investigator report, not re-checked).
9. **Dependencies of generated libraries.** `mty::Library` adds LoopTools headers and `-looptools` / `-lgfortran`
   ([mtylibrary.h:34-47](../../../../../src/marty/core/mtylibrary.h)). The generated Makefile links `-lgsl -lgslcblas`
   ([librarygenerator.cpp:756-779](../../../../../src/csl/librarygenerator.cpp)) (investigator report, partly
   verified). Consequence: GSL is a runtime requirement of generated libraries. Not verified whether it is always needed.
10. **Shared-state risk.** LoopTools has a process-wide cache ([dep/LoopTools-2.16/src/util/cache.c:22-56](../../../../../dep/LoopTools-2.16/src/util/cache.c)).
    Not verified whether loading a generated `.so` into the Python process duplicates it. Spike needed (WP6.0 item 3).
11. **Python conventions.** `bindCalculation(py::module_&)` creates `_pymarty.calculation`
    ([calculation.cpp:98-106](../../../../../python/src/calculation.cpp), verified). Package-level lazy submodule loading
    through `__getattr__` ([python/pymarty/__init__.py:17-41](../../../../../python/pymarty/__init__.py), investigator
    report). Markers `slow`, `needs_compiler`, `quad`, `loops` are declared and pytest uses `--strict-markers`
    ([python/pyproject.toml:47-60](../../../../../python/pyproject.toml), verified).
12. **Kinematics** ([tests/system/libsrc/kinematics.h:59-84](../../../../../tests/system/libsrc/kinematics.h), verified
    for `pSquared = E*E - m*m`). The 2-to-2 split and the six invariants are at lines 130-139 and 290-336 (investigator
    report, not re-checked).
13. **Comparison helper.** [tests/system/src/comparedata.h:30](../../../../../tests/system/src/comparedata.h) computes
    `abs(a-b) / (a < 1e-10 ? 1 : a)`. The `eps` argument is not used in that expression (verified).
14. **Tolerances and reference data** (from the C++ tests; the simpleSpectrum tolerance was verified at
    [simpleSpectrum.cpp:123-124](../../../../../tests/system/src/simpleSpectrum.cpp), the others are investigator reports):

| Test | Reference file (`tests/system/data/output/`) | Tolerance | Size |
|------|-----------------------------------------------|-----------|------|
| simpleSpectrum | `simplespectrum.txt` | 1e-10 | 6 values |
| Zmumu | `ee_mumu_tot.txt`, `ee_mumu_AFB.txt`, `ee_mumu_LR.txt` | 1e-5 | 1461, 1301, 72 rows |
| gg_to_tt | `reinterpretation.txt` | 1e-5 | 3496 rows, 4 columns |
| EWGauge | `EWgauge.txt` | 1e-3 | 46 rows, 5 columns |
| testModelWidth | `modelwidthlib.txt` | 1e-5 | 1 row, 12 columns |

15. **The C++ programs** (investigator report):
    - `gettingStarted`: one `addFunction` (see fact 6), `build()`, no evaluation, no reference.
    - `simpleSpectrum`: sets `g_D, lSH, lSS, vph, MPsi, mhH2`, calls `updateSpectrum`, writes six masses. Its test
      ([simpleSpectrum.cpp:110-124](../../../../../tests/system/src/simpleSpectrum.cpp)) uses `system()` to remove,
      copy and run. The Python port must not do this.
    - `Zmumu`: three functions, full energy and angle grids, writes `data.txt`.
    - `gg_to_tt`: three functions (`M2` and two ghosts), energy scan 400 to 5000 step 25, `theta` scan step pi/20.
    - `EWGauge`: **enables quadruple precision** ([EWGauge.cpp:36](../../../../../tests/system/src/EWGauge.cpp),
      `Library::setQuadruplePrecision(true)`, verified), four functions, 100-point angular integration per energy.
    - `testModelWidth`: default parameters, evaluates the whole width collection.
16. **Build and tests.** `simpleSpectrum`, `testModelWidth`, `Zmumu`, `gg_to_tt` and `EWGauge` are `system` tests
    ([tests/system/CMakeLists.txt](../../../../../tests/system/CMakeLists.txt), investigator report).

## Plan conflicts found

- **EWGauge and quadruple precision.** The phase 6 port table asks for `EWGauge` in phase 6, while the plan puts the
  "quad variant in Phase 7". The C++ test uses quadruple precision (fact 15), so the phase 6 port needs quad support,
  or a double-precision reproduction that cannot match the quad reference at 1e-3. Decision needed (Q3).
- **WP6.1 `marty_capi.cpp`.** Largely duplicates the existing C layer (fact 1). The phase needs metadata and a loader,
  not a second C API. Decision needed (Q1).
- **Numerical precision.** Generated code uses `double` (investigator report, `librarygenerator.cpp:62-68`), while CSL
  scalars use `long double` ([src/csl/numerical.h](../../../../../src/csl/numerical.h)). Whether 1e-12 is reachable
  with `double` is not verified; measured in the spike.

## Options and trade-offs

- **Binding route.** (a) ctypes on the existing `extern "C"` layer, with metadata emitted by the generator and a
  generic importer in Python. Simplest: no compiled Python module per library and no Python headers in the user's build.
  Cost: the parameter struct layout is mirrored in Python. (b) A pybind11 module generated per library (`import toy`
  native). Cost: pybind11 and Python development headers needed for every library build. (c) The plan's `dlopen` loader
  inside the `_pymarty` extension. Most logic in C++, one place for in-process safety checks; more C++ code.
- **Forced C++ functions** (`forceCppFunc`, trivial functions) have no C symbol. Either the generator emits C wrappers
  for them too (must keep output of existing targets unchanged), or the binding excludes them.
- **Fallback backend** (out-of-process) keeps the plan's option open. Costs a second execution path.

## Not verified

- Exact generated `param_t` / `cparam_t` declaration and field types.
- Whether generated `.so` files load and run together with `_pymarty` without symbol clashes (WP6.0 item 3 spike).
- Whether GSL is needed for every generated library.
- Baseline test status (the baseline build is still running in `/tmp/pymarty-build`, GCC 11 instead of GCC 12).

## Baseline (2026-11-24)

- Build: `/tmp/pymarty-build`, Release, GCC 11.4.0 (GCC 12 not installed), CMake 3.22.1, `-DMARTY_BUILD_PYTHON=ON`. Build exit 0.
- `ctest -L 'units|integration'`: 203 of 203 passed (units 156, integration 47 labels, about 3 s wall time).
