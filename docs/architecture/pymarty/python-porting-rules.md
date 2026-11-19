# Ground rules for porting CSL and MARTY features to pymarty

Status: proposal, pending user acceptance. Items marked **Decision needed** are not settled. They are
listed in section 9 and must be answered before the rules are applied to code.

This document complements [api-conventions.md](api-conventions.md), which holds the general C++ and
Python rules. It covers the workflow and the choices specific to porting a feature. The list of features
not yet ported is kept in [unported-features.md](unported-features.md) and is updated with every port.

## 1. Principles

1. **Thin layers.** The C++ API (`src/csl/api/`, `src/marty/api/`) and the Python wrapper
   (`python/src/*.cpp`, `python/pymarty/`) do not reimplement algorithms. They adapt: argument coercion,
   plain-data conversion, exception translation, operators, docstrings. A new algorithm lives in CSL or
   MARTY. The only exception is section 6 (callback algorithms).
2. **Tests first.** Every port starts with failing tests (section 3). The Python tests and the usage
   example are reviewed by the user before the implementation starts, because the Python API is what the
   user sees.
3. **Mirror CSL.** The Python layer follows CSL's concepts and names, unless a documented Python spelling
   is listed in the phase document (for example `oo` becomes `infinity`, `subs` becomes `replace`).
4. **Ask rather than patch.** If CSL is inconsistent or missing a feature the Python layer needs, the
   change is proposed to the user in the phase document. The C++ implementation is then changed so that
   the two always align. The Python layer does not work around a CSL inconsistency silently.
5. **Immutability, and no setters.** Expressions and tensors are values. Direct member modification
   (the CSL `set*` virtuals) is never ported. A change is expressed by building a new expression or
   sub-expression (section 4), even though the C++ code allows in-place modification.

## 2. Checklist for porting one feature

Each step is required unless marked optional. The order is the order of work.

| # | Step | Where | Notes |
|---|------|-------|-------|
| 1 | Record the feature in the phase document, and in [unported-features.md](unported-features.md) with status "in progress" | `docs/architecture/pymarty/` | Name the CSL symbol and file |
| 2 | Write the Python usage example or the Python test that shows the intended API | `python/examples/usage/`, `python/tests/<module>/` | Failing. Reviewed by the user before step 4 |
| 3 | Write the C++ API test, if C++ changes or a new C++ API function is needed | `tests/api/src/symbolic_*.cpp` (one file per topic) | Failing |
| 4 | C++ API function: plain data in and out, typed errors, doc comment with the intended Python name and one example | `src/csl/api/*.h`, `*.cpp` | No pybind headers. See [api-conventions.md](api-conventions.md) section 1 |
| 5 | Pybind11 binding: argument coercion, keyword arguments, defaults, docstring with doctest examples | `python/src/symbolic.cpp` (or `module.cpp`) | Only syntax. No algorithm |
| 6 | Re-export from the Python package | `python/pymarty/symbolic.py` (import list and `__all__`) | Module docstring lists the public names |
| 7 | Stubs | `python/pymarty/symbolic.pyi`, `_pymarty.pyi` | Must agree with the binding (AGENTS.md "Stub sync") |
| 8 | Duck typing for any new input type | `coerce`, `tryAsExpr` in `symbolic.cpp`; `typing.Protocol` in the stubs | See section 5 |
| 9 | New exception class (only if none fits) | `src/csl/api/error.h`, `python/pymarty/errors.py` | Derives from `MartyError` and a built-in |
| 10 | Parity marker and parity source entry | `@pytest.mark.parity("<Module>Api.<Behaviour>")`, `python/tests/parity_sources.txt` | Checked by `python/tools/check_parity.py` |
| 11 | Port table, when the feature is a CSL unit test | `docs/architecture/pymarty/phase-1/port-table.md` | Checked by `test_port_table.py` |
| 12 | Update [unported-features.md](unported-features.md) to "ported", and the phase document | `docs/architecture/pymarty/` | Also the changed decisions |
| 13 | Verification (section 7) | | |

## 3. Tests first

- The Python test or example is written first and fails for the right reason (a missing attribute or
  function, not an import error or a typo). The failure is shown in the PR.
- The C++ API test is written first when step 4 is needed. It fails for the same reason.
- The user reviews step 2 for usability before step 4 starts. This is the gate for the Python API.
- A test is added only for behaviour the change introduces or fixes. Do not add tests that only check
  constants, exports, or the default of a keyword argument.

## 4. Expr methods or free functions

Proposal. The rule is that a function whose first argument is the subject of the operation is a method,
so that calls chain. Free functions are used for construction and for operations without a privileged
subject.

| Kind | Python form | Examples |
|------|-------------|----------|
| Builder (creates a new object from plain inputs) | free function | `variable`, `constant`, `sqrt`, `exp`, `sin`, `tensor`, `delta`, `metric`, `epsilon`, `Space` |
| Constant | module attribute | `I`, `pi`, `e`, `infinity` |
| Accessor without argument (read-only, total on its kind) | read-only property | `Expr.name`, `Expr.operand`, `Expr.free_indices`, `Tensor.name`, `Space.dimension` |
| Accessor with argument | method | `Expr.argument(i)`, `Expr.index(i)`, `TensorParent.permutation(...)` |
| Predicate on the subject | method | `Expr.depends_on(s)`, `Expr.is_real()` |
| Transformation of one object (returns a new object) | method | `expand`, `factor`, `collect`, `diff`, `replace`, `evaluate`, `relabel_indices` |
| Operation with no privileged subject, or n-ary construction | free function | `sum_of(...)` if needed |
| Callback algorithm (Python only) | free function in `symbolic_algorithms.py` (section 6) | |

Consequences for the current API:
- `relabel_indices` is the only Expr method that applies a renaming. `apply_renaming` is removed.
  See [phase-1-symbolic-addendum.md](phase-1-symbolic-addendum.md).
- The `set*` virtuals of CSL (`setOperand`, `setArgument`, `setName`, ...) are never exposed in Python, and
  no `with_*` or `set_*` counterpart is created. To change one argument, build a new expression, for
  example with `replace` or with a builder. Python objects are immutable.
## 5. Duck typing

- Numbers are tried in this order: exact integers (`__index__`), rationals (`Fraction`-like, numerator and
  denominator), `__float__`, `__complex__`. `Fraction(1, 6)` is never turned into a float.
- Expressions: anything that is an `Expr`, a number, or an object with a `__marty_expr__()` method. Booleans
  and strings are rejected, except where a string is documented as a name (`constant("g")`).
- Sequences and mappings: any iterable or `collections.abc.Mapping`. Results are `tuple`, `list`, `dict` or
  an immutable mapping view, as documented.
- New input types get a `typing.Protocol` in the stub (for example `SupportsExpr`), so that mypy strict
  accepts the duck-typed argument.
- Coercion lives in the binding (`coerce` and `tryAsExpr` in `python/src/symbolic.cpp`), not in Python code.

## 6. Callback algorithms

CSL algorithms that take a C++ callback (`ExpandIf`, `DistributeIf`, predicates) cannot be bound directly.
Decided (section 9, item 4): every such algorithm is duplicated in Python, consistently. The alternative of
replacing callbacks with data in C++ is dropped.

- **Duplicate in Python.** The algorithm is written in `python/pymarty/symbolic_algorithms.py`, imported
  by `symbolic.py`. It uses only the accessors that are bound. Every duplicated algorithm has parity tests
  that compare its output with the C++ algorithm on a shared set of expressions in `tests/api/data/symbolic/` (the existing shared data folder).
  Each duplicate is listed in [unported-features.md](unported-features.md) as "Python port of a callback
  algorithm".

Recommendation: (a) only. The C++ algorithms keep their callbacks. Each Python duplicate lives in
`python/pymarty/symbolic_algorithms.py` and has parity tests against C++ on shared golden expressions.
Duplicates must not contain physics or algebra that is not also tested against C++.

## 7. Verification

Run the smallest command that covers the change (AGENTS.md).

- Python: from the build directory, `ctest -L python`, or the direct pytest command in AGENTS.md with
  `-m "not slow"`. Never drop `-m "not slow"`.
- Python gates, in `python/`: `ruff check .`, `ruff format --check .`, `mypy --strict pymarty tests tools`.
- Parity: `PYTHONPATH=<build>/python <venv>/bin/python python/tools/check_parity.py` from the repo root.
- C++ formatting: `clang-format --dry-run --Werror` on the edited `python/src/*.cpp`.
- **If any C++ implementation under `src/` changed**: `ctest -L units` and `ctest -L integration`. Only
  these two. The system suite is never run (AGENTS.md). Do not run unfiltered `ctest`.
- Single-test debugging: `ctest -R <name>`.

## 8. Errors and accessors on the wrong node kind

Facts from the CSL code:
- The default `Abstract` accessors (`getIndex`, `getArgument`, `getValued`, `getNIndices`, ...) print to
  stdout with `print()` and throw a bare `CSLError` enum (`src/csl/abstract.cpp:212-217`,
  `src/csl/error.cpp:75-84`). The API must never print (api-conventions section 1, rule 7), so the facade must check
  the node kind before it calls such an accessor.
- Some accessors do not follow this pattern. `Abstract::getNum` returns 0 silently (`abstract.cpp:189-192`).
  A consistent policy needs a C++ change first.
- `getParent` returns a null `Parent` when there is none (`abstract.cpp:260-263`). This is a legitimate
  absence, not an error.

Options for the Python behaviour when an accessor does not apply to the node:

- (A) Return `None`. Simple, but it confuses "does not apply" with "no value", and hides mistakes.
- (B) Raise a typed error, `MartyTypeError` (derives from `TypeError`), with the message naming the node
  kind. Consistent with api-conventions section 1, rule 6 and section 2, rule 3. Callers check `kind` first.
- (C) Typed views per kind (`expr.as_sum()` returns a `SumView` or raises). Good static typing with mypy,
  but a larger surface and a second class hierarchy to maintain.

**Decided (C1: option B).** Accessors that do not apply raise a typed Python exception, based on the CSL
error types, with a `kind` property to check first. `None` is only for real absence (such as `getParent`).
Exceptions are raised at the Python boundary. The C++ inconsistencies (for example `getNum`) are not fixed
now and are tracked in [unported-features.md](unported-features.md).

## 9. Decisions

1. Modifier naming: **resolved**. No `with_*` and no `set_*` names, since setters are never ported
   (section 4, principle 5).
2. Accessor error model: **(B) decided**, typed exceptions (section 8).
3. Inconsistent CSL accessors (`getNum`): **not fixed now**, tracked in the unported list (section 8).
4. Callback algorithms: **decided**, Python duplicates for all, consistently (section 6).
5. Setters: **decided**, never ported (section 4, principle 5).
6. Whether the rules in sections 2 and 7 apply to CSL-only features too, or only to pymarty features. Open.
