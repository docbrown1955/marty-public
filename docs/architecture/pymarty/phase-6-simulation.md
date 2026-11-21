# Phase 6: `pymarty.simulation`

Generate C/C++ libraries from symbolic expressions, build them, load them into the Python process and
evaluate them numerically. Today MARTY stops after writing and compiling the library: the user writes
a `main` that includes the generated header (see `tests/system/libsrc/example_*.cpp`, where the
generated namespace exposes a `param_t` struct and functions returning complex values) and runs the
executable. `csl::LibraryGenerator` and `mty::Library` (`src/csl/librarygenerator.h`,
`src/marty/core/mtylibrary.h`) have no run-time loader. This phase adds one.

Prerequisites: Phase 5 accepted (amplitudes to feed the generator). The generator part itself only
needs Phase 1.

Usage example gate: before any C++ API or binding work starts, `python/examples/usage/simulation.py` is
written and approved by a human reviewer ([testing-strategy.md](testing-strategy.md) section 1.2). It fixes
the Python API of this phase (generate, build, load, evaluate) and is the first deliverable. It may be
drafted earlier.

## 1. Scope

In scope:
- Library description and generation: `Library(name, path)`, `add_function(name, expr, ...)`,
  `generate_spectrum(model)`, include/library paths, dependencies.
- Building: compiler discovery, build via argument vectors, error reporting with the compiler output.
- Loading: a generated library is compiled as (or additionally as) a shared library with a small C ABI
  and metadata, loaded with `dlopen`, and exposed in Python with named parameters and callable
  functions returning `complex`.
- Parameter handling: defaults from the library, setting and reading by name, bulk evaluation over
  sequences of parameter sets.
- Kinematics helpers ported from `tests/system/libsrc/kinematics.h` into the C++ API (2 to 2 body
  kinematics, Lorentz invariants, `pSquared`).
- A fallback backend that runs the generated example program out of process (decided by spike 6.0).
- Reproduction of `Zmumu`, `gg_to_tt`, `EWGauge`, `simpleSpectrum`, `testModelWidth` and the full
  `gettingStarted`.

Out of scope: LoopTools-dependent libraries (Phase 7), quadruple-precision libraries (Phase 7), LHA
import (Phase 7), vector/tensor-valued library outputs beyond what the listed examples need
(`NotImplementedError` otherwise).

## 2. Work packages

- WP6.0 Spike (go/no-go decisions, recorded):
  1. How are the generated files organised (`libs/<name>/{src,include,lib,bin,script}`, `Makefile`),
     what does the generated function signature look like (`param_t&`, return type), what does the
     generated code depend on (CSL runtime headers, LoopTools headers), and which parts depend on
     compile-time `MARTY_*` definitions.
  2. Can the generated sources also be built as a shared library (`-fPIC -shared`, the Makefile
     generator in `src/csl/librarygenerator.cpp` has commented out CMake `.so` lines) without breaking
     the existing static build?
  3. In-process loading safety: a generated library linking LoopTools or CSL runtime pieces that
     also exist in the Python process. Symbol visibility, `RTLD_LOCAL`, duplicate global state.
  4. Out-of-process fallback: run the generated example binary with parameter file and read results.
     Cost per evaluation.
  Decision: default backend (proposal: in-process `dlopen`), and the supported fallback.
- WP6.1 Generated metadata and C ABI: the generator additionally emits `marty_capi.cpp` with
  `extern "C"` functions: `marty_describe()` (JSON: parameters with default values, functions with their
  signatures and result kinds), `marty_params_new()`, `marty_params_set(h, idx, re, im)`,
  `marty_params_get`, `marty_eval(h, fn_idx, out_re, out_im)`, `marty_params_free`. The existing
  generated API and Makefile targets stay as they are (additive change). ABI version number embedded.
- WP6.2 C++ API `src/marty/api/simulation/`: `LibraryBuilder` (wraps `mty::Library` with explicit
  methods: no `forceCppFunc` string tricks, no `defineLibPath` macros, uses `Config`), `BuildResult`
  (success, log, artefact paths), `LoadedLibrary` (RAII `dlopen`/`dlclose`, introspection,
  `evaluate(function, parameters)`, `evaluate_many`), `Kinematics`.
- WP6.3 Name and path validation, build executed with `posix_spawn` of `make`/compiler (no shell).
- WP6.4 Bindings and Python package: `Library`, `LoadedLibrary`, `Parameters` mapping, `kinematics`.
- WP6.5 Ports: `gettingStarted` (full), `Zmumu`, `gg_to_tt`, `EWGauge`, `simpleSpectrum`,
  `testModelWidth`, with comparison against `tests/system/data/output/*`.

## 3. Python spelling

| C++ | Python |
|-----|--------|
| `mty::Library lib("toy", path);` | `lib = Library("toy", path)` |
| `lib.addFunction("squared_ampl", expr)` (third argument of the core overload is `bool forceCppFunc`; `gettingStarted` passes a string literal that converts implicitly: verify and keep out of the API) | `lib.add_function("squared_ampl", expr)` (returns the library, chainable) |
| `lib.generateSpectrum(model)` | `lib.add_spectrum(model)` |
| `defineLibPath(lib)` (test helper) | `config` (automatic, overridable by arguments and environment) |
| `lib.build()` / `lib.print()` | `lib.generate()` (write only), `lib.build(jobs=8)` returns `BuildResult` |
| (no equivalent: compile and run an example) | `loaded = lib.load()`; `loaded.functions`; `loaded.parameters`; `loaded.squared_ampl(**params)` returns `complex` |
| `Kinematics<2, 2> k(m1, m2, m3, m4, E_CM, theta)` | `kinematics.two_to_two(m1, m2, m3, m4, e_cm, theta)` |

Duck typing: parameter sets accept any `Mapping` of name to number (anything `complex`-able);
function call accepts either a mapping or keyword arguments; unknown parameter names raise
`LookupError` with suggestions; missing parameters use library defaults where defined and otherwise raise.
`Library.add_function` accepts anything coercible to a symbolic expression.

## 4. Test plan

L1 (`tests/api/simulation/`):
- Generation: `GeneratesExpectedFileLayout`, `NameValidationRejectsMetacharacters`,
  `PathValidation`, `ChainedAddFunction`, `GenerateSpectrumListsParameters`.
- Build: `BuildsTrivialLibrary` (expression `a*b + 1`), `BuildFailureReportsCompilerLog`,
  `MissingCompilerReportsBuildError`, `NoShellInvolved` (name with spaces and quotes handled as data),
  `BuildIsIdempotent`, `BuildInReadOnlyDirectoryFails`.
- Load and evaluate: `EvaluateMatchesCslEvaluate` (for 20 expressions, the library value equals CSL's own
  numeric evaluation within 1e-12 relative), `ParametersByName`, `DefaultParameters`, `ComplexResult`,
  `EvaluateMany`, `UnknownFunctionFails`, `UseAfterCloseFails`, `TwoLibrariesLoadedTogether`,
  `ReloadAfterRebuild` (behaviour documented: new object needed).
- ABI: `AbiVersionMismatchRejected`, `MetadataDescribesAllFunctions`.
- Kinematics: `TwoToTwoInvariantsAndMomenta` (checks against closed-form relations: `s + t + u = sum of m^2`,
  `pSquared` formula), compared with the values the legacy test helper computes.
- Hardening: loading a file that is not a MARTY library fails cleanly, library path outside the build
  directory requires an explicit flag.
- Fallback backend (if kept): `OutOfProcessMatchesInProcess`.
- State: `LibraryBuildLeavesGlobalStateUntouched`.

L2 (`python/tests/simulation/`, marker `needs_compiler`): parity tests for all L1 cases plus
- duck typing of parameters (`Mapping`, kwargs, `Fraction`, `complex`, objects with `__complex__`)
- `loaded` object attributes: `dir()`, `help()` show generated functions and parameters; stubs for the
  dynamic part use `__getattr__` documentation
- lifetime: library stays loaded while any function object is alive; `close()`/context manager
- `built_library` fixture caches builds per session to keep CI time down
- behaviour when a compiler is not available (skipped locally, failure in CI jobs that require it)

L4/L5 (ports, compare against reference files with the C++ test's tolerance, read from each C++ file):

| Port | C++ source | Reference data | Marker |
|------|-----------|----------------|--------|
| `gettingStarted` full | `examples/gettingStarted.cpp` | no reference file: the generated library result compared with the direct CSL evaluation of the same expression | `needs_compiler` |
| `Zmumu` | `tests/system/src/Zmumu.cpp`, `libsrc/example_zmumu.cpp` | file in `tests/system/data/output/` named by the test | `needs_compiler`, `slow` |
| `gg_to_tt` | `gg_to_tt.cpp`, `libsrc/example_gg_to_tt.cpp` | idem | `needs_compiler` |
| `EWGauge` | `EWGauge.cpp`, `libsrc/example_ewgauge.cpp` | idem | `needs_compiler`, `slow` (and `quad` variant in Phase 7) |
| `simpleSpectrum` | `simpleSpectrum.cpp`, `libsrc/example_simplespectrum.cpp` | idem | `needs_compiler` |
| `testModelWidth` | `testModelWidth.cpp`, `libsrc/example_modelwidthlib.cpp` | idem | `needs_compiler`, `slow` |

The Python tests use `pymarty.testing.compare(reference, result, tolerance)`, a port of
`tests/system/src/comparedata.h` (returns average and maximum difference, `(-1, -1)` equivalent as an
exception on mismatch of sizes or missing file).

## 5. Acceptance criteria

1. Spike 6.0 decision record exists, with the measured cost of in-process and out-of-process
   evaluation and the chosen default.
2. Generated-library output is backwards compatible: a fast unit test checks that the generated files of a
   sample library are byte-identical before and after the change (the generator output is otherwise
   untouched). System tests that use `Library` are not a criterion; the PR lists them for an optional manual run.
3. For the ports in the table not marked `slow` (`gettingStarted`, `gg_to_tt`, `simpleSpectrum`), the Python
   results agree with the reference data within the C++ test's tolerance. For the `slow` ports (`Zmumu`,
   `EWGauge`, `testModelWidth`) the status is recorded in the port coverage table (informational). In all
   cases the Python and C++ API versions of the same scenario give results equal to 1e-12 relative when
   evaluated at the same parameters, on a reduced case if needed.
4. `EvaluateMatchesCslEvaluate` holds for the 20-expression set at 1e-12 relative.
5. No `system()`/`popen()` in new code (CI grep gate). Library names and paths are validated;
   `NoShellInvolved` and the metacharacter tests pass. Loading executes code: stated in the docstring
   of `load()` and in the module documentation.
6. Robustness: a failing build or a bad library never crashes the interpreter (tests for
   syntax errors in a generated function, missing symbols, wrong ABI version). For crashes in the
   generated numerical code itself (outside the guarantees of the project) the documentation states the
   in-process risk and the fallback backend.
7. Resource handling: `close()` unloads the library, repeated build/load/close for 50 iterations does not
   grow memory beyond a recorded bound (test with `tracemalloc` for Python objects and RSS check for the
   process).
8. Build time for the cached fixture is reported; the PR job time for this phase is within the CI budget
   agreed in Phase 0.
9. `ctest -L 'units|integration'` green (system tests are not a criterion). Parity, stubs, doctests, sanitizers (for the C++ API; the generated library
   test runs without ASan if that is impractical, recorded) and quality gates pass.
10. `python/examples/usage/simulation.py` was approved by a human before implementation started (reviewer
    and date in the PR), is unchanged since or its changes were re-approved, and runs in
    `test_usage_examples.py` (L4b).

## 6. Exit artifacts

Decision record of spike 6.0, ABI description (`docs/architecture/pymarty/simulation-abi.md`, written
at the time), stubs, examples, ports, timing table, `Config` documentation for compilers.

## 7. Risks and open questions

- In-process `dlopen` of generated code means a bug in generated code can kill the interpreter. A
  fallback backend and clear docs mitigate it.
- A wheel does not know the user's compiler: `Config` and the `BuildError` messages must be excellent
  (what is missing, how to fix it). ABI compatibility between the user compiler and the wheel's bundled
  `libstdc++`/`libgfortran` is tested in spike E of Phase 0 and again here.
- Generated library size and compile time grow quickly with amplitude complexity: only fast cases run in
  PR CI.
- The existing Makefile generator is code-generation heavy: changes there must keep byte-identical
  output for the old targets (checked by a diff test on a sample library).
