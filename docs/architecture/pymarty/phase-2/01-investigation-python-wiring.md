Status: READY
Updated: 2026-11-20
Author: investigator

# Phase 2 investigation 01: python/ wiring, parity checker, and Phase 0 state

## Question

How is the `python/` tree wired today, so that a new module `pymarty.diracology`
(`python/src/diracology.cpp`, `python/pymarty/diracology.py`, `.pyi`, `tests/diracology/`)
can be added and registered by the parity checker and the usage-example test? Also: what does
Phase 0 deliver, and is there a record that Phase 0 was accepted at gate G3?

Read-only investigation. No build, no ctest, no pytest. Commands run: `python3
python/tools/check_parity.py`, `git log`, `git show --stat`, `git ls-tree`, `git cat-file`, grep,
and `view`-style reads.

## Facts

### 1. How `symbolic` is registered in `_pymarty`

- `python/CMakeLists.txt:20`: `pybind11_add_module(_pymarty src/module.cpp src/symbolic.cpp)`.
  Line 21 links `marty-static`.
- `python/src/module.cpp:32`: `void bindSymbolic(py::module_ &m);` (declaration).
- `python/src/module.cpp:269`: `bindSymbolic(m);` (call inside the `PYBIND11_MODULE` body).
- `python/src/module.cpp:272-287`: the `_debug` submodule.
- `python/src/symbolic.cpp:1273`: `void bindSymbolic(py::module_ &m)` (definition).
- `python/pymarty/symbolic.py` is a thin re-export of names from `pymarty._pymarty`; it has no
  pure-Python logic.
- `python/pymarty/__init__.py:12`: `from pymarty import config, errors`.
  `__init__.py:15`: `__all__ = ["__version__", "config", "errors"]`. `symbolic` is not imported
  there; it is imported explicitly by users and tests.

Pattern for diracology: one new `bindDiracology(py::module_ &m)` declaration in module.cpp, a
call next to line 269, a definition in a new `python/src/diracology.cpp`, one added source in
`python/CMakeLists.txt:20`, and a `pymarty/diracology.py` re-export plus `.pyi` stubs.

- `python/CMakeLists.txt:29-36`: `file(GLOB ... CONFIGURE_DEPENDS)` plus `configure_file` copies
  `pymarty/*.py`, `*.pyi` and `py.typed` into the build tree. A new `diracology.py` is therefore
  copied at configure time. CONFIGURE_DEPENDS re-globs at build time, but a new file still needs
  a re-configure to be copied.
- `python/CMakeLists.txt:49-56`: `pymarty.pytest` ctest (`-m "not slow"`). Lines `57-58`:
  `pymarty.parity`. Label `python`.

### 2. How the parity checker discovers C++ tests

File: `python/tools/check_parity.py`.

- Line 27: `API_TEST = re.compile(r"^\s*TEST(?:_F)?\(\s*(\w+Api)\s*,\s*(\w+)\s*\)", re.MULTILINE)`.
  Only suites whose name ends in `Api` match.
- Line 28: `ANY_TEST = ...\s*(\w+)\s*,\s*(\w+)...` (any suite name).
- Line 42-43: `tests/api/**/*.cpp` (rglob) scanned with `API_TEST`.
- Lines 44-47: each file listed in `python/tests/parity_sources.txt` is scanned with `ANY_TEST`.
- Lines 59-68: exemptions parsed from `python/tests/parity_exemptions.txt`, format `<id> | reason`.
- Python markers: `pytest.mark.parity("<id>")` in `python/tests/**/*.py`.

`python/tests/parity_sources.txt`: lines 1-4 are comments only. Quote (line 3-4): "None yet: the gamma
API test (tests/integration/src/gammaAPI.cpp) is listed here when pymarty exposes the gamma API."
So **`tests/integration/src/gammaAPI.cpp` is NOT in parity_sources.txt today.**

`python/tests/parity_exemptions.txt`: 6 active lines: `ErrorApi.StdExceptionIsTranslated`,
`ErrorApi.LegacyThrowStillCatchableAsCoreException`, `ErrorApi.ExistingErrorPathKeepsStderrTextAndIsCatchable`,
`SessionApi.*`, `TerminatingCallsApi.*`, `SymbolicApi.SubsCompoundKeyBehaviourPinned`.

Consequence: gammaAPI.cpp has 12 `TEST(marty_gamma_api, ...)` cases (lines 36, 89, 115, 176,
245, 294, 356, 441, 558, 619, 707, 806). The suite name `marty_gamma_api` does not end in `Api`,
so it is invisible to `API_TEST`. Listing the file in parity_sources.txt makes the checker
expect 12 ids of the form `marty_gamma_api.<name>`, each needing a `pytest.mark.parity` marker or
an exemption.

**Current result** (run read-only):
`parity: OK, 85 C++ API tests: 63 with a Python counterpart, 22 exempted`, exit 0.

### 3. How `test_usage_examples.py` decides which modules are present

File: `python/tests/test_usage_examples.py`.

- Line 16: examples directory `python/examples/usage/`.
- Lines 19-27: `MODULES_WITH_EXAMPLE` list, which includes `"diracology"`.
- Lines 30-31: `installed_modules()` uses `pkgutil` over `pymarty.__path__`.
- Lines 36-38: `test_usage_example_runs` runs each existing example file. If `pymarty.<stem>` is
  not installed it calls `pytest.skip(...)` (line 38). A missing module is a skip, not a failure.
- Lines 44-50: `test_every_module_has_a_usage_example` fails only when a module that IS installed
  has no example in `MODULES_WITH_EXAMPLE`.

Answer: `python/examples/usage/diracology.py` does **not** exist (only `errors.py` and `symbolic.py`
are present). It is not needed while `pymarty.diracology` is absent. Once the module is installed,
its absence becomes a test failure at line 44-50. Per the phase-2 plan, the example must be approved
before implementation (gate G1), so it should exist before the binding is installed.

### 4. ADD_API_TEST macro and tests/api/ layout

- `tests/api/CMakeLists.txt:1-6`: `function(ADD_API_TEST NAME)` creates target `${NAME}.x` from
  `src/${NAME}.cpp`, calls `SETUP_MARTY_TEST_TARGET` and `SETUP_GTEST_TARGET(... api)`, then
  `endfunction()` at line 6.
- `tests/api/CMakeLists.txt:8-20`: 13 registrations (`test_error_api`, `test_session_api`,
  `test_terminating_calls_api`, `test_config_api`, nine `symbolic_*`).
- `tests/api/CMakeLists.txt:21-24`: data-path definitions.
- `tests/api/src/`: 13 `.cpp` files, flat, no module subfolders.
- `tests/CMakeLists.txt:5-11`: `SETUP_MARTY_TEST_TARGET`. `tests/CMakeLists.txt:23-27`:
  `SETUP_GTEST_TARGET`. `tests/CMakeLists.txt:31`: `add_subdirectory(api)`.
- `tests/integration/CMakeLists.txt:1-6`: `ADD_INTEGRATION_TEST`. Line 10: `ADD_INTEGRATION_TEST(gammaAPI)`.

Gap: the phase-2 plan puts C++ tests in `tests/api/diracology/`. The current layout is flat, and
the phase-0 spec says flat during Phase 0. A subfolder needs a `file(GLOB)` or explicit
`add_subdirectory` change, and `ADD_API_TEST` must accept a subpath (it currently assumes
`src/${NAME}.cpp`).

### 5. Phase 0 deliverables present in the repository

Present (commit `eaa9ce6`, "API Phase 0 - error system & basic pymarty", 41 files):

- `python/` skeleton: `CMakeLists.txt`, `src/module.cpp`, `src/symbolic.cpp`, `pyproject.toml`,
  `pymarty/{__init__,errors,config,symbolic}.py` and `.pyi`, `py.typed`.
- Tests: `python/tests/test_clean_state.py`, `test_config.py`, `test_errors.py`, `test_output.py`,
  `state_guard.py`, `conftest.py`, `test_usage_examples.py`.
- `python/tools/check_parity.py`.
- `src/csl/api/error.h`; `src/marty/api/common/{config,error,session}.h`.
  Note: no `rational.h` in `src/marty/api/common/` (verified; deferred to Phase 1 per the phase-0 plan).
- `tests/api/src/{test_error_api,test_session_api,test_terminating_calls_api,test_config_api}.cpp`.
- `session.h:32-76`: `Session` RAII guard saves all `MTY_MARTY_OPTION_FLAGS` and
  `MTY_CSL_OPTION_FLAGS` and sets `printErrors = false`.
- `docs/architecture/pymarty/spike-{a..f}-*.md` statuses: A PARTIAL, B PARTIAL, C DONE (static
  audit, `spike-c-error-paths.md:3`), D BLOCKED (deferred), E DONE (host), F PARTIAL.

Phase 1 implementation (for context): `phase-1/04-implementation-log.md` shows WP4 and WP5 done,
parity at 68/46/22 at that time, and the G3 round "awaiting user acceptance at G3. Not accepted yet"
(line 520-522). The current parity count (85/63) is higher, so the tree has moved on since then.

### 6. Is there a record that Phase 0 was accepted at G3?

**Verdict: NO RECORD (unknown). No evidence of acceptance found.**

- `grep -rn "G3"` over `docs/architecture/pymarty/` returns only Phase 1 entries
  (`phase-1/03-spec.md:304`, `phase-1/04-implementation-log.md:520-522`). None concern Phase 0.
- `git log --all --grep` for G3, "accept", "phase 0" returns nothing relevant.
- `process/README.md:3`: "Phase 0 was run differently and is not covered here." The Phase 0
  process is not documented.
- Phase 1 docs state "Prerequisites: Phase 0 accepted" without a dated acceptance record.

Reading: the gate sequence is documented in `process/README.md:37-45`, but no G3 sign-off for
Phase 0 is written anywhere in the repository. Acceptance may have happened outside the repo; the
repository does not show it.

### 7. Old gamma binding (context for the parity question)

- Commit `39f288f` ("All outputs in ntoebook") and branch `origin/api/gamma` contain the old
  binding: `python/src/pymarty/gamma_api/`, `python/tests/test_{import,objects,simple_api,simplifications}.py`,
  `src/pybinding/gamma_api.cpp`, `python/notebooks/sample_gamma_api.ipynb`. Verified with
  `git cat-file -e`.
- `39f288f` is not an ancestor of `HEAD`. These files are not in the current tree.
- The current `src/marty/api/gamma.{h,cpp}` and `gamma_api_error.h` are the facade that a
  diracology C++ layer would build on.

## Options

1. **Parity source (gammaAPI.cpp).** (a) Add it to `parity_sources.txt` now: the checker then expects
   12 `marty_gamma_api.*` ids, each needing a marker or exemption, and the current OK state
   becomes FAIL until those are added. (b) Add it at G1 with the markers in the same change. Option (b)
   keeps the checker green between phases. Opinion: (b).
2. **Binding location.** Phase-2 plan and `pymarty/README.md` §5 say `python/bindings/`; the actual
   code is `python/src/`. Opinion: follow the actual tree (`python/src/diracology.cpp`) and fix the
   doc, or move the plan's path to match.
3. **C++ test layout.** Flat `tests/api/src/test_diracology_*.cpp` (matches today) versus
   `tests/api/diracology/` (plan). The flat layout needs no `ADD_API_TEST` change. Opinion: flat, unless the
   user wants the plan path, in which case `ADD_API_TEST` needs a subpath argument.
4. **Usage example.** Create `python/examples/usage/diracology.py` at G1 so that the module
   is covered as soon as it is installed. Before that, the test skips.

## Questions for the user

1. Is the plan's `python/bindings/` path authoritative, or should the binding live in `python/src/`?
2. Should `gammaAPI.cpp` be listed in `parity_sources.txt` at G1 (with its 12 markers) or later?
3. Is the phase-2 C++ test path `tests/api/diracology/` required, or is flat acceptable?
4. Was Phase 0 accepted at G3? No record in the repository. If yes, where is it recorded?

## Not verified

- Build tree state (`/home/gregoire/marty-public/build` not touched, per instructions). I did not
  check whether the current build compiles or whether `ctest -L api` is green now.
- ctest and pytest not run (per instructions). The parity result above is from one read-only run of
  `check_parity.py`.
- Whether `gammaAPI.cpp` still compiles against the current facade.
- Whether `python/tests/symbolic/test_port_table.py` or other docs already reference diracology.
- Whether Phase 0 acceptance happened outside the repository (chat, issue tracker).
