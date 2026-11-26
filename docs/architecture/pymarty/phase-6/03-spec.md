Status: READY
Updated: 2026-11-24
Inputs: [01-investigation-summary.md](./01-investigation-summary.md), [02-questions.md](./02-questions.md)

# Phase 6 spec: simulation library generation, build and Python binding

## 1. Goal

From one Python script: generate a MARTY library, compile it, load it, and call its functions to fill plot data.

```python
from pymarty import simulation

lib = simulation.generate(model=..., path="work/toy", name="toy")   # generation + build
from toy import X, Y        # generic importer finds the compiled library
values = X(params, ...)     # calls the generated C function
```

## 2. Decisions (from Q1 to Q4)

- Binding: ctypes over the existing `extern "C"` layer that the generator already emits (`clib/`, `c_<fn>.{h,c}`,
  `ccommon.h`, `cparams.h`). No new C API is written in C++ (plan WP6.1 `marty_capi.cpp` is dropped, see
  investigation fact 3).
- Generator change (small): emit a metadata file next to the `.so` (function names, parameter names and types with
  defaults, result kind: real or complex). Needed so Python knows the signatures. Output of existing libraries must not
  change, except the added metadata file.
- Ports: full scans (Zmumu, gg_to_tt, EWGauge, testModelWidth) reproduce the C++ test reference files. Marked `slow`.
- EWGauge: included in phase 6. Its quadruple precision is internal to the C implementation (section 9), so the C
  interface and Python use double (Amendment 2).
- No out-of-process fallback in phase 6.

## 3. Components

| Item | Location | Content |
|------|----------|---------|
| Generator metadata | `src/csl/librarygenerator.{h,cpp}` | Writes `<name>.meta.json` (functions, result kind, and for each function the parameters it uses, with types and defaults). Also writes `<name>.pyi` stubs so linters see the symbols. |
| Simulation API (C++) | `src/marty/api/simulation/` | Generate, build (make), return the library path. Typed errors. No pybind headers. |
| pybind binding | `python/src/simulation.cpp`, `bindSimulation(py::module_&)` | Thin wrapper over the C++ API. Registered in `python/src/module.cpp`. |
| Python package | `python/pymarty/simulation/__init__.py` (+ `.pyi`) | `generate()` returns a `Library` object. |
| Library object | `python/pymarty/simulation/library.py` | `Library.build()` compiles, then adds the library directory to `sys.path` and installs the importer. User scripts contain no `sys.path` code. Minimal interface: `name`, `path`, `build()`, `functions`. |
| Parameter binding | `python/pymarty/simulation/functions.py` | Builds the Python callables from metadata: required and optional keyword arguments, `ValueError` for unset parameters (see section 3b). |
| Generic importer | `python/pymarty/simulation/importer.py` | `sys.meta_path` finder installed by `build()`: `import toy` finds `<path>/toy/lib/toy.so` + `toy.meta.json`. |
| ctypes loader | `python/pymarty/simulation/loader.py` | Loads the `.so` with ctypes, builds Python callables from metadata. |
| Usage example | [simulation.py](/workspaces/marty-public/python/examples/usage/simulation.py) | Draft of the single script. Shows the target API; does not run until implemented. |
| Tests | `python/tests/simulation/` (pytest, `needs_compiler`, `slow`) | Ports of the five tests (see section 5). |

Python errors: `SimulationError`, `BuildError`, `LoadError` in `pymarty.errors`. A missing parameter raises the built-in `ValueError` (no new error class).

### 3b. Undefined parameters (review requirement)

The C++ layer refuses to use a parameter that was never set (`InitSanitizer`, [initSanitizer.h](/workspaces/marty-public/src/csl/initSanitizer.h)).
The C functions take the whole parameter struct, so a C function with only the struct cannot tell an unset field from a
set one. The generator is changed as follows (agreed in review):

- For each function, the generator emits an explicit-parameter C function `c_<name>_explicit(double pA, ...)` (complex
  parameters as `ccomplex_t`). Its body is the current body. Its parameter list is the function's used parameters, in the
  order given by `LibFunction::parameters` (set through `setParameters`, `src/csl/libraryfunction.cpp:165`).
- The existing struct-based `c_<name>(const cparam_t*)` stays and only forwards its fields to `c_<name>_explicit`. The
  C++ wrapper and the existing C++ and C users are not changed.
- Python binds the `_explicit` functions with the suffix dropped. Each binding knows its parameters from the metadata.
  Each function accepts two forms, both checked the same way:
  1. Keyword arguments: `mass_H(vph=246.0, mhH2=125.0)`. Parameters with a model default are optional; the others are
     required.
  2. A params object: `mass_H(params)`, where `params` is a dataclass generated per library (`<name>.params`, see below).
     Its fields are all the library's parameters, default `None` (unset). The function checks that the fields it uses are
     not `None`.
- The library dataclass has one field per library parameter, so the same object is filled once and passed to every
  function. Parameters a function does not use may stay `None`. A missing required value raises `ValueError` naming the
  parameter and the function (before C is called).
- Decided: one library-wide params dataclass (one object for all functions).
- No runtime flag in C is needed: the check is the call signature itself, so it cannot be bypassed from Python.
- The C++ `InitSanitizer` check still happens when the C++ wrapper copies a used parameter, so C++ behaviour is kept.

### 3c. Spectrum libraries (review decision, scope B)

- The diagonalization core (GSL hermitian eigendecomposition, symmetrization, mass sorting, bidiagonalization and sign
  fix) is ported to C in `clib/cdiagonalization.{h,c}`. The C++ `Diagonalizer` runtime (`libdiagonalization.cpp`) keeps
  its public API and calls these C functions, so C++ spectrum utilities wrap the C code like every other library function.
- Mass-matrix expressions are still built in C++ (they are C++ strings from upstream). Only the numerical core moved.
- A spectrum model exposes its masses through a C function that takes the explicit spectrum inputs (the parameters
  `updateSpectrum` reads) and returns the computed masses. Its `_explicit` form and binding follow section 3b, so
  Python calls it with keyword arguments and gets the masses back.
- The struct form `c_<name>(const cparam_t*)` keeps its current behaviour.
- Checked: simpleSpectrum (`simple-spectrum-generation`) passes at 1e-10 with the C diagonalizer.


## 4. Process rules (repository)

- Build only in `/tmp/pymarty-build`. Baseline uses GCC 11 (GCC 12 not installed; AGENTS.md allows 11+).
- Test commands: `ctest -L 'units|integration'`, `ctest -L api`, `ctest -L python`. For the ports, run single named
  tests with `ctest -R <name>` only when needed. Never `ctest -L system` and never unfiltered ctest.
- Pytest only with `-m "not slow"` for the fast set. The slow ports run separately with `-m slow`.
- No `system()` or `popen()` in new code. The C++ tests use `system()`; the Python port must not copy that.

## 5. Acceptance criteria

1. `simulation.generate` + build + `import toy` works in one script on a clean directory.
2. simpleSpectrum: six masses match `simplespectrum.txt` at 1e-10 (relative, as in `comparedata.h:30`).
3. Zmumu, gg_to_tt, testModelWidth: full scans match their reference files at their tolerances (1e-5).
4. EWGauge: runs in double precision through the C interface and matches `EWgauge.txt` at 1e-3.
5. Existing `ctest -L 'units|integration'` and `api` pass. `python` tests pass with `-m "not slow"`.
6. No change to generated C++/C output of existing libraries except the new metadata file.

## 6. Open risks (checked in the spike, WP6.0)

- In-process load of generated `.so` with LoopTools (process-wide cache, `dep/LoopTools-2.16/src/util/cache.c:22-56`)
  and GSL. If the spike fails, stop and report; do not silently add the fallback.
- Whether `double` precision reaches the reference tolerances (the 1e-10 for simpleSpectrum may be tight).

## 7. Out of scope

Quad precision in the public interfaces (section 9 keeps it internal to C), the quad test variant of EWGauge
(phase 7), MARTY plugins, GRAFED, Fortran/LoopTools tuning, packaging/wheels.

## 8. Open items for G1

- `simulation.generate(model=...)`: decided in review. The argument is a `pymarty.modeling.Model` instance (dependency on
  `pymarty.modeling` is allowed).
- Function and parameter names in the draft (`mass_H`, `mass_A`, `mhH2`, `vph`) are illustrative and must match the
  simpleSpectrum model.
- Import without `lib.import(...)`: `from toy import ...` works after `build()`, and linters see the symbols through the
  generated `toy.pyi`. Confirm this is acceptable.
- Undefined parameters: decided in review, see section 3b (explicit-parameter C functions). Remaining point: the names
  of the explicit functions (`c_<name>_explicit` proposed, keeping the `c_` prefix of the existing layer).
- Spectrum libraries: decided in review (scope B). The spectrum utilities (`updateSpectrum`, `updateDiagonalization`)
  stay C++ and are wrapped by the C layer, like every other library function. See section 3c.

## 9. Breaking change: quadruple precision only in the C implementation

Quadruple precision is removed from every public interface: the C, C++ and Python APIs and the parameter structs
are double-only. It stays as an option of the C implementation of generated libraries
(instance method `setQuadruplePrecision(bool)` of `LibraryGenerator`, and of `mty::Library` which also switches the
LoopTools link flags): the C calculation files use `__float128` and `__complex128` internally and return double
values. Integer and real literals in the quad C code carry the `Q` suffix. Real `abs` prints `fabs`/`fabsq`. EWGauge needs this option: its C library is built with quadruple precision
internally and returns double values to the C interface and to Python.

For MARTY consumers, this is a breaking change for C++ code that used quadruple types or
`setQuadruplePrecision` on the C++ path. Its effect is:
- Generated C++ headers and bodies are double, and the C++ layer no longer carries quad types.
- The internal headers `libcomplexop.h` and `libdiagonalization.h` (dead copies with quad types) are removed. The
  generator writes headers from the `*_hdata.h` sources.
- `setQuadruplePrecision(true)` still needs a build with `MARTY_QUADRUPLE_ENABLED`.
- The global `setQuadruplePrecision` call is replaced by the library instance method
  (`lib.setQuadruplePrecision(true)`), called after the library is constructed. Examples and tests are migrated.

This entry is for the next release CHANGELOG. There is no CHANGELOG file in the repository yet.

Test coverage: `tests/integration/src/test_library_targets.cpp` (C, C++, Python and C++&Python targets) checks the
quad result in double output, and checks that public headers contain no quad types.

## Gate G1

Approved by the user, with the amendments below.

## Amendment 1 (2026-11-26, approved by gregoire-uhlrich_SAGCP)

What changed:
- No metadata file. `<name>.meta.json`, the generic importer, the `functions.py` parameter binding and `loader.py`
  are dropped (sections 2, 3 and the file table). The generated `src/py/<name>.py` ctypes binding already gives the
  function names, the parameters, the complex result and the library location, so `pymarty.simulation` loads it with
  `importlib` instead.
- The generator is not changed. `pymarty.simulation` uses the existing `LibraryGenerator` through
  `mty::api::simulation::Generator` (C++ API, `src/marty/api/simulation/`) and the pybind submodule
  `pymarty._pymarty.simulation.Generator`. Both always write the Python binding and no C++ interface.
- Input is a mapping of function names to `pymarty.symbolic` expressions, not a `Model`: `generate(name,
  functions, path=..., include_path=...)`. Compilation is `Library.build(n_jobs)` (`make` from
  `pymarty.config`), and `Library.load()` returns the generated binding module.
- Missing parameters raise the binding's built-in `ValueError`, as generated. Build failures raise
  `MartyBuildError`, invalid names `MartyValueError`, and `load()` before `build()` `MartyStateError`.
- The usage example uses a small symbolic expression. A model-derived example needs a design decision (see
  the open items below).

Open items (not in this amendment): the simpleSpectrum, Zmumu, gg_to_tt and testModelWidth ports; a model-based
library input; `.pyi` stubs for `pymarty.simulation`. Amendment 2 covers the ports and the work plan.

## Amendment 2 (2026-11-26, approved by gregoire-uhlrich_SAGCP)

What changed:
- EWGauge is in phase 6. The quadruple precision issue is solved: quad is internal to the C implementation
  (section 9), and the C interface and Python use double. This supersedes section 2 (EWGauge bullet), section 5
  criterion 4, section 7, and the Q3 decision in [02-questions.md](./02-questions.md) (defer to phase 7).
- The quad test variant of EWGauge stays in phase 7 (phase 7, section 7e).
- Squared amplitudes (scalar `Expr`) are the library input, as agreed in the review. A model-based input is not
  part of phase 6.
- Every system test in `python/tests/system/` gets library generation, build, load and numerical comparison.

Acceptance (replaces section 5 for phase 6):
1. `simulation.generate` + build + load works in one script on a clean directory.
2. simpleSpectrum: six masses match `simplespectrum.txt` at 1e-10 (relative, `comparedata.h:30`).
3. Zmumu, gg_to_tt, testModelWidth, EWGauge: full scans match their reference files at their tolerances.
4. The nine system tests generate, build, load and compare against their references.
5. `ctest -L 'units|integration'` and `api` pass. Python tests pass with `-m "not slow"`. The slow set runs with
   `-m slow`.
6. No change to generated C/C++ output of existing libraries, apart from the quad-internal option.

Work plan (20 items, in waves; "after" lists the real prerequisites):

| ID | Item | After | Wave | Track |
|----|------|-------|------|-------|
| A | `pymarty.testing.compare` (port of `comparedata.h`) and scalar squared-amplitude input test | none | 1 | Python |
| B | Spectrum binding: `generateSpectrum`, `replace`, `renameParticle`, `promoteToGoldstone`, through the simulation API | none | 1 | C++/pybind |
| C | Width binding: `computeModelWidths` and the widths library build | B | 1 | C++/pybind |
| Q | Quad-internal C option exposed by the simulation generator (needs `MARTY_QUADRUPLE_ENABLED`) | C | 1 | C++/pybind |
| D1 | Zmumu port, with the squared-amplitude library test against `ee_mumu_*.txt` | A | 1 | Python |
| D2 | gg_to_tt port against `reinterpretation.txt` | A | 1 | Python |
| E | simpleSpectrum port against `simplespectrum.txt` | A, B | 2 | Python |
| F | testModelWidth port against `modelwidthlib.txt` | A, C | 2 | Python |
| G | EWGauge numeric run against `EWgauge.txt` (double interface, quad inside C) | A, Q | 2 | Python |
| H1-H9 | Generation, build, load and comparison in each of the eight remaining system tests (`test_c7_sm`, `test_c8_sm`, `test_conjugation`, `test_ewgauge`, `test_nlepto`, `test_sm_2idm`, `test_sm_triplet`, `test_widths_sm`). `test_majorana_loop` was moved to the C++ integration suite (see H5) | A | 2 | Python |
| I | Docs: `unported-features.md`, `README.md`, phase 6 port table, status notes | all | 3 | docs |
| J | Verification: fast Python suite, `slow` set, `ctest -R` for the touched targets, `/tmp` cleanup | all | 3 | verification |

Parallelism: wave 1 has four tracks. The C++/pybind items (B, C, Q) run in one track, because they share the
`/tmp/pymarty-build` build. The Python items (A, D1, D2) run in parallel with that track. Wave 2 runs up to eight
Python items in parallel (E, F, G, H1-H9 are separate files), limited by the shared build for the runs. Wave 3 is
sequential.

Outcome of the work plan items (2026-11-26):

| ID | Outcome |
|----|---------|
| E | Done. simpleSpectrum matches `simplespectrum.txt` at 1e-10 (relative); achieved ~3e-16. |
| F | Done. testModelWidth: all 12 widths match `modelwidthlib.txt` at 1e-5; achieved 2.2e-6. |
| G | Done. EWGauge matches `EWgauge.txt` at 1e-3; achieved 8.07e-4. The quad path is verified; quad is internal to C, and the C interface and Python use double. |
| H1 | `test_c7_sm`: done. The m_t scan matches the C++ reference at 1e-5. Root cause of the earlier zeros: the Python list overload of `getWilsonCoefficient` (`src/marty/core/wilsonOperator.cpp`) rebuilt each `Wilson` from its expression and lost the magnetic prefactors. Fixed with `chromo_magnetic_coefficient`, which passes the native `Wilson` objects. The test uses the default topology (24 amplitudes, as in C++). |
| H2 | `test_c8_sm`: done, same fix as H1, matches at 1e-5. |
| H3 | `test_conjugation`: moved to the C++ integration suite (`tests/integration/src/test_conjugation.cpp`) and removed from the Python suite. It checks relative signs, not numbers, so it needs no Python counterpart. The Python model `conjugation` is deleted. |
| H4 | `test_ewgauge`: done for the double-precision scan (item G). The quad test variant stays in phase 7. |
| H5 | `test_majorana_loop`: removed from the Python suite. The test checks amplitude simplification, not numbers. It is now a C++ integration test (`tests/integration/src/test_majorana_loop.cpp`), and the Python model `majorana_loop` is deleted. |
| H6 | `test_nlepto`: moved to the C++ integration suite (`tests/integration/src/test_nlepto.cpp`) and removed from the Python suite. It builds the library and checks the binary, with no numbers. The Python model `nlepto` is deleted. |
| H7 | `test_sm_2idm`: tree level checked. One-loop: build and load only, matching the C++ scope (decided by the user). No one-loop numerical reference. |
| H8 | `test_sm_triplet`: tree level checked. One-loop: build and load only, matching the C++ scope (decided by the user). No one-loop numerical reference. |
| H9 | `test_widths_sm`: done. All 8 widths match the reference at 1e-5; achieved 1.06e-6. |

Also changed: complex-parameter functions are exposed to Python (`src/csl/librarygenerator_python.cpp`,
`isComplexParameter`, `_Complex` argtypes).

Decided after phase 6: the one-loop tests of sm_2idm and sm_triplet keep the C++ scope (build and load, no one-loop
numerical reference). The `majorana_loop`, `conjugation` and `nlepto` tests check amplitude simplification or build
success, not numbers, so they are C++ integration tests (`tests/integration/`) and no longer Python tests.
