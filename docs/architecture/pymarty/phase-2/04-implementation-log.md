Status: COMPLETE, pending user acceptance and commit.
Updated: 2026-11-21
Author: orchestrator

# Phase 2 implementation log

Scope and acceptance criteria: [03-spec.md](03-spec.md), section 6. Old-to-new test mapping:
[port-table.md](port-table.md).

## Packages

| Package | Title | Status |
|---------|-------|--------|
| P2-1 | Merge `gamma_api` into `mty::api::diracology` (single API, no wrapper) | done (commit `783cdbc`) |
| P2-2 | `pymarty.diracology` binding, usage example, `order()` fixed point, symbolic coefficients | done (commit `62afa96`) |
| P2-3 | Gap closure: C++ identity test, parity tests for `marty_gamma_api`, facade `Coefficient` (option A), invalid-input tests, subprocess import test | done (this commit) |
| P2-4 | Port table, spec amendments, gate run | done (this commit) |

## Decisions recorded during implementation

- `P_L()+P_R()` is deliberately not simplified to `1` (spec criterion 3).
- Bare `gamma5()` outside a chain is rejected (RuntimeError). The identity `gamma5()*gamma5()==1`
  is checked in its `current([...])` form.
- Criterion 4 (option A, user decision): the `sgl`/`csl` conversion and comparison logic lives in the
  facade (`dirac::Coefficient`, `dirac::equalsCoefficient`). The binding keeps one `csl::` use, the
  `csl::api::ExprHandle` cast that accepts a symbolic coefficient (accepted diracology→symbolic
  dependency).
- Trace errors from the algebra core reach Python as `MartyValueError`. The `sgl` and `std` exception
  types are converted in `rethrowAsApiError()`, so no bare `RuntimeError` escapes the binding.
- Chisholm identities: the sigma and fully-contracted cases use the default epsilon-index ordering. The
  simple case sets it with `epsilon_index_ordering(order=False)`, a scoped guard over
  `sgl::option::orderEspilonIndices`, matching the C++ `ScopedProperty(false)` reference.
- `set_symbolic_dimension` is kept as a plain setter; the `symbolic_dimension` context manager is the
  preferred form.
- A `P_L`/`P_R` merge rule in `src/marty/sgl/simplify.cpp` was tried and reverted: it broke
  `marty_gamma_api.basis_projection` (user decision).

## Gate results (final tree)

| Gate | Command | Result |
|------|---------|--------|
| C++ tests | `ctest -L 'units\|integration\|api\|python'` in `/tmp/pymarty-build` | 336/336 passed (units 151, integration 46, api 137, python 2) |
| Python tests | `pytest python/tests -m "not slow"` | 359 passed |
| Parity | `python/tools/check_parity.py` | OK, 105 C++ API tests: 82 with a Python counterpart, 23 exempted |
| Lint | `ruff check .`, `ruff format --check .` (in `python/`) | clean |
| Types | `mypy --strict pymarty tests tools` | clean, 35 files |
| C++ format | `clang-format --dry-run --Werror` on the four changed C++ files | clean |
| Criterion 4 | `grep -n "sgl::\|csl::" python/src/diracology.cpp` | one line: `csl::api::ExprHandle` cast |

Not run: `ctest -L system` and unfiltered `ctest` (forbidden by [AGENTS.md](../../../../AGENTS.md)).

## Known gaps carried forward

- The CHANGELOG question is open: the repository has no CHANGELOG file.
