Status: READY (D1 and D2 answered; awaiting G1 approval to freeze)
Updated: 2026-11-28
Author: main agent (orchestrator)

# F1 spec: restructure the layout (move only)

Scope: move files and update the paths that point to them. No behaviour change, no API change, no code edits
beyond path strings in build files, CI and docs. Source of the target layout:
[USER_FEEDBACK.md](../USER_FEEDBACK.md) "Repository layout". Inventory: [01-inventory.md](./01-inventory.md) section A.
Decisions: [02-questions.md](./02-questions.md) Q1 (move-only PR first), Q2 (`builtin_models`).

## 1. Decisions needed at G1

These two items are not pure moves. **Answered by the user:** D1 = (a), D2 = (a). The D2 inventory in section 6 is
a step-0 task of the implementation, before any file is moved.

**D1. Python built-in models and the import path.** 48 Python files import `pymarty.modeling.models.*`
(for example `python/pymarty/modeling/models/__init__.py:13-17`). `python/CMakeLists.txt:32-36` copies only
`pymarty/**` into the build tree. Moving the models to `builtin_models/python/` takes them out of the package,
so the import path `pymarty.modeling.models` would break.

- (a) Keep the import path. The models stay importable as `pymarty.modeling.models`, and the build copies
  `builtin_models/python/*.py` into `<build>/python/pymarty/modeling/models/`. No import changes. Recommended.
- (b) Change the import path to `pymarty.builtin_models`. This is a public API change and belongs to F3, not F1.

**D2. C++ built-in models.** The C++ "models" are the 8 sample programs in `examples/*.cpp` and the sources in
`tests/system/src/*.cpp`. USER_FEEDBACK wants them in `builtin_models/cpp/`. Those files are both examples and
test sources, so a move would also move test content.

- (a) Move only the model definitions that are reused by more than one program into `builtin_models/cpp/`, and
  leave the samples in `usage_examples/cpp/` (F2 rewrites them). Recommended, but the split is not yet
  inventoried (see section 6).
- (b) Defer the C++ models folder to F2, and in F1 only move `examples/` to `usage_examples/cpp/`.

## 2. Moves (all in scope)

| From | To | Notes |
|------|----|-------|
| `python/pymarty/` | `src/pymarty/` | Python package. The C++ `src/` tree has no `*.py` so there is no clash. Install rule `CMakeLists.txt:121` only copies `*.h` from `src/`. |
| `python/tests/` | `tests/python/` | includes `system/`, `conftest.py`, `test_usage_examples.py`. |
| `python/examples/` | `usage_examples/python/` | 7 files plus `group_theory_basics.py`. |
| `python/notebooks/` | `usage_examples/python/` | one notebook, `gthdm_investigation.ipynb`. |
| `examples/` | `usage_examples/cpp/` | 8 `.cpp` files and `CMakeLists.txt`. Scope of C++ models per D2. |
| `tests/{api,integration,system,units}` | `tests/cpp/` | and `tests/CMakeLists.txt`, `tests/README.md`, `CodeCoverage.cmake`. |
| `python/pymarty/modeling/models/` | `builtin_models/python/` | per D1 (a). |
| `dep/` | `third-party/` | `CMakeLists.txt:111` and `AGENTS.md:23,62`. |
| `python/pyproject.toml` | repo root `pyproject.toml` | scikit-build project at the root, next to `CMakeLists.txt`. `cmake.source-dir` becomes `.`; the `sdist.include` and `sdist.exclude` lists lose the `../` prefix. |
| `python/CMakeLists.txt` | `src/pymarty/CMakeLists.txt` | only the glob at lines 32-36 and the `configure_file` copy are kept. Its `add_subdirectory` line in `CMakeLists.txt:118` changes accordingly. |
| `python/src/*.cpp`, `python/src/symbolic_bridge.h` | `src/pymarty/cpp_binding/` | pybind11 C++ sources of `_pymarty`, next to the package they build. Problem: the install rule `CMakeLists.txt:124-126` installs every `*.h` under `src/` into `include/`, so `symbolic_bridge.h` would be installed. Fix in F1: add an exclude for `cpp_binding/` to that rule. |
| `python/tools/*.py` | `tests/python/tools/` | `check_parity.py` and `check_modeling_bindings.py` are verification scripts, not package code. |

After F1 no `python/` folder remains. Only `python/notebooks` and `python/examples` were moved to `usage_examples/`
(section 2 above), and the package, tests and models are in `src/pymarty/`, `tests/python/` and `builtin_models/python/`.

Not moved in F1: the Python module shapes (single files versus packages). This is F3 work, because
changing a module's shape changes its import path.

## 3. Path updates (must be in the same PR)

- `CMakeLists.txt`: `add_subdirectory(dep)` to `third-party`, `add_subdirectory(examples)` to
  `usage_examples/cpp`, `add_subdirectory(tests)` unchanged in name, and the `install` rules at lines 121-129.
- `src/pymarty/CMakeLists.txt` (moved from `python/CMakeLists.txt`): the glob covers `src/pymarty` and
  `builtin_models/python` (per D1). Its `configure_file` copy target stays `<build>/python/pymarty/`, so the
  `_pymarty` extension location and the tests do not change. The glob is also needed for the copied models.
- Root `pyproject.toml` (moved from `python/pyproject.toml`): `cmake.source-dir = "."`. The `sdist.include` list
  adds `src`, `builtin_models`, `third-party` and `tests/CMakeLists.txt`.
- `tests/CMakeLists.txt` and the C++ test `add_subdirectory` calls: point to `tests/cpp/`.
- `.github/workflows/python.yml` and `c-cpp.yml`: every `python/` path, and `dep/`.
- `AGENTS.md`: the layout table, the "Forbidden" and "Do not edit" paths (`dep/LoopTools-2.16/` becomes
  `third-party/LoopTools-2.16/`; `build/`), and the Python section (`python/tests`, `python/src`, `python/pymarty`,
  `python/examples`, `python/tools`).
- `docs/architecture/build-and-tests.md`, `docs/architecture/pymarty/*.md`, `docs/architecture/functionalities.md`:
  path references only.
- `python/tests/test_usage_examples.py`: the path to the examples folder.
- Stale notes (from [01-inventory.md](./01-inventory.md) E2):
  - `docs/architecture/todo/insertion-ordering.md` says "not committed" and "C9_SM not yet fixed". Git history
    has `96834ab1` and `5e5df91b`. Update the note to the current state, or delete it if it is done.
  - `docs/architecture/pymarty/phase-5-calculation.md`, "Findings and remaining work", the "blocked" list:
    remove `simpleSpectrum` and `testModelWidth` (ported at `python/tests/system/test_simple_spectrum.py` and
    `test_model_width.py`); keep `FItests`, `BsMixing*`, `wilsons_GTHDM`, `gm2SM`.

Phase documents are historical records. Only path references change; the text of the findings does not.

## 4. Verification (before and after the move)

The baseline is recorded before any file is moved, in `04-implementation-log.md`.

1. Test counts, without running the system suite (rules in `AGENTS.md`):
   - `ctest -N -L units`, `ctest -N -L integration`, `ctest -N -L api` (list only, do not run `ctest` without a label).
   - `pytest --collect-only -m "not slow" -q` on `tests/python` (no run of the slow tests).
   Counts must be equal before and after.
2. Build: out-of-tree configure and build of `marty`, `csl`, `_pymarty` with `-DMARTY_BUILD_PYTHON=ON`, and the
   CMake options used by the Python CI (`-DMARTY_BUILD_GRAFED_GUI=OFF -DMARTY_REQUIRE_LATEX_TOOLS=OFF`).
3. Tests: `ctest -L units`, `ctest -L integration`, `ctest -L api`; the Python suite with
   `-m "not slow"`; `python/tools/check_parity.py` from the repo root with the new paths.
   The system suite is not run. One named system test (for example `ctest -R test_simple_spectrum`) may be run
   to confirm the path, as the rules allow.
4. Static checks: `ruff check`, `ruff format --check`, `mypy --strict` on the new paths; the `mypy` error count must
   equal the baseline count (the phase 7 spec says 74 pre-existing errors, not re-verified; re-measure before the move).
5. `grep` for the old paths (`python/pymarty`, `python/tests`, `python/examples`, `python/notebooks`, `dep/`,
   `examples/`) across the repo, excluding `build/`, `.venv/` and the historical phase documents. Result must be empty
   except for the historical text listed in section 3.
6. `git status` shows renames (`git mv`), not delete-and-add, so that history is kept.

## 5. Out of scope for F1

- API changes, renames, removal of namespace classes, `mypy` cleanups: F3.
- Usage example content, notebooks running in CI: F2.
- `doctor`, wheels, Docker, GRAFED defaults (Q6d): F5. The GRAFED default change is not part of the move.
- Moving `phase-8-polish-release.md` or `USER_FEEDBACK.md` (no move without the user's approval).

## 6. Open inventory for D2

Not yet verified: which model definitions in `examples/*.cpp` and `tests/system/src/*.cpp` are shared. Step 0 of the
implementation records this in `04-implementation-log.md`, with `grep` evidence per file. Only definitions used by
more than one program move to `builtin_models/cpp/`. If no definition is shared, the folder is not created and this
is recorded as an amendment.

## Amendments

(none)
