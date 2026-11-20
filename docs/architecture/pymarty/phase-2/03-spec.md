Status: APPROVED (user G1, 2026-11-20). Q4 answered B (leave A1 to A4 for a later phase). O1 answered flat test file. Q5 and Q6 answered as recorded in 02-questions.md. Implementation pending.
Updated: 2026-11-20
Author: orchestrator

# Phase 2 (`pymarty.diracology`): specification

Inputs: [02-questions.md](02-questions.md) (answers Q0 to Q3 recorded there),
[../phase-2-diracology.md](../phase-2-diracology.md) (plan, updated to match this spec),
[01-investigation-gamma-cpp.md](01-investigation-gamma-cpp.md),
[01-investigation-old-binding.md](01-investigation-old-binding.md),
[01-investigation-python-wiring.md](01-investigation-python-wiring.md),
usage example [python/examples/usage/diracology.py](../../../../python/examples/usage/diracology.py).

## 0. Baseline

Run 2026-11-20 in `/tmp/pymarty-build` (GCC 11.4, `/tmp/pymarty-venv`), before any change. Full build `cmake --build . -j12`: exit 0, 0 errors.
- `ctest -L 'units|integration'`: 197 of 197 passed (`/tmp/p2-baseline-ctest.log`).
- `ctest -L 'api|python'`: 131 of 131 passed (`/tmp/p2-baseline-api-python.log`), including pytest with `-m "not slow"`.
- `python/tools/check_parity.py`: `OK, 85 C++ API tests: 63 with a Python counterpart, 22 exempted`.

Green baseline, so any later failure belongs to this phase.

## 1. Decisions from the Q&A

| Q | Answer | Consequence in this spec |
|---|--------|--------------------------|
| Q0 | Phases 0 and 1 are accepted by the user | No blocker. Phase 1 log is updated with the acceptance date (section 9). |
| Q1 | A: keep `GammaAPIError` unchanged | The new API translates `GammaAPIError` and `sgl::MathError` at its boundary (section 3.4). |
| Q2 | A: canonical C++ equality, unhashable | `DiracExpr.__eq__` uses C++ canonical comparison; `__hash__ = None`. |
| Q3 | A: plan format | `FierzBasis.STANDARD` / `CHIRAL`; `single_fierz` and `double_fierz` return a named `(rhs, lhs)` result; indices are explicit, no hard-coded 1..4. |

Plan change (decided by the user after G1, "api::diracology duplicates gamma_api, we should have only one API"):
there is no second wrapper layer. `src/marty/api/gamma.{h,cpp}` are renamed to `diracology.{h,cpp}` and adapted in
place, in namespace `mty::api::diracology`. The `mty::gamma_api` namespace no longer exists.

## 2. Scope

In scope: the Python API of `pymarty.diracology` over the single Dirac API in `src/marty/api/diracology.{h,cpp}`
(gamma matrices, metric, epsilon, gamma5, projectors, `C`, `D`, `I`, `current`, `trace`, simplify, order,
project, Fierz, LaTeX and string output, dimension switch), arithmetic, equality, indexing, copy, and the
input validation needed by the invalid-input matrix (section 6).

Out of scope (each needs a separate decision): momenta and slashed momenta, `sigma^{mu nu}`, named
indices, conversion to `csl::Expr` tensors, and the Phase 1 deferred items A1 to A4 (see question Q4 in
[02-questions.md](02-questions.md)).

## 3. C++ API (`src/marty/api/diracology.{h,cpp}`)

Files: `src/marty/api/diracology.h`, `src/marty/api/diracology.cpp` (renamed from `gamma.{h,cpp}`), namespace
`mty::api::diracology`. There is one Dirac API; the functions below are added to it, and the existing facade
functions keep their names.

### 3.1 Functions

| Function | Purpose |
|----------|---------|
| `size(expr)`, `argument(expr, i)` | Introspection (replaces `expr->size()` / `expr->argument(i)` in the old binding) |
| `equals(a, b)`, `less(a, b)` | Canonical comparison (replaces `sgl_to_csl` comparison in the old binding) |
| `from_int`, `from_rational`, `from_double`, `scale(expr, Rational)` | Scalar coefficients |
| `deep_copy(expr)` | Independent copy |
| `ScopedSymbolicDimension` | RAII guard over `sgl::option::keepEvanescentOperators`, restores on exit and on exception |
| `singleFierz(c1, c2, basis)`, `doubleFierz(c1, c2, basis)` | Return `FierzResult{rhs, lhs}`; replace `applySingleFierz` / `applyDoubleFierz` (now private helpers) |

Names in C++ are camelCase (`fromInt`, `deepCopy`); the snake_case names above are the Python names.

The binding calls only these functions, never `sgl::` or `csl::` directly.

### 3.2 Index validation

Checked in `diracology.cpp` (`IndexManager`, `requireNonNegative`) before SGL is called:
- negative Lorentz or Dirac indices (`gamma`, `g`, `eps`, `current`)

No collision rule for `current`: Dirac indices (`d` prefix) and Minkowski indices (`m` prefix) are
distinct SGL indices (`src/marty/api/diracology.cpp`, `IndexManager`). Equal Dirac indices are legal, since
`trace` is `current(gammas, 0, 0)` and `trace({})` stays the identity trace `4` (pinned by
`tests/integration/src/gammaAPI.cpp`, `simple_traces`).

### 3.3 Dimension switch

`keepSymbolic4D(bool)` stays. The Python `symbolic_dimension` context manager and
`set_symbolic_dimension` use `ScopedSymbolicDimension`. The global flag is restored after every call.

### 3.4 Error translation

- The Fierz functions translate `GammaAPIError` to `mty::api::ValueError` (`translateFierzErrors`, `diracology.cpp`).
  Index validation throws `ValueError` directly.
- Other functions (`trace`, `current`, ...) are not wrapped: their `sgl::MathError` behaviour is pinned by
  `tests/integration/src/gammaAPI.cpp` (lines 411-417) and stays unchanged.
- `GammaAPIError` is not derived from `mty::api::Error`; the boundary mapping stays in `rethrowAsApiError()`
  (`src/marty/api/common/error.h`). Q1 answer A: the class itself is unchanged.

## 4. Python layer (`python/src/diracology.cpp`, `python/pymarty/diracology.py`, `.pyi`)

| Python | Maps to |
|--------|---------|
| `DiracExpr` | `mty::api::diracology` expression type (the old package called it `Expr`) |
| `g(mu, nu)`, `eps(a, b, c, d)` | `g`, `eps` |
| `gamma(mu)`, `gamma(mu, nu)`, `gamma5()`, `P_L()`, `P_R()`, `C()` | same |
| `D`, `I` | `DMinko()`, `imaginaryUnit()` as module constants |
| `current(chain, iL, iR)`, `trace(chain)` | `current`, `trace` |
| `.simplify()`, `.order()`, `.project(basis)` | `simplified`, `ordered`, `project` |
| `single_fierz(c1, c2, basis=...)`, `double_fierz(c1, c2)` | `singleFierz`, `doubleFierz`, returning a named tuple `(rhs, lhs)` |
| `FierzBasis.STANDARD`, `FierzBasis.CHIRAL` | `FierzBasis::Standard`, `FierzBasis::Chiral` (an `enum.Enum`) |
| `symbolic_dimension(keep=...)` context manager, `set_symbolic_dimension(keep_symbolic=...)` | `ScopedSymbolicDimension` |
| `str(e)`, `.to_latex()`, `_repr_latex_()` | `generateString`, `generateLatex` |

Rules:
- Indices accept anything with `__index__`.
- Coefficients accept `int`, `float`, `Fraction`, or a `pymarty.symbolic` `Expr` (the C++ binding unwraps `ExprHandle`).
- `current` and `trace` accept any iterable of `DiracExpr` (tuple, list, generator).
- `__eq__` is canonical; `__hash__` is `None`.
- The module imports nothing from other pymarty modules, so it works without `pymarty.symbolic`.
- Every public class, function and property has a docstring. Examples are doctested.

The old binding's `/` operator and `print_latex` are not in the Python API unless the user asks for them
(they are not in the plan).

## 5. Work packages

| WP | Content | Files | Verification |
|----|---------|-------|--------------|
| WP0 | Baseline run, recorded | none | section 0 commands |
| WP1 | C++ API layer, validation, error translation, scoped dimension | `src/marty/api/diracology/*`, `src/marty/api/CMakeLists.txt` (if needed) | new tests in `tests/api/src/test_diracology_api.cpp` (flat, see open item O1) run with `ctest -R test_diracology_api` |
| WP2 | pybind11 binding, Python wrapper, stubs | `python/src/diracology.cpp`, `python/src/module.cpp` (declaration and call), `python/CMakeLists.txt` (source), `python/pymarty/diracology.py`, `python/pymarty/diracology.pyi`, `python/pymarty/__init__.py` if needed | build `_pymarty`, `ruff`, `mypy --strict`, pytest |
| WP3 | Port of the old tests (`git show 39f288f:python/tests/<file>`) and parity markers for `gammaAPI.cpp` | `python/tests/diracology/*`, `python/tests/parity_sources.txt` | pytest `-m "not slow"`, `check_parity.py` |
| WP4 | Usage example run, notebook (exit artifact), CHANGELOG decision | `python/examples/usage/diracology.py`, notebook | `test_usage_examples.py`, nbmake or equivalent |
| WP5 | Doc updates (phase plan, implementation log) | `docs/architecture/pymarty/` | review |

Order: WP0, then WP1, then WP2, then WP3 and WP4 in parallel, then WP5. Each WP is implemented by a
subagent and verified by the orchestrator. No commits by agents.

## 6. Acceptance criteria

Each criterion has a command. "Done" means the command passes and its output is in the implementation log.

1. Every case of `tests/integration/src/gammaAPI.cpp` has a Python counterpart in parity, or an exemption with a reason. Command: `python/tools/check_parity.py`.
2. The four old Python test files are ported; each assertion is present or has a documented reason for dropping. Command: `pytest python/tests/diracology -m "not slow"`.
3. Identity checks from the plan hold in C++ and in Python: `gamma5()*gamma5() == 1`, `P_L()+P_R() == 1`, `P_L()*P_R() == 0`, `P_L()*P_L() == P_L()`, `trace([gamma(0), gamma(1)]) == 4*g(0,1)`, odd trace is 0, `eps(0,1,2,3)*eps(0,1,2,4) == -6*g(3,4)`. Command: `ctest -R test_diracology_api` and pytest.
4. The binding has no algebra: `grep -n "sgl::\|csl::" python/src/diracology.cpp` returns nothing. Command: the grep.
5. State neutrality: the symbolic-dimension flag is the same before and after every test, including after an error. Command: pytest fixture check.
6. Independence: `pymarty.diracology` imports succeed with `pymarty.modeling`, `calculation`, `simulation`, `group_theory` blocked. Command: pytest subprocess test.
7. Invalid-input matrix: one Python test per validation rule and per `gamma.cpp` throw site (lines 200, 206, 216, 347, 406), checking exception class and message.
8. `ctest -L integration` stays green, including `gammaAPI`. Command: `ctest -L integration` (not `system`).
9. Stubs, doctests, `ruff check .`, `ruff format --check .`, `mypy --strict pymarty tests tools`, `clang-format --dry-run --Werror python/src/diracology.cpp`. Commands as listed.
10. `python/examples/usage/diracology.py` is approved by a human before implementation (G1), and runs in `test_usage_examples.py`. Command: `pytest python/tests/test_usage_examples.py -m "not slow"`.

Tests are added only for behaviour this phase introduces or fixes (AGENTS.md); no tests for constants, re-exports or trivial accessors.

## 7. Tests to keep or drop

The old-branch tests contain assertions that are not in the plan. Each is kept or dropped with a reason in
the port table (`phase-2/port-table.md`, written in WP3):
- Clifford and order-sensitivity checks: keep if they test a documented convention.
- `Tr` with `gamma5` and `P_L`: keep if the value is in `gammaAPI.cpp`, else check it against the implementation first.
- `C*C = -1`, and equality of currents: keep if covered by the C++ identity list above.
- List-based Fierz with fixed indices 1..4 (`test_simplifications.py:78-89`): drop (hard-coded indices, Q3 answer A). Replace with explicit-index tests.

## 8. Risks

- Symbolic coefficients: supported. The `diracology` → `symbolic` dependency is accepted, mirroring SGL → CSL in C++ (decided by the maintainer). The spike confirmed SGL handles a CSL coefficient in `g*x`, `x*g` and `==`.
- SGL can assert or throw `sgl::MathError` on unusual input. The invalid-input matrix finds these; the fix goes in the API layer.
- Unhashable `DiracExpr` breaks code that puts expressions in sets or dict keys. Accepted per Q2.
- Build time: a full build of `_pymarty` is long. Use the smallest target (`--target _pymarty`) and single test targets.

## 9. Exit artifacts and housekeeping

- Notebook `python/examples/diracology_basics.ipynb` (revived from the `api/gamma` branch notebook), output reviewed.
- Stubs and docstrings.
- Parity mapping of the legacy test file.
- CHANGELOG entry: no `CHANGELOG*` file exists at the repository root. Needs a decision (Q6).
- Phase 1 implementation log: add the acceptance date (the user has accepted Phase 1).
- Phase 2 plan updated to match this spec (section 1 plan change).

## 10. Open items for G1

- O1: test location. The plan says `tests/api/diracology/`, but `tests/api/CMakeLists.txt` uses a flat `ADD_API_TEST(name)` macro. Proposal: flat `tests/api/src/test_diracology_api.cpp`.
- Q4: the Phase 1 deferred items A1 to A4 (matrix printing, non-commuting symbols, LaTeX index names, signed indices, 44 test cases) are generic CSL printing, not Dirac algebra. Include them in Phase 2 or leave them for a later phase?
- Q5: symbolic coefficients in `DiracExpr`: in scope (with `__marty_expr__`), or `NotImplementedError` in this phase?
- Q6: CHANGELOG: no file exists. Create one, or skip the CHANGELOG entry?
