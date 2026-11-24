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
- EWGauge: excluded from phase 6 (quadruple precision needed, phase 7).
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
4. EWGauge: out of scope (phase 7).
5. Existing `ctest -L 'units|integration'` and `api` pass. `python` tests pass with `-m "not slow"`.
6. No change to generated C++/C output of existing libraries except the new metadata file.

## 6. Open risks (checked in the spike, WP6.0)

- In-process load of generated `.so` with LoopTools (process-wide cache, `dep/LoopTools-2.16/src/util/cache.c:22-56`)
  and GSL. If the spike fails, stop and report; do not silently add the fallback.
- Whether `double` precision reaches the reference tolerances (the 1e-10 for simpleSpectrum may be tight).

## 7. Out of scope

Quad precision (EWGauge), MARTY plugins, GRAFED, Fortran/LoopTools tuning, packaging/wheels.

## 8. Open items for G1

- `simulation.generate(model=...)`: decided in review. The argument is a `pymarty.modeling.Model` instance (dependency on
  `pymarty.modeling` is allowed).
- Function and parameter names in the draft (`mass_H`, `mass_A`, `mhH2`, `vph`) are illustrative and must match the
  simpleSpectrum model.
- Import without `lib.import(...)`: `from toy import ...` works after `build()`, and linters see the symbols through the
  generated `toy.pyi`. Confirm this is acceptable.
- Undefined parameters: decided in review, see section 3b (explicit-parameter C functions). Remaining point: the names
  of the explicit functions (`c_<name>_explicit` proposed, keeping the `c_` prefix of the existing layer).
- Spectrum libraries: `updateSpectrum` takes the struct and sets parameters. It does not fit the explicit form. Its
  binding (setting parameters from Python) needs its own design. Out of scope for the first milestone unless required by
  simpleSpectrum; to decide.

## Gate G1

Waiting for the user's approval of this spec before any implementation.
