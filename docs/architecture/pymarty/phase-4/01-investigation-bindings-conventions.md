Status: READY
Updated: 2026-11-21
Author: orchestrator (written from the findings of investigator "inv-bindings-conventions"; the subagent could not write files, the orchestrator re-checked the lines marked [V])

# Investigation: binding and convention pattern for a new module

## 1. Question

Which files does a new pymarty module touch, following phases 1 to 3, and how are errors and stream output handled?

## 2. Facts

1. [V] The C++ facade is separate from pybind11 and uses plain C++ types. For group theory: `src/marty/api/group_theory/group_theory.h` (header) and `group_theory.cpp`; the binding is `python/src/group_theory.cpp`.
2. [V] `python/pymarty/__init__.py:14-17` imports `config, diracology, errors, group_theory` and lists them in `__all__`. A new module must be added here.
3. [V] `python/src/module.cpp:296` registers the exception translator (`py::register_exception_translator(translateException)`). `module.cpp:315` redirects `cout` through `py::scoped_ostream_redirect` in the print helper.
4. [V] `python/tests/parity_sources.txt` lists only legacy test files (`tests/integration/src/gammaAPI.cpp`). `python/tests/parity_exemptions.txt` lists C++ API tests without a Python counterpart, for example `SessionApi.*` (line 5).
5. Reported, not re-checked: `python/src/module.cpp:37` declares the group-theory binding entry point defined in `group_theory.cpp`; `python/CMakeLists.txt:20` lists the binding source. A new binding needs the same two additions.
6. Reported, not re-checked: the Python wrappers (`python/pymarty/group_theory.py`) contain no algorithm, only constructor delegation to `_core`; stubs `group_theory.pyi` mirror them.
7. Reported: the documented error set is `Error`, `ValueError`, `TypeError`, `LookupError`, `NotImplementedError`, `StateError`, `BuildError` (`docs/architecture/pymarty/api-conventions.md` section 1.6; `src/marty/api/common/error.h`). The Python hierarchy is in `python/pymarty/errors.py`.
8. Reported: the autouse state fixture is `python/tests/state_guard.py`; usage examples run through `python/tests/test_usage_examples.py`; `python/tests/test_clean_state.py` checks state restoration.

## 3. Files a new module touches

| Area | Files |
|------|-------|
| C++ facade | `src/marty/api/modeling/*.h`, `*.cpp`; `src/marty/api/CMakeLists.txt` |
| C++ tests | `tests/api/modeling/*.cpp` (label `api`) |
| Binding | `python/src/modeling.cpp`, `python/src/module.cpp` (declaration and call), `python/CMakeLists.txt` |
| Python | `python/pymarty/modeling.py`, `modeling.pyi`, `__init__.py` |
| Tests and checks | `python/tests/modeling/`, `python/tests/parity_sources.txt` or `parity_exemptions.txt` if needed, `python/examples/usage/modeling.py` |

## 4. Consequences

- The Python layer for modeling must be syntax only: argument coercion, operators, iteration, exception translation.
- Handles and immutable rule vectors must be wrapped in C++, not in Python.

## 5. Questions for the user

None on this topic.

## 6. Not verified

- Exact contents of `src/marty/api/group_theory/` headers beyond the cited lines.
- Whether `api-conventions.md` rules 1 to 12 need an update for handles that hold a model (lifetime rule in the plan section 3).
