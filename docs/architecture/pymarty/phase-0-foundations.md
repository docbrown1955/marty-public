# Phase 0: foundations, hardening and spikes

Nothing user-visible is delivered. The phase makes everything after it safe: build and packaging
skeleton, typed errors, state guard, test harness, and the technical spikes whose answers shape
later phases.

Prerequisites: none. See [api-conventions.md](api-conventions.md) and
[testing-strategy.md](testing-strategy.md).

Usage example: `python/examples/usage/errors.py` shows the error module (`from pymarty.errors import
MartyError`). Like every module example it is reviewed by a human before the implementation is built on it
([testing-strategy.md](testing-strategy.md) section 1.2).

## 1. Scope

In scope:
- `python/` layout, CMake option `MARTY_BUILD_PYTHON`, pybind11 through `FetchContent`.
- `src/csl/api/` and `src/marty/api/common/` (errors, `Rational`, `Session`, `Config`).
- Core hardening (decision D3): `CallHEPError`, `exit`/`abort`, `Model::current`, option flags.
- Test harness: `tests/api/`, `ADD_API_TEST`, pytest setup, parity checker, fixtures, CI jobs.
- Wheel and distribution: deferred, see [distribution-deferred.md](distribution-deferred.md).
- Spikes A to F (section 4), each ending with a written decision record.

Out of scope: any feature module (symbolic, ...). The only Python API is `pymarty.__version__`,
`pymarty.config`, the error classes and two test-only helpers under `pymarty._debug`.

## 2. Work packages

### WP0.1 Build skeleton
- `python/CMakeLists.txt` creating the target `_pymarty` (pybind11 module) linked to `marty-static`
  (spike A confirms position independence). Option `MARTY_BUILD_PYTHON`, default OFF so existing
  builds are unchanged.
- `pybind11` pinned at >= 2.11 (use 2.11.2 unless a newer version is validated). Try `find_package(pybind11 <pin>)`
  first and fall back to `FetchContent` (like gtest in `tests/CMakeLists.txt`). Distribution packages are
  usually older (Ubuntu 22.04 ships 2.9.1): the fetch fallback is the expected path on those machines and needs
  network access at configure time (document an offline override such as `-DFETCHCONTENT_SOURCE_DIR_PYBIND11=<dir>`).
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
- Decision (accepted): `CSLError`'s `operator<<` and other diagnostic output written to `std::cout` by the
  error paths move to `std::cerr`. This is the only intended difference in C++ output; acceptance 3 is
  read accordingly (same text, error streams only).

### WP0.3 Terminating calls
- List the `exit()`/`abort()` calls found by `grep -rnE "\bexit\(|abort\(\)" src/csl src/marty` (12 grep hits
  outside `src/marty/ufo` at the time of writing, of which 10 are live calls: `librarygroup.cpp` (2),
  `librarytensor.h` (1), `looptools_patch.h` (1), `semiSimpleAlgebra.h` (6). The other two are not calls:
  an `exit(123)` inside a string emitted into generated code in `librarytensor_hdata.h`, and the `SystemExit`
  enumerator in `mrtError.h`. There is no `abort()`. Re-count before starting).
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
- Source trees given to wheel builds or spikes: `docs/.gitattributes` puts the release archives
  (`docs/code/MARTY-*.zip`) under Git LFS, so `git archive` or a clean clone without `git-lfs` either triggers
  large downloads or yields pointer files. Exclude `docs/` (for example `git archive` with a pathspec, or an
  sdist include list that omits it) in sdist and wheel builds.

### WP0.7 Test harness
- `python/pyproject.toml` pytest section with `--strict-markers`, marker list, `clean_state` fixture.
- `python/tools/check_parity.py` and the two text files (parity sources, exemptions): the checker may land at
  the end of Phase 0 or with Phase 1, but is required once two modules have tests (decide at the Phase 0 review).
- `pymarty._debug` test hook: `raise_error(kind, message)`. (`live_object_count()` is introduced in Phase 1,
  when the first handle types exist.)
- CI: add jobs to `.github/workflows/c-cpp.yml` for (a) Python build + L1/L2/L3 on Ubuntu 24.04 and
  22.04, (b) sanitizers, (c) wheel build and test, (d) a manually triggered (`workflow_dispatch`) job for the
  slow and system tests: no scheduled run, see testing-strategy.md section 1.1.

## 3. C++ API deliverables

| Item | Location |
|------|----------|
| `csl::api::Error` and subclasses | `src/csl/api/error.h` |
| `mty::api::Error` subclasses, `BuildError` | `src/marty/api/common/error.h` |
| `Rational` (`int64`/`int64`, normalised, exact conversion from `csl::Expr` or error). Deferrable to Phase 1, its first user | `src/marty/api/common/rational.h` |
| `Session` | `src/marty/api/common/session.h` |
| `Config` (needed by Phases 5 and 6; may be implemented after the wheel skeleton, but its design is fixed here) | `src/marty/api/common/config.h` |
| `mty::error::Exception` | `src/marty/core/mrtError.h` |

## 4. Spikes (time-boxed investigations that end in a decision record)

Each spike writes a short record in this folder (`spike-<letter>-<topic>.md`) with the question, the
experiment, the result and the decision. Acceptance of the phase requires all six.

| Spike | Question | Output |
|-------|----------|--------|
| A | Is `marty-static` position independent so that it can be linked into a Python extension? Do `csl-core`, `grafed-core`, `jsonparser`, `marty-lha-obj`, `sgl`, `marty-models` (all object libraries) carry `POSITION_INDEPENDENT_CODE`? Does `import` of a minimal `_pymarty` work with the Fortran LoopTools library linked? | Fix list, and a working minimal module |
| B | Can two `Model` objects coexist and be used in sequence and interleaved in one process when guarded by `Session`? Which core statics besides `Model::current` break this (registries, abbreviation cache `csl::Abbrev`, allocators)? | Pass/fail matrix and, if needed, extra state to snapshot |
| C | What happens on error today: which exceptions escape the core in practice (enum, `std::exception`, GammaAPIError), and which paths terminate? | Complete error-path table used by WP0.2/0.3 |
| D | Wheel feasibility: can the manylinux image build LoopTools (Fortran), GSL linking, bundle `libgfortran` with `auditwheel`, and the installed wheel import in a clean venv without compilers? Needs a container runtime. If none is usable locally (for example Docker Desktop without WSL integration), run it as a CI job on a branch and record the result from the job log. | Dockerfile/CI snippet, size of the wheel |
| E | Generated-library build from the wheel: can a compiler on the user machine compile a trivial generated library against headers and libraries from the wheel (ABI, `libstdc++` version, rpath)? (Only the compile-and-link question, the loader is Phase 6.) | Go/no-go and the required flags |
| F | Do `std::cout` redirect and output of long C++ computations behave well in Jupyter? | Binding helper recipe |

## 5. Test plan

L1 (C++ API tests; Phase 0 has no module of its own, so files are flat in `tests/api/src/` as `test_error_api.cpp`,
`test_session_api.cpp`, ...; from Phase 1 on, tests live in `tests/api/<module>/`):
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
   new required tools). With `ON`, `_pymarty` builds on GCC 14, GCC 11 and Clang 18 in CI. When some of
   these toolchains are not installed locally, the local evidence is the available subset and the
   missing ones are covered by the CI run of the PR (state which in the PR). GCC 10 only accepts
   `-std=gnu++2a`, not `gnu++23`, and is not a supported toolchain.
2. L0 regression: `ctest -L 'units|integration'` green on the modified tree, and no file under
   `tests/system/data/output/` changed. The `system` suite is not a criterion (testing-strategy.md
   section 1.1). Informational: the author compared one full `system` run before and after the core changes
   (result attached); later phases do not repeat it.
3. C++ behaviour unchanged for non-API users: an existing error path prints the same stderr text as
   before (checked by a test), and ends in a catchable exception.
4. All L1/L2/L3 tests listed above exist and pass. State-neutrality tests pass under ASan + UBSan.
5. Zero `exit`/`abort` reachable from the API surface of phases 1 to 7 (list of remaining ones
   attached, each with justification) and the audit table of spike C is committed.
6. Deferred: manylinux wheel, `auditwheel`, clean-venv install and distribution questions are moved to
   [distribution-deferred.md](distribution-deferred.md) and do not block Phase 0. A host-built wheel that
   installs and imports is informational.
7. Spikes A to F have decision records (spike D's record points to the deferred note). Any "no-go" result
   other than distribution has a mitigation agreed before closing the phase.
8. Quality gates of [testing-strategy.md](testing-strategy.md) pass (format, warnings, mypy, ruff,
   coverage >= thresholds for the new files).
9. Errors live in `pymarty.errors` (`from pymarty.errors import MartyError`) and are not re-exported at the
   top level. `python/examples/usage/errors.py` was approved by a human (reviewer and date in the PR),
   runs in `python/tests/test_usage_examples.py` (L4b), and the L4b test fails for a module that has no
   example.

## 7. Exit artifacts

Working skeleton on CI, spike records, error table, updated `AGENTS.md` pointer
to this folder (small addition to the documentation map), updated `docs/architecture/build-and-tests.md`
with the new option, label and macro.

## 8. Risks

- Changing `CallHEPError` could alter behaviour of code that catches the enum. Mitigation: audit, tests
  of legacy catches, baseline comparison.
- Option snapshot may miss flags living outside `mrtOptions.h` and `csl/options.h`. Mitigation: grep for
  other global mutable state (`inline static`, namespace-scope `bool`) and add to the audit table.
- manylinux toolchain may lack features needed by C++23 code. Mitigation: spike D decides the image and
  compiler (`gcc-toolset`), or the minimum supported GCC for wheels.
