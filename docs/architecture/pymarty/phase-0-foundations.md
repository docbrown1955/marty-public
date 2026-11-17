# Phase 0: foundations, hardening and spikes

Nothing user-visible is delivered. The phase makes everything after it safe: build and packaging
skeleton, typed errors, state guard, test harness, and the technical spikes whose answers shape
later phases.

Prerequisites: none. See [api-conventions.md](api-conventions.md) and
[testing-strategy.md](testing-strategy.md).

## 1. Scope

In scope:
- `python/` layout, CMake option `MARTY_BUILD_PYTHON`, pybind11 through `FetchContent`.
- `src/csl/api/` and `src/marty/api/common/` (errors, `Rational`, `Session`, `Config`).
- Core hardening (decision D3): `CallHEPError`, `exit`/`abort`, `Model::current`, option flags.
- Test harness: `tests/api/`, `ADD_API_TEST`, pytest setup, parity checker, fixtures, CI jobs.
- Minimal wheel built and tested in CI.
- Spikes A to F (section 4), each ending with a written decision record.

Out of scope: any feature module (symbolic, ...). The only Python API is `pymarty.__version__`,
`pymarty.config`, the error classes and two test-only helpers under `pymarty._debug`.

## 2. Work packages

### WP0.1 Build skeleton
- `python/CMakeLists.txt` creating the target `_pymarty` (pybind11 module) linked to `marty-static`
  (spike A confirms position independence). Option `MARTY_BUILD_PYTHON`, default OFF so existing
  builds are unchanged.
- `pybind11` fetched with `FetchContent` and pinned (like gtest in `tests/CMakeLists.txt`).
- `tests/api/CMakeLists.txt` with `ADD_API_TEST(NAME)` modelled on `ADD_INTEGRATION_TEST`
  (`SETUP_MARTY_TEST_TARGET` + `SETUP_GTEST_TARGET(<target> api)`), included from
  `tests/CMakeLists.txt`; the coverage targets get the `api` label.
- Make the configure step usable on a wheel build machine: the top-level `CMakeLists.txt` makes
  `lualatex`, `latex`, `convert` and `dvipng` required, and Qt 5 is needed for `grafed-gui`.
  Add options (default preserving today's behaviour) to skip the GUI target and the LaTeX tool checks
  when only the libraries are built. Check the remote branch `making_lualatex_optional` first.
  `marty` links `grafed-core` and `jsonparser`, which must stay buildable without Qt GUI code.
- `python/pyproject.toml` (scikit-build-core), cibuildwheel configuration (manylinux, Linux x86_64).

### WP0.2 Typed errors
- Add `csl::api::Error` and `mty::api::Error` hierarchy (see conventions section 1.6).
- Change `CallHEPError` / `HEPAssert` in `src/marty/core/mrtError.h`: throw a class
  `mty::error::Exception : std::runtime_error` carrying the enum value, the function, file, line and
  the message. The stderr print stays by default (no behaviour change for C++ users) behind a flag
  `mty::option::printErrors` that the `Session` turns off.
- Audit every `catch` of `mty::error::Type` by value (a thrown class is not caught by a handler for
  the enum). Provide a compatibility strategy: catch sites updated, and a test that existing error
  paths still end in a catchable `mty::error::Exception` with the same enum value.
- Audit CSL's own error path (`csl::option::errorStopsProgram` suggests terminating behaviour) and
  make the API path throw instead.
- Translate `GammaAPIError` and standard exceptions at the API boundary.

### WP0.3 Terminating calls
- List the `exit()`/`abort()` calls found by `grep -rn "\bexit(\|abort()" src/csl src/marty` (12 outside
  `src/marty/ufo` at the time of writing; re-count, check which are real calls and not comments).
- For each: replace by a thrown `mty::api::Error` subclass when reachable from the API; otherwise
  record why it stays. The rest of the code behaves exactly as before.

### WP0.4 State: `Session`
- Implement the guard described in conventions section 3, including the X-macro (or equivalent) so
  the option list used by the snapshot cannot drift from the headers.
- `Model::current` handling: a scoped setter/restorer. Review the 36 uses (`grep -rn "Model::current"`)
  and the branch `separating_global` (merged with master, content to be read) for intended direction.
- Gamma API 4D flag (`keepSymbolic4D`) inside the snapshot.

### WP0.5 Output
- Redirect of `std::cout`/`std::cerr` into Python streams for legacy printing (pybind11
  `scoped_ostream_redirect`) as a binding helper used by later phases.

### WP0.6 Packaging and run-time `Config`
- `mty::api::Config`: finds include dir, lib dir, compiler, `make`, LoopTools, GRAFED at run time:
  explicit argument, then environment (`MARTY_INCLUDE_PATH`, `MARTY_LIBRARY_PATH`, `MARTY_CXX`/`CXX`,
  `MARTY_GRAFED`), then package data (`pymarty/_data/{include,lib}`), then system paths. Replaces use of the
  compile-time `MARTY_CXX`, `MARTY_INSTALL_PATH` definitions in API code (they stay for the C++ library).
- Wheel contents: `_pymarty`, `libmarty`/`libcsl` if shared, `libooptools` (LoopTools), headers needed to
  compile generated libraries, license files. `auditwheel repair` bundles `libgfortran` and GSL.
- `libooptools.so` is referenced through `${PROJECT_BINARY_DIR}/libooptools.so` in `src/marty/CMakeLists.txt`:
  the wheel needs a relocatable reference (`$ORIGIN` rpath).

### WP0.7 Test harness
- `python/pyproject.toml` pytest section with `--strict-markers`, marker list, `clean_state` fixture.
- `python/tools/check_parity.py` and the two text files (parity sources, exemptions).
- `pymarty._debug` test hooks: `raise_error(kind, message)`, `live_object_count()`.
- CI: add jobs to `.github/workflows/c-cpp.yml` for (a) Python build + L1/L2/L3 on Ubuntu 24.04 and
  22.04, (b) sanitizers, (c) wheel build and test, (d) nightly slow jobs.

## 3. C++ API deliverables

| Item | Location |
|------|----------|
| `csl::api::Error` and subclasses | `src/csl/api/error.h` |
| `mty::api::Error` subclasses, `BuildError` | `src/marty/api/common/error.h` |
| `Rational` (`int64`/`int64`, normalised, exact conversion from `csl::Expr` or error) | `src/marty/api/common/rational.h` |
| `Session` | `src/marty/api/common/session.h` |
| `Config` | `src/marty/api/common/config.h` |
| `mty::error::Exception` | `src/marty/core/mrtError.h` |

## 4. Spikes (time-boxed investigations that end in a decision record)

Each spike writes a short record in this folder (`spike-<letter>-<topic>.md`) with the question, the
experiment, the result and the decision. Acceptance of the phase requires all six.

| Spike | Question | Output |
|-------|----------|--------|
| A | Is `marty-static` position independent so that it can be linked into a Python extension? Do `csl-core`, `grafed-core`, `jsonparser`, `marty-lha-obj`, `sgl`, `marty-models` (all object libraries) carry `POSITION_INDEPENDENT_CODE`? Does `import` of a minimal `_pymarty` work with the Fortran LoopTools library linked? | Fix list, and a working minimal module |
| B | Can two `Model` objects coexist and be used in sequence and interleaved in one process when guarded by `Session`? Which core statics besides `Model::current` break this (registries, abbreviation cache `csl::Abbrev`, allocators)? | Pass/fail matrix and, if needed, extra state to snapshot |
| C | What happens on error today: which exceptions escape the core in practice (enum, `std::exception`, GammaAPIError), and which paths terminate? | Complete error-path table used by WP0.2/0.3 |
| D | Wheel feasibility: can the manylinux image build LoopTools (Fortran), GSL linking, bundle `libgfortran` with `auditwheel`, and the installed wheel import in a clean venv without compilers? | Dockerfile/CI snippet, size of the wheel |
| E | Generated-library build from the wheel: can a compiler on the user machine compile a trivial generated library against headers and libraries from the wheel (ABI, `libstdc++` version, rpath)? (Only the compile-and-link question, the loader is Phase 6.) | Go/no-go and the required flags |
| F | Do `std::cout` redirect and output of long C++ computations behave well in Jupyter? | Binding helper recipe |

## 5. Test plan

L1 (C++ API tests, `tests/api/common/`):
- `ErrorApi.HierarchyDerivesFromStdRuntimeError`
- `ErrorApi.CoreEnumErrorIsTranslatedWithMessage` (a core `CallHEPError` becomes `mty::api::Error` with the
  function name and message)
- `ErrorApi.GammaApiErrorIsTranslated`
- `ErrorApi.NothingIsPrintedToStderrInQuietMode`
- `ErrorApi.LegacyThrowStillCatchableAsCoreException` (compatibility, existing handlers)
- `SessionApi.RestoresCurrentModelOnExit` and on exception
- `SessionApi.RestoresEveryOptionFlag`
- `SessionApi.OptionListCoversAllFlagsInHeaders`
- `SessionApi.RestoresGammaSymbolicFlag`
- `SessionApi.TwoModelsSequentialAndInterleaved` (from spike B)
- `RationalApi.Normalisation`, `.ExactFromCslExpr`, `.NonRationalFails`, `.OverflowFails`
- `ConfigApi.EnvironmentOverridesPackageData`, `.MissingCompilerReportsBuildError`, `.NoShellInvolved`

L2 (Python):
- import of `pymarty`, `__version__` equals the C++ version string
- each error kind raised from `_debug.raise_error` has the right Python class, the message, and is
  an instance of the corresponding built-in (`ValueError`, ...)
- `clean_state` fixture catches a deliberately leaking test (meta test)
- output redirect: text printed by C++ appears in `capsys`

L3: parity checker runs, reports all of the above matched.

L0 regression: described in acceptance criteria.

## 6. Acceptance criteria

All of the following, with evidence in the PR:

1. `cmake` configures and builds with `MARTY_BUILD_PYTHON=OFF` exactly as before (same targets, no
   new required tools). With `ON`, `_pymarty` builds on GCC 14, GCC 11 and Clang 18 in CI.
2. L0 regression: `ctest -L 'units|integration'` green. The full `system` suite result (pass/fail per
   test) is identical to the baseline recorded **before** the first core change, and no file under
   `tests/system/data/output/` changed. The baseline run is attached to the PR.
3. C++ behaviour unchanged for non-API users: an existing error path prints the same stderr text as
   before (checked by a test), and ends in a catchable exception.
4. All L1/L2/L3 tests listed above exist and pass. State-neutrality tests pass under ASan + UBSan.
5. Zero `exit`/`abort` reachable from the API surface of phases 1 to 7 (list of remaining ones
   attached, each with justification) and the audit table of spike C is committed.
6. A wheel builds in the manylinux image, passes `auditwheel show`, installs in a clean virtual
   environment that has no compiler and no Fortran runtime, and the wheel test (`import pymarty`,
   `__version__`, error round trip) passes. Wheel size is recorded.
7. Spikes A to F have decision records. Any "no-go" result has a mitigation agreed before closing the
   phase.
8. Quality gates of [testing-strategy.md](testing-strategy.md) pass (format, warnings, mypy, ruff,
   coverage >= thresholds for the new files).

## 7. Exit artifacts

Working skeleton on CI, spike records, error table, baseline test results, updated `AGENTS.md` pointer
to this folder (small addition to the documentation map), updated `docs/architecture/build-and-tests.md`
with the new option, label and macro.

## 8. Risks

- Changing `CallHEPError` could alter behaviour of code that catches the enum. Mitigation: audit, tests
  of legacy catches, baseline comparison.
- Option snapshot may miss flags living outside `mrtOptions.h` and `csl/options.h`. Mitigation: grep for
  other global mutable state (`inline static`, namespace-scope `bool`) and add to the audit table.
- manylinux toolchain may lack features needed by C++23 code. Mitigation: spike D decides the image and
  compiler (`gcc-toolset`), or the minimum supported GCC for wheels.
