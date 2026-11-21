Status: ACCEPTED (user, 2026-11-20). Phase 1 is accepted; the G3 record is below.
Updated: 2026-11-17
Author: orchestrator

# Phase 1 implementation log

## Packages

| Package | Title | Status | Implementer round | Date |
|---------|-------|--------|-------------------|------|
| WP0 | Two missing investigations (naming and duck typing; `Model::current`, `exit`/`abort`, `Show()`) | done (investigations, read-only) | 1 | 2026-11-17 |
| WP1a | `pow_s` special cases and exact perfect powers | done, verified by orchestrator | 1 | 2026-11-17 |
| WP1b | Remove stale declarations in `src/csl/simplification.h` | done, verified by orchestrator | 1 | 2026-11-17 |
| WP2 | Expr, numbers, symbols, operators, functions, errors | done, verified by orchestrator (usage example pending WP4) | 1 | 2026-11-17 |
| WP3 | Transforms and printing | partial: C++ and Python transforms and printing verified; golden set 36 of 50 (gap 14, accepted by user, see WP3 section) | 3 | 2026-11-18 |
| WP4a | C++ facade for spaces, indices and tensors, with C++ tests (no Python binding) | done: build clean; `ctest -L api` 81/81 after fix round | 4 | - |
| WP4 | Space, Index, Tensor (Python binding) | done: WP4b implemented and verified; `pytest python/tests -m "not slow"` 121 passed; `ctest -L api` 85/85; usage example passes (decisions in WP4b) | 4b | 2026-11-18 |
| WP4 fix | Null-space guard in `commonSpace` (open item from WP4 review, see "WP4 null-space fix") | done: `ctest -L api` 88/88; `ctest -L 'units|integration|api' -E system` 278/278 | 4 (fix) | 2026-11-18 |
| WP5 | Parity and port table | in progress: WP5a (parity and numeric checks) done and verified; WP5b (port table) done and verified; criterion 2 gate open (25 of 66 in scope ported, 38%) | 5a, 5b | - |
| WP6 | Stubs, doctests, usage harness, quality gates | todo | - | - |

## Work package WP0

Two read-only investigators (Haiku, one at a time):

- `01-investigation-naming-duck-typing.md`: no name validation in CSL (`src/csl/literal.cpp:665-720`); `Abstract::regularName` is not injective (`src/csl/abstract.cpp:97-123`); numeric entry points `int_s`, `float_s`, `intfraction_s`, `complex_s` (`src/csl/numerical.h:468-476`).
- `01-investigation-global-state.md`: `pow_s` raises SIGSEGV (`src/csl/operations.cpp:3161`, `:3170`); CSL errors throw (`src/csl/error.cpp:83`, `:95`); `Model::current` is a static pointer (`src/marty/core/model.h:66`); `errorStopsProgram` is declared and listed but not read (`src/csl/options.h:28`, `:49`); `Show()` is the only `system()` path (`src/marty/core/drawer.cpp:162-163`).

Not built, not run.

## Work package WP1a

Round 1, done (2026-11-17). Scope: spec sections 5.1, 5.2, 8 (WP1a), 9 (criteria 11).

Files changed:

- `src/csl/operations.cpp`: two static helpers (`checked_power`, `exact_perfect_power`); zero base with a negative numeric exponent raises `CSLError::ZeroDivisionError`; exact perfect powers of an integer base with a fraction exponent return an exact result; `pow_s` with an infinite base raises `CSLError::ValueError`; the two `std::raise(SIGSEGV)` blocks are removed.
- `tests/units/csl/test_pow_special.cpp` (new): six tests `PowInfinityBaseThrows`, `PowZeroBaseNegativeExponentThrows`, `SqrtPerfectSquare`, `PowPerfectPowerRational`, `PowNonPerfectUnchanged`, `PowNegativeBaseUnchanged`.
- `tests/units/csl/CMakeLists.txt`: one line, `ADD_UNIT_TEST(test_pow_special)`.

Decisions:

- The `SIGSEGV` and stderr dump are replaced by the exception message. The `CALL_SMERROR_SPEC` macro still prints `ValueError in pow_s (...)` to stderr.
- The zero-base check runs before the integer and fraction blocks. Before this change, `pow_s(0, -1/2)` returned through the fraction branch and raised nothing.
- If the perfect-power computation overflows, `checked_power` returns 0 and the existing path runs unchanged.
- `a == 1` is left to the existing path. Negative bases are unchanged.

Formatting: `clang-format --dry-run --Werror` on `src/csl/operations.cpp` reports one violation at line 2878. That line is pre-existing (it also fails on `HEAD`) and was left unchanged. The new code and `test_pow_special.cpp` pass the check.

Not run: the `system` suite (forbidden by `AGENTS.md`). `tests/system/data` contains no `Pow(` match (`grep -rn "Pow(" tests/system/data` returns nothing); the data layout was not inspected further.

## Work package WP1b

Round 1, done (2026-11-17). Scope: spec sections 5.3 (Q14), 8 (WP1b), 9 (criterion 12).

Files changed:

- `src/csl/simplification.h`: 13 function declarations and their Doxygen blocks removed. The `extern` variables (`RANDOM_SEED`, `NUMBER_OF_MUTATIONS`, `NATURAL_SELECTION`, `NUMBER_OF_GENERATION`, `GENERATION_BEFORE_SELECTION`, `SIMPLIFICATION_METHOD`, `MAX_ALTERNATE_FORMS`, `MAX_RECURSION_ALTERNATE`) are kept, as is the include guard and namespace close.

Removed declarations: `printVector`, `addAlternateForm`, `reduceAlternate`, `clearRedundancyAlternate`, `getRecursiveAlternateForms`, `internalRecursiveAlternateForms`, `Simplify`, `numberOfMutatingNodes`, `insertSortMutant`, `addMutants`, `getRandomMutation`, `naturalSelection`, `evolve`.

Grep evidence (word match, run from the repository root):

| Name | Command | HEAD header hits | Hits outside header (working tree, `src tests examples python`) |
|------|---------|------------------|------------------|
| `printVector` | `grep -rnw "printVector" src tests examples python` | 2 | 0 |
| `addAlternateForm` | same pattern | 2 | 0 |
| `reduceAlternate` | same pattern | 2 | 0 |
| `clearRedundancyAlternate` | same pattern | 2 | 0 |
| `getRecursiveAlternateForms` | same pattern | 3 | 0 (1 hit left: a doc comment in the header, see below) |
| `internalRecursiveAlternateForms` | same pattern | 4 | 0 |
| `Simplify` | same pattern | 2 | 13, all `sgl::Simplify` on `GExpr` in `src/marty/` (see below) |
| `numberOfMutatingNodes` | same pattern | 1 | 0 |
| `insertSortMutant` | same pattern | 1 | 0 |
| `addMutants` | same pattern | 1 | 0 |
| `getRandomMutation` | same pattern | 1 | 0 |
| `naturalSelection` | same pattern | 1 | 0 |
| `evolve` | same pattern | 1 | 0 |

Definition and caller check: `grep -rn "csl::Simplify\|Expr Simplify(\|Expr evolve(\|void printVector\|addAlternateForm(\|getRecursiveAlternateForms(\|naturalSelection(" src tests examples python`, excluding the header, returns 0 lines. `src/csl/simplification.cpp` defines none of the 13 names.

The 13 outside hits for `Simplify` are `sgl::Simplify(GExpr&, bool)` and its callers in `src/marty/core/sglsimplify.{cpp,h}`, `src/marty/core/diracology.cpp:320`, and `src/marty/sgl/simplify.{cpp,h}`. They belong to a different namespace and a different function, so `csl::Simplify` is not used anywhere.

Decisions:

- All 13 declarations have no definition and no caller in the repository, so removing them cannot break a working program. Removal was done only in `src/csl/simplification.h`.
- The `extern` variables are not in the stale list and were not part of this check. They are kept.
- The Doxygen comments on `MAX_ALTERNATE_FORMS` and `MAX_RECURSION_ALTERNATE` still mention `getRecursiveAlternateForms()`, which no longer exists in the header. They were left unchanged (not in the scope of the declaration removal). Recorded under Open problems.

Verification by the orchestrator (2026-11-17):

- `git diff -- src/csl/simplification.h`: only the 13 declaration blocks are removed (79 lines); the `extern` variables and include guard are unchanged.
- `grep -rnw` for the 12 csl names over `src tests examples python`, excluding the header: 0 hits. `Simplify` hits are all `sgl::Simplify` in `src/marty/` (different namespace; the build passes, so none resolves to a removed `csl::Simplify`).
- `cmake --build .` in `/tmp/pymarty-build`: exit 0, 0 `error:` and 0 `warning:` lines.
- `ctest -L 'units|integration|api'`: 220/220 passed.
- `clang-format --dry-run --Werror src/csl/simplification.h`: clean. HEAD had 4 violations, all inside the removed declarations.
- Note: the `cmake --build` from the implementer's background run and mine ran concurrently; the final foreground build is the one used as evidence.

## Work package WP2

Round 1, done (2026-11-17). Scope: spec section 6.2 up to functions, section 4, section 6.4 (`MartyZeroDivisionError`).

Files added or changed:

- `src/csl/api/expr.h`, `expr.cpp` (new): `Expr` handle, `Number`, `NumberKind`, `Shape`, arithmetic, `power()` via `csl::pow_s`, `CSL_I` and `CSL_INF` used as the global names.
- `src/csl/api/functions.h`, `functions.cpp` (new): function list taken from `mathFunctions.h`.
- `src/csl/api/error.h`: `ZeroDivisionError : public Error` added.
- `src/csl/CMakeLists.txt`: api sources added to `ALL_SOURCES`.
- `python/src/symbolic.cpp` (new), `python/src/module.cpp` (`bindSymbolic`, `MartyZeroDivisionError`, translator entry placed before the `mty::api::Error` catch), `python/CMakeLists.txt` (source added).
- `python/pymarty/symbolic.py`, `symbolic.pyi`, `errors.py`, `_pymarty.pyi`.
- Python tests: `python/tests/symbolic/test_numbers.py`, `test_operators.py`, `test_equality.py`, `test_errors_zero_division.py`.
- C++ tests: `tests/api/src/symbolic_numbers.cpp` (8 tests), `symbolic_symbols.cpp` (5 tests); `tests/api/CMakeLists.txt` registration.

Decisions and deviations:

- `ConstantAcceptsAnyName` replaces `ConstantNameValidation` (user decision, no name check, see Deviations and amendments).
- `IntegerArithmeticExact`: the assertion `power(2, 10) == 1024` was removed. CSL keeps `Integer^Integer` as `Pow` (`pow_s` folds only perfect powers with a fractional exponent, spec section 5.2). Core behaviour not changed.
- `CSLErrorMessageNotLost` checks that the message names the operation and the cause.
- `marty/api/common/error.h` is unchanged, so a core `csl` `ZeroDivisionError` that is not mapped to `MartyZeroDivisionError` still reaches Python as `MartyValueError`.
- Extra tests beyond the spec list: `OperationsLeaveGlobalOptionsUntouched` (C++), `test_expr_rejects_bool_and_str`, `test_expr_rejects_unconvertible_value` (Python).
- Hash tests (`HashSymbolsAndNumbers`, `NumberHashMatchesEquality`, `HashCompoundRaises`, `StructuralKeyOrderIndependent`) are Python-only. The structural key for an `Opaque` value is keyed by its type only.
- Python test files were renamed from `test_symbolic_*.py` to the spec names (content unchanged).
- `python/examples/usage/symbolic.py` was not edited. It imports `Minkowski` from `pymarty.symbolic`, which belongs to WP4 (`Space`); `test_usage_example_runs[symbolic]` fails with `ImportError` until then.

Formatting and lint:

- `clang-format --dry-run --Werror` passes on all WP2 C++ files. `python/src/module.cpp` passes; the WP2 lines were formatted, and HEAD was clean under the repository style.
- `src/csl/operations.cpp` line 2878 is a pre-existing violation (WP1a section); not touched.
- `ruff check` and `ruff format --check` pass on `python/pymarty` and `python/tests`.
- `mypy --strict` on `pymarty/symbolic.py`, `pymarty/errors.py`, `tests/symbolic`: no issues.

## Work package WP3

Rounds 1 to 3 (2026-11-18). Status: partial. Golden set 36 of 50, accepted by user decision (see below). Scope: spec section 6.2 (transforms and printing), section 8 (WP3).

Files added or changed:

- `src/csl/api/transform.h`, `transform.cpp` (new): `expanded`, `factored`, `factoredBy`, `collected`, `derived`, `replaced`, `evaluated`, `evaluatedByName`, `conjugated`, `realPart`, `imagPart`, `dependsOn`, `freeSymbols`, `symbolName`, `numericValue`, `text`, `latex`. Each transform works on a copy; the input is not mutated.
- `src/csl/CMakeLists.txt`: transform sources added to `ALL_SOURCES`.
- `python/src/symbolic.cpp`: `bindTransforms` (`expand`, `factor`, `collect`, `diff`, `subs`, `evaluate`, `conjugate`, `real_part`, `imag_part`, `depends_on`, `free_symbols`, `name`, `__str__`, `__repr__`, `to_latex`, `_repr_latex_`, `__complex__`, `__float__`, `__int__`).
- `python/pymarty/_pymarty.pyi`: `Expr` methods added.
- `tests/api/src/symbolic_transform.cpp` (17 tests), `tests/api/src/symbolic_printing.cpp` (2 tests; `CodeOutputStable` now pins the text and LaTeX literals `"(-3)*x + (x + y)^2\n"` and `"-3x + \\left(x + y\\right)^{2}"`); `tests/api/CMakeLists.txt` registration.
- Python tests: `python/tests/symbolic/test_transform.py` (25 tests), `python/tests/symbolic/test_printing.py` (7 tests).
- `python/tests/parity_exemptions.txt`: `SymbolicApi.SubsCompoundKeyBehaviourPinned` (dict keys must be hashable) and `SymbolicApi.CollectIndexedFactorThrows` (needs `Space` and `Index` from WP4).

Decisions and deviations:

- `expanded` is full (deep) expansion, `csl::Expanded(expr, true, false)`. The C++ default `csl::Expanded(e, false)` does not expand `(x+y)^2`, so the facade uses the deep form. Recorded as a deviation from the non-deep CSL default. User decision (2026-11-18): keep the deep default. A shallow default would fail the approved usage example (`python/examples/usage/symbolic.py` line 62). No `full` flag added.
- Printing is declared in `transform.h` (not a separate header), per the spec's list of WP3 functions.
- `kind` and `copy` are deferred (not part of the WP3 list in this implementation).
- CSL dependence semantics (disagreement with the spec's assumption, to be reported): a plain `variable` depends on other plain variables (`Variable::dependsOn` delegates to the parent). `DiffPolynomial` and `EvaluateWithFreeSymbolsKeepsThem` therefore model `y` as a `constant`. Results: `d/dx(2xy)` with `y` constant gives `2y`; `evaluate` keeps `y` free when `y` is a constant.
- `CollectIndexedFactorThrows` needs `Space` and `Index` (WP4) and is exempted. The C++ test uses the valid input `x*yi + z*yi` and expects `csl::api::TypeError`.
- `SubsCompoundKeyBehaviourPinned` is pinned in C++ only (`{x*y -> z}` on `x*y + w` gives `z + w`). Python dict keys must be hashable, and a compound `Expr` is unhashable, so it cannot be passed from Python. Exempted.
- `text()` output ends with a newline (CSL `print` appends one). `str(e)` returns `text()` unchanged, so it equals the C++ output. `repr(e)` strips trailing newlines before wrapping in `<Expr: ...>` (round 3, user decision 2026-11-18). Before/after for `(x + y)**2 - 3*x`: `repr` `'<Expr: (-3)*x + (x + y)^2\n>'` became `'<Expr: (-3)*x + (x + y)^2>'`; `str` unchanged.
- Error mapping, confirmed by the Python tests (none of these expectations was changed to pass): `name` on a compound expression raises `MartyTypeError`; `collect` with a string factor raises `TypeError`; ambiguous `evaluate` names raise `MartyValueError`.
- Golden set (`tests/api/data/symbolic/expressions.txt`): 36 of 50 expressions, LaTeX form, drawn from `tests/units/csl/test_latex.cpp` (source line recorded per entry in `tests/api/src/symbolic_golden.cpp`). Gap of 14: no `Abs` in the facade (blocks two cases), no complex or conjugate support, no `setCommutable`, no tensors or indices (WP4), mode-specific and `printLatex` outputs, and duplicate strings. Not padded. Generated once with `MARTY_UPDATE_GOLDEN=1`, verified line by line against the source. `SymbolicApi.GoldenExpressionsStable` passes. `pymarty.parity` fails on it until a Python counterpart or exemption is decided (see WP5).
- `python/tests/parity_exemptions.txt` (round 3, user decision 2026-11-18): temporary exemption `SymbolicApi.GoldenExpressionsStable`, reason "Python reader for the golden file lands in WP5; remove this exemption in WP5". Remove it in WP5.
- `clang-format -i` applied to `src/csl/api/transform.cpp`, `transform.h`, `tests/api/src/symbolic_transform.cpp` and `python/src/symbolic.cpp` (WP2/WP3 files) to clear format violations. Behaviour unchanged (rebuild and tests re-run). `src/csl/operations.cpp` line 2878 is pre-existing and not touched.

Not run: the `system` suite (forbidden by `AGENTS.md`).

## Work package WP4a

Round 4. Scope: C++ facade for spaces, indices and tensors, with C++ tests. No Python binding in this round.

Files added or changed:

- `src/csl/api/tensor.h`, `src/csl/api/tensor.cpp` (new): `IndexKey`, `SpaceHandle` (statics `minkowski`, `euclid2`, `euclid3`, `euclid4`; `index`, `indices`, `fixedIndex`), `IndexHandle`, `SymmetrySpec`, `TensorHandle` (`rank`, `operator()`), free functions `metric`, `delta`, `epsilon`, `relabelIndices`, `freeIndices`, `dummyIndices`. Handles own their space through `shared_ptr`. Facade pre-checks: symmetry conflicts on the same tensor pair, space identity, fixed-index range (1..127, the CSL `char` storage limit), epsilon dimension.
- `src/csl/CMakeLists.txt`: `api/tensor.cpp` and `api/tensor.h` added to `ALL_SOURCES`.
- `tests/api/src/symbolic_indices.cpp` (new, 7 tests): `IndexOutlivesScope`, `IndicesFromListAreDistinct`, `FixedIndexRange`, `NameAndValueAreTypeChecked`, `IndexHashIgnoresSign`, `IndexSpaceMismatchRaises`, `ExpressionKeepsSpaceAlive`.
- `tests/api/src/symbolic_tensors.cpp` (new, 12 tests): `RepeatedIndexContracts`, `SignedSpaceSameSignContractionRaises`, `MetricDeltaInvariant`, `EpsilonAntisymmetry`, `EpsilonDimensionRange`, `SymmetricTensorPair`, `AntisymmetricTensorPair`, `FullySymmetricTensor`, `ConflictingSymmetryRaises`, `ElementRejectsForeignIndex`, `RelabelIndicesCanonical`, `FreeAndDummyIndexQueries`.
- `tests/api/CMakeLists.txt`: `ADD_API_TEST(symbolic_indices)` and `ADD_API_TEST(symbolic_tensors)`.

Process note: the tests were written together with the implementation, not test-first.

Commands and results (run in `/tmp/pymarty-build`):

- `cmake . -DPython_EXECUTABLE=/tmp/pymarty-venv/bin/python`: `CONFIG_EXIT=0`.
- `cmake --build . --target symbolic_indices.x symbolic_tensors.x -j12`: `BUILD_EXIT=0`, 0 `error:`, 0 `warning:` (after one fix: a `[[nodiscard]]` return value discarded in `EXPECT_THROW` in `symbolic_indices.cpp`, now cast to `void`).
- `cmake --build . -j12` (full): `BUILD_EXIT=0`, 0 `error:`. One `warning:` line is the LTO linker note `lto-wrapper: using serial compilation of 7 LTRANS jobs`, not a compiler warning.
- `./tests/api/symbolic_indices.x`: 6 passed, 1 failed (`ExpressionKeepsSpaceAlive`).
- `./tests/api/symbolic_tensors.x`: 11 passed, 1 failed (`SignedSpaceSameSignContractionRaises`).
- `ctest -L api`: 80 of 82 passed. Failures: `SymbolicIndices.ExpressionKeepsSpaceAlive`, `SymbolicTensors.SignedSpaceSameSignContractionRaises`.
- `clang-format -i` on the four new files, then `clang-format --dry-run --Werror` on them: exit 0. The files are new, so the whole file was formatted.
- `ctest -L 'units|integration|api|python' -j12`: 271 of 274 passed. Failures: the two api tests above, and `pymarty.pytest` (`test_usage_example_runs[symbolic]`, `ImportError: cannot import name 'Minkowski' from 'pymarty.symbolic'`, expected: the Python binding is out of scope for this round). Python: 95 passed, 1 failed.
- `system` suite: not run (forbidden by `AGENTS.md`).

Failing tests (open, not fixed in this round):

- `SymbolicIndices.ExpressionKeepsSpaceAlive`: after the handles that own the space are released, `watch.expired()` is `true`, expected `false`. `ExprHandle` holds only `csl::Expr`, which refers to the space by raw pointer, so an expression does not keep its space alive. Per the plan, `ExprHandle` was not given an ownership mechanism. Design decision needed (see below).
- `SymbolicTensors.SignedSpaceSameSignContractionRaises`: `V({mu}) * W({mu})` on a signed space raises `RuntimeError` inside `csl::Index::testContraction` (`src/csl/index.cpp:234`). The guard maps only ZeroDivisionError, ValueError, TypeError and NotImplementedError (`src/csl/api/expr.cpp`, `throwCSLError`), so the error reaches the caller as base `csl::api::Error` with description `multiplication: internal error in the computer algebra system`. The test expects `ValueError`. Deviation from "through the guard". Options: map `RuntimeError` to `ValueError` in `throwCSLError` (broad, affects every RuntimeError), or add a facade pre-check in the product operator (more code). Not changed in this round.

Deviations and open points:

- Epsilon range: the implementation accepts 2 ≤ dim ≤ 4, which is what `buildEpsilon` (`src/csl/space.cpp:182`) defines. The brief said "< 4" in one place; the code follows `space.cpp`. Test `EpsilonDimensionRange` pins 2 ≤ dim ≤ 4.
- Fixed index values are limited to 1..127 by the CSL `char` storage. Values ≥ 128 are rejected before reaching CSL. A raw CSL reproduction of the wrap-around was not run; it is not reachable through the facade.
- `freeIndices` and `dummyIndices` return indices with a null owner. Open design point: whether they should share the owner of the tensor.
- CSL limits the number of names per space to 127 (name specs stored as `char`). Not guarded in the facade.
- Symmetry group-closure conflicts (for example symmetric on (0,1) with antisymmetric on (1,2)) are not detected. `TensorParent` silently overrides symmetry setters, so only direct conflicts on the same pair are pre-checked.
- C++ tests with no Python counterpart: all 19 new tests (the Python binding is WP4 later). `pymarty.parity` will need exemptions or Python counterparts when WP4 lands.
- No C++ or CSL source outside `src/csl/api/tensor.*` and `src/csl/CMakeLists.txt` was changed in this round. No reference data was edited.

## Verified by orchestrator (WP3)

2026-11-18, after rounds 1 to 3, by running the commands:

- Build in `/tmp/pymarty-build`: `BUILD_EXIT=0`, 0 `error:` lines.
- `ctest -L 'units|integration|api|python'`: 254 of 255 passed. Only failure: `pymarty.pytest` `test_usage_example_runs[symbolic]` (`ImportError: Minkowski`, WP4). `pymarty.parity` passes.
- `ctest -L api`: 63 of 63 passed, including `SymbolicApi.GoldenExpressionsStable`.
- `tests/api/data/symbolic/expressions.txt` exists with 36 expression lines.
- `python/src/symbolic.cpp` line 1 is the comment line (the stray `Y` is gone).
- Gaps still open for WP3: golden set 36 of 50 (accepted, see above).

## Verified by orchestrator

2026-11-17, WP0, by reading the cited lines:

- `numerical.h:468-476`: signatures of `float_s`, `int_s`, `intfraction_s`, `complex_s` match the report.
- `abstract.cpp:97-123`: `regularName` maps `+`, `-`, `~` and keeps `[` `]`, as reported.
- `literal.cpp:665-720`: `constant_s` and `variable_s` do not validate names, as reported.
- `operations.cpp:3161`, `:3170`: `std::raise(SIGSEGV)`, as reported.
- `model.h:66`: `inline static Model *current = nullptr;`, as reported.
- `options.h:28`, `:49`: `errorStopsProgram` declared and listed; the grep for `errorStopsProgram` under `src/` finds no reader.
- `csl.h:78` includes `librarytensor.h`; `librarytensor.h:81` has `exit(123)`. This contradicts `spike-c-error-paths.md` (which says the header is not compiled). Open problem below.

Not verified by the orchestrator (WP0): the runtime behaviour of the SIGSEGV paths, and whether `LibraryTensor` is reachable from the facade.

## Verified by orchestrator (WP2)

2026-11-18, by the orchestrator, after the user's edits to `expr.*`, `functions.cpp`, the symbolic tests and `python/src/symbolic.cpp`, `symbolic.py`, `symbolic.pyi`:

- Build: `cmake --build .` in `/tmp/pymarty-build` exits 0, 0 `error:` lines.
- `ctest -L 'units|integration|api|python'`: 234/235 passed. The one failure is `pymarty.pytest`: 62 passed, 1 failed (`test_usage_example_runs[symbolic]`, `ImportError: cannot import name 'Minkowski'`). Expected until WP4, accepted by the user on 2026-11-18 ("Yes, proceed").
- Zero-division mapping: `python/src/module.cpp` maps `csl::api::ZeroDivisionError` to `MartyZeroDivisionError`. Implementer's note, not verified by the orchestrator: a core CSL zero-division error that the facade does not convert reaches Python as `MartyValueError` (`marty/api/common/error.h` unchanged). No WP2 test covers this path.

## Verified by orchestrator (WP1a)

2026-11-17, by the orchestrator (diff, build, tests, format):

- `git diff`: `src/csl/operations.cpp` changes are limited to `pow_s` and two new static helpers placed above it; the other diff hunks are the two `phase-1` headers and the one-line CMake registration. Scope matches spec sections 5.1 and 5.2.
- Build: `cmake --build .` in `/tmp/pymarty-build` succeeds, 0 `warning:` lines.
- `ctest -L 'units|integration'`: 190/190 passed. `ctest -L api`: 30/30 passed. `ctest -L 'units|integration|api|python'`: 222/222 passed.
- `test_pow_special.x`: 6/6 passed. `ctest -R 'csl_pow_special|Pow|Sqrt'`: passed.
- `tests/system/data`: `grep -rln "Pow("` returns nothing.
- Format: `clang-format --dry-run --Werror` on `src/csl/operations.cpp` reports one violation at line 2878. The same violation exists in `HEAD` (checked with `git show HEAD:src/csl/operations.cpp | clang-format --assume-filename=...`). Not introduced by WP1a.
- Count: the 216 baseline equals the 222 total minus the six new tests, so the baseline covered the four labels `units|integration|api|python`. The earlier 190 and 220 figures are per-regex subsets, not a discrepancy.

Not verified by the orchestrator (WP0): the runtime behaviour of the SIGSEGV paths, and whether `LibraryTensor` is reachable from the facade.

## Deviations and amendments

- 2026-11-17, orchestrator, WP1a test file: the implementer's zero-base and infinity-base tests used `ASSERT_THROW(..., CSLError)`, which does not check the error kind required by spec section 5.1. I rewrote the two tests (`PowInfinityBaseThrows`, `PowZeroBaseNegativeExponentThrows`) with a file-local helper `expectCSLError` that asserts `CSLError::ValueError` and `CSLError::ZeroDivisionError`. Re-ran the build and tests (6/6, 222/222 with the python label). Ran `clang-format -i` on the file. No change to `src/`.
- 2026-11-17, user decision (asked by the orchestrator): no name validation for `constant` and `variable`. Spec section 2 row for `constant`, `variable` changed from `ValueError` for an invalid name to no name check, matching CSL (`src/csl/literal.cpp:665-720`). WP2 must not add a name check.
- 2026-11-17, user decision: section 7 "Verified by orchestrator" removed from `01-investigation-transforms-2.md`, `01-investigation-transforms.md` and `01-investigation-indices-tensors.md`. The "Not verified" section of `01-investigation-transforms-2.md` is kept.

## Open problems

- Known tensions carried from the spec: `x/0` gives `oo` (with a stderr warning) while `0**-1` raises (Q2, Q16, Q17); `Integer::operator==` int-cast limitation (`src/csl/numerical.cpp:338`).
- Resolved 2026-11-17 (user): constant and variable names are not checked (see Deviations and amendments).
- `csl::option::errorStopsProgram` is declared (`src/csl/options.h:28`) and has no reader in `src/`. Not used by the facade.
- `src/csl/csl.h:78` includes `librarytensor.h`, which calls `exit(123)` (`src/csl/librarytensor.h:81`). `spike-c-error-paths.md` says the header is not compiled. Reachability from the facade is unverified.
- The terminal history of this workspace shows `ctest -L system` runs (in `/tmp/pymarty-build` and `/tmp/pymarty-baseline-build`), which the standing rules forbid. The author of these runs is not recorded here; they were stopped by `pkill` in the history. Results are not used as evidence. The user should confirm that no system-suite output is needed for the phase.
- Closed 2026-11-18 (see "WP4 null-space fix"): `csl::api::metric`, `delta` and `epsilon` with null-space indices segfaulted through `csl::GetMetric(nullptr)`; now raise `ValueError` in `commonSpace`.
- Still open (WP4 null-space, not fixed): `csl::Index::setValue(char)` on a null-space index segfaults (`src/csl/index.cpp:114-123`). Reachable from direct C++ CSL only: `src/csl/api/` has no `setValue` call (grep), `SpaceHandle::fixedIndex` range-checks 0..127 before constructing the index, and `IndexHandle` has no setter. Not fixed in WP4 fix.

## Evidence table

| Criterion | Command or test | Result | Date |
|-----------|-----------------|--------|------|
| Baseline | `ctest -L 'units|integration'` in `/tmp/pymarty-build` | 216/216 passed (`/tmp/p1-baseline.log`) | 2026-11-17 |
| WP1a build | `cmake --build . -j12` in `/tmp/pymarty-build` | success, no warnings from `marty` | 2026-11-17 |
| WP1a unit tests | `./tests/units/csl/test_pow_special.x` | 6/6 passed | 2026-11-17 |
| WP1a Pow/Sqrt | `ctest -R 'Pow|Sqrt'` | 12/12 passed | 2026-11-17 |
| WP1a labels | `ctest -L 'units|integration|api'` | 220/220 passed (units 144, integration 46, api 30) | 2026-11-17 |
| WP1a format | `clang-format --dry-run --Werror` on changed files | new test file clean; `operations.cpp` line 2878 is pre-existing (same on `HEAD`) | 2026-11-17 |
| WP1a orchestrator | `cmake --build`; `ctest -L 'units|integration|api|python'` | 222/222 passed (baseline 216 + 6 new) | 2026-11-17 |
| WP1b build | `cmake --build .` in `/tmp/pymarty-build` | exit 0, 0 `error:` lines; 2 `warning:` lines are LTO linker notes (`lto-wrapper: using serial compilation`), not compiler warnings | 2026-11-17 |
| WP1b tests | `ctest -L 'units|integration|api'` | 220/220 passed | 2026-11-17 |
| WP1b format | `clang-format --dry-run --Werror src/csl/simplification.h` | exit 0 (HEAD had 4 violations, inside the removed declarations) | 2026-11-17 |
| WP1b names | `grep -rnw` over `src tests examples python`, excluding the header | 0 hits for the 12 csl names; `Simplify` hits are `sgl::Simplify` only | 2026-11-17 |
| WP2 build | `cmake --build .` in `/tmp/pymarty-build` | exit 0, 0 `warning:` from compiler (one LTO linker note) | 2026-11-17 |
| WP2 api | `ctest -L api` | 43/43 passed (30 before WP2, plus 13 new) | 2026-11-17 |
| WP2 units/integration/api | `ctest -L 'units|integration|api'` (before the Python test rename) | 233/233 passed (units 144, integration 46, api 43) | 2026-11-17 |
| WP2 python symbolic | `pytest tests/symbolic -k 'numbers or operators or equality or zero_division' -m 'not slow'` (from `/tmp/pymarty-build/python`) | 23 passed | 2026-11-17 |
| WP2 parity | `python/tools/check_parity.py` | OK: 43 C++ API tests, 22 with Python counterpart, 21 exempted | 2026-11-17 |
| WP2 final labels | `ctest -L 'units|integration|api|python'` | 234/235 passed. The failure is `pymarty.pytest`: 62 Python tests pass, 1 fails (`test_usage_example_runs[symbolic]`, `ImportError: Minkowski`, expected until WP4) | 2026-11-17 |
| WP3 C++ transforms | `symbolic_transform.x` | 17/17 passed | 2026-11-18 |
| WP3 C++ printing | `symbolic_printing.x` (with text and LaTeX literals) | 2/2 passed | 2026-11-18 |
| WP3 Python | `pytest tests/symbolic/test_transform.py tests/symbolic/test_printing.py -m 'not slow'` (from `/tmp/pymarty-build/python`, source test paths) | 33/33 passed | 2026-11-18 |
| WP3 labels | `ctest -L 'units|integration|api|python' -E system` | 253/254 passed. Only failure: `pymarty.pytest` `test_usage_example_runs[symbolic]` (`ImportError: Minkowski`, WP4). Python: 95 passed, 1 failed | 2026-11-18 |
| WP3 parity | `python/tools/check_parity.py` | OK: 62 C++ API tests, 39 with Python counterpart, 23 exempted | 2026-11-18 |
| WP3 quality | `ruff check`, `ruff format --check`, `mypy --strict` (`tests pymarty tools`) | all pass | 2026-11-18 |
| WP3 format | `clang-format --dry-run --Werror` on the five WP3 C++ files | exit 0 after `clang-format -i` | 2026-11-18 |
| WP4 fix build | `cmake --build . --target symbolic_indices.x -j12` in `/tmp/pymarty-build` | exit 0, 0 `error:`, 0 `warning:` (`/tmp/wp4-guard-build.log`) | 2026-11-18 |
| WP4 fix new tests | `ctest -R "^NullSpaceIndices\."` | 3/3 passed | 2026-11-18 |
| WP4 fix api | `ctest -L api -j8` | 88/88 passed (85 before + 3 new) | 2026-11-18 |
| WP4 fix format | `clang-format --dry-run --Werror` on `src/csl/api/tensor.cpp` and `tests/api/src/symbolic_indices.cpp` | exit 0 | 2026-11-18 |
| WP4 fix full build | `cmake --build . -j12` in `/tmp/pymarty-build` | exit 0, 0 `error:`; 1 `warning:` line is the LTO linker note (`/tmp/wp4-guard-full.log`) | 2026-11-18 |
| WP4 fix labels | `ctest -L 'units|integration|api' -j12 -E system` | 278/278 passed (`/tmp/wp4-guard-ctest.log`) | 2026-11-18 |
| WP5a golden printing | `pytest tests/symbolic/test_parity.py -k printing -m 'not slow'` (`PYTHONPATH=/tmp/pymarty-build/python`) | 2/2 passed; 36 of 36 golden lines mapped | - |
| WP5a numeric | `pytest tests/symbolic/test_parity.py -k numeric -m 'not slow'` | 15/15 passed (12 real, 3 complex; tolerance 1e-12) | - |
| WP5a collect port | `pytest tests/symbolic/test_transform.py` | passes, includes `test_collect_indexed_factor_raises` | - |
| WP5a python suite | `pytest tests -m 'not slow'` | 139 passed, 0 failed | - |
| WP5a mypy | `mypy --strict pymarty tests tools` | Success: no issues found in 22 source files | - |
| WP5a parity | `python/tools/check_parity.py` | OK: 63 C++ API tests, 41 with a Python counterpart, 22 exempted | - |

Count note: the baseline's 216 covers the labels `units|integration|api|python`. The current total for those labels is 222, which is 216 plus the six new tests. The earlier per-regex counts (190 for `units|integration`, 30 for `api`) are subsets and do not conflict with this.

### Decisions and fix round (WP4a)

User decisions (askQuestions):

- Q4 amended for expressions: `ExprHandle` does not keep its space alive. Spec `03-spec.md` (Q4 row and section 6.3 note) amended. Test `SymbolicIndices.ExpressionKeepsSpaceAlive` removed. Spaces, indices and tensor handles keep their owner.
- `RuntimeError` is mapped to `ValueError` globally in `throwCSLError` (`src/csl/api/expr.cpp`, lines 39-41). This resolves `SymbolicTensors.SignedSpaceSameSignContractionRaises`.
- `freeIndices` and `dummyIndices` null-owner indices: kept as a documented limitation.

Verified by orchestrator (WP4a, after fix round):

- `cmake --build . -j12` in `/tmp/pymarty-build`: `BUILD_EXIT=0`, 0 `error:` lines.
- `ctest -L api`: 100% passed, 81 of 81.
- Grep confirms the `RuntimeError` case in `expr.cpp` and no remaining `ExpressionKeepsSpaceAlive` in `symbolic_indices.cpp`.
- Known leftovers: unused `TensorHandle` and `<memory>` includes in `symbolic_indices.cpp` (not removed; to clean up in WP6 quality gates).

### Work package WP4b (Python binding)

Implementer (Haiku): added `index()` and `indices(count)` without a name to the facade (`src/csl/api/tensor.*`), a check that a tensor cannot be fully symmetric and fully antisymmetric, the Python binding (`python/src/symbolic.cpp`), exports and typing (`symbolic.py`, `symbolic.pyi`, `_pymarty.pyi`), and `python/tests/symbolic/test_indices.py` and `test_tensors.py`.

Deviations reported by the implementer (to confirm):

- `DEFAULT_INDEX_NAMES = ("i", "j", "k", "l")`, taken from CSL, not the six names proposed in the spec.
- `Index.space` is `None` for indices returned by `free_indices` and `dummy_indices` (null owner, see WP4a).

Verified by orchestrator (WP4b), 2026-11-18:

- Build in `/tmp/pymarty-build`: `BUILD_EXIT=0`, 0 `error:` lines.
- `ctest -L api`: 85 of 85 passed.
- `pytest python/tests/symbolic/test_indices.py python/tests/symbolic/test_tensors.py -m "not slow"`: 25 passed.
- `pytest python/tests -m "not slow"`: 120 passed, 1 failed (`test_usage_example_runs[symbolic]`).
- `ctest -L 'units|integration|api|python'`: 276 of 277 passed. Only failure: `pymarty.pytest`.
- Usage example failure: `df/dx = 3*x^2 + 2*y + 2*x*d/d(x)(y)`; the assert `(x**3 + 2*x*y).diff(x) == 3*x**2 + 2*y` fails. Cause: `ExprHandle::variable` (`src/csl/api/expr.cpp:98`) calls `csl::variable_s`, which creates a non-elementary `VariableParent`. CSL then treats `y` as dependent on `x` (`src/csl/literal.cpp:495`). Decision (user, 2026-11-18): option B, change the example. Applied as a one-line change at `python/examples/usage/symbolic.py` line 38 (`y = constant("y")`, was `variable`), the same workaround as WP3 `test_diff_polynomial`. The `diff` assertion then passes. This edit is the user's approved decision; no other part of the example was changed.
- Usage example decisions (user, 2026-11-18):
  - `str()` has no trailing newline: `__str__` strips it, as `__repr__` does. The WP3 tests that pinned the newline (`test_code_output_stable`, `test_str_returns_text_output`, `test_repr_wraps_text_output`) were updated to match. The C++ `text()` output is unchanged, so the parity entry `SymbolicApi.CodeOutputStable` is unaffected.
  - Metric LaTeX uses lower indices, as CSL does: `g_{\mu\nu}`. The example line 260 was changed from `g^{\mu\nu}` to `g_{\mu\nu}`.
- Status: `pytest python/tests -m "not slow"`: 121 passed, 0 failed. `ctest -L api`: 85/85. Usage example `test_usage_example_runs[symbolic]` passes.
- Gates: `check_parity.py` OK (63 C++ API tests, 39 with a Python counterpart, 24 exempted); `ruff check`, `ruff format --check`, `mypy --strict` (`tests pymarty tools`) pass; `clang-format --dry-run --Werror` on the WP4a/WP4b C++ files and `python/src/symbolic.cpp` passes.
- Pending for WP5: the parity exemption count grew from 23 to 24. The new exemption is not yet explained in the log; check `python/tests/parity_exemptions.txt`.

### WP4 null-space reproduction

Implementer round, WP4 open item (`03-spec.md`, WP4 note). Scope: reproduce only, no fix in `src/csl` or `src/csl/api`.

Method: a temporary `TEST(NullSpaceReproduction, ...)` block was appended to `tests/api/src/symbolic_indices.cpp`, built, and each case was run alone (a segfault kills the whole binary). The file was then restored from the backup; md5 `c8101a1507dedf8a4d20c122131f7a47`, unchanged. No test code is kept.

| Case | Path | Outcome |
|------|------|---------|
| `NullSpaceTensorCall` | `tensor({null index})` on a 1-space tensor | PASS (ValueError). Not reproduced: the facade pre-check in `TensorHandle::operator()` (`src/csl/api/tensor.cpp`) throws before any CSL call. |
| `NullSpaceMetric` | `csl::api::metric(null, null)` | SEGFAULT. Reproduced: `commonSpace` returns nullptr (no mismatch), then `csl::GetMetric(nullptr)` (`src/csl/interface.cpp:466`) calls `Space::getMetric()` on null. Reachable from the facade. |
| `NullSpaceIndexSetValueNegative` | `csl::Index idx; idx.setValue(-1)` | SEGFAULT. Reproduced in `csl::Index::setValue(char)` (`src/csl/index.cpp:114-123`, error branch prints `space->getName()`). Direct C++ only. |

Commands and output tails:

- `cd /tmp/pymarty-build && cmake --build . --target symbolic_indices.x -j12`: first attempt failed on my test code (`csl::CSLError` is global, not in `csl`); fixed in the test block. Second attempt: `BUILD_EXIT=0`, 0 `error:` lines.
- `ctest -N -R NullSpaceReproduction`: tests #262, #263, #264, `Total Tests: 3`.
- `timeout 120 ctest -R '^NullSpaceReproduction\.NullSpaceTensorCall$' --output-on-failure --timeout 60`: `1/1 Test #262: NullSpaceReproduction.NullSpaceTensorCall ... Passed 0.01 sec`, `100% tests passed, 0 tests failed out of 1`.
- Same with `NullSpaceMetric`: `1/1 Test #263: NullSpaceReproduction.NullSpaceMetric ...***Exception: SegFault 0.01 sec`, `263 - NullSpaceReproduction.NullSpaceMetric (SEGFAULT)`, exit 8.
- Same with `NullSpaceIndexSetValueNegative`: `264 - NullSpaceReproduction.NullSpaceIndexSetValueNegative (SEGFAULT)`, exit 8.
- `gdb -batch -ex run -ex bt --args ./tests/api/symbolic_indices.x --gtest_filter=NullSpaceReproduction.<case>`:
  - Metric: `#0 csl::Space::getMetric() const`, `#1 csl::GetMetric(csl::Space const*)`, `#2 csl::api::metric(...)`, `#3 NullSpaceReproduction_NullSpaceMetric_Test::TestBody()`.
  - SetValue: `#0 csl::Index::setValue(char)`, `#1 NullSpaceReproduction_NullSpaceIndexSetValueNegative_Test::TestBody()`.
  The library has no line info; the source attribution comes from reading the code.

Not done or not verified:

- `csl::api::delta(null, null)` not run (same `commonSpace` shape, so likely the same crash in `csl::GetDelta`; unverified).
- Negative `Index(char)` through the facade not checked.
- Null-owner indices from `freeIndices`/`dummyIndices` into `TensorParent::checkIndexRequest` (`src/csl/indicial.cpp:1558-1581`) not exercised.
- No fix made (not requested this round). The `NullSpaceMetric` path is reachable from the facade, so the spec's fix condition is met; the fix is for the next round, pending the user's decision.
- No changes in `src/csl` or `src/csl/api` in this round; earlier uncommitted changes untouched.

### WP4 null-space fix

Implementer round 4 (fix). Scope: the open item from "WP4 null-space reproduction" for `csl::api::metric` (reachable from the facade). Fix limited to `commonSpace` in the facade, plus tests. No change to CSL core.

Files changed:

- `src/csl/api/tensor.cpp`, `commonSpace` (about lines 104-116): if the first index has no space (`getSpace() == nullptr`), throw `ValueError("<what>: index has no space")` before any CSL call. The existing mismatch loop is unchanged. The doc comment now also says it raises `ValueError` when the indices have no space. `metric`, `delta` and `epsilon` all go through `commonSpace`, so all three are covered. `epsilon` checks for an empty vector first, as before.
- `tests/api/src/symbolic_indices.cpp`: suite `NullSpaceIndices`, three tests appended. The existing 7 tests are unchanged. `MetricWithoutSpaceRaises` and `DeltaWithoutSpaceRaises` use two `IndexHandle(nullptr, csl::Index())`; `EpsilonWithoutSpaceRaises` uses three. Each uses `EXPECT_THROW(static_cast<void>(...), ValueError)` because the functions are `[[nodiscard]]`.
- `docs/architecture/pymarty/phase-1/04-implementation-log.md`: this section, the Packages row, the Evidence rows, and the Open problems entry.

Decisions:

- The guard is in `commonSpace`, not in `csl::GetMetric`/`GetDelta`/`GetEpsilon`, so the fix stays in the facade (the layer that owns the user-facing error type).
- `delta(null, null)` was not reproduced before the fix. It goes through the same `commonSpace` path and is covered by `DeltaWithoutSpaceRaises`, which passes after the fix.
- Not changed: `csl::Index::setValue(char)` with a null space (`src/csl/index.cpp:114-123`). It is not reachable from the facade (see Open problems).

Commands and results (in `/tmp/pymarty-build`):

- `cmake --build . -j12 --target symbolic_indices.x`: `BUILD_EXIT=0`, 0 `error:`, 0 `warning:`. Tail: `[100%] Built target symbolic_indices.x`.
- `ctest -R "^NullSpaceIndices\." --output-on-failure`: 3/3 passed (`MetricWithoutSpaceRaises`, `DeltaWithoutSpaceRaises`, `EpsilonWithoutSpaceRaises`), 0.01 s each.
- `ctest -L api -j8`: `100% tests passed, 0 tests failed out of 88`.
- `clang-format --dry-run --Werror src/csl/api/tensor.cpp tests/api/src/symbolic_indices.cpp`: exit 0. The `diff` check against `clang-format` output showed no differences in either file.
- `cmake --build . -j12` (full): `BUILD_EXIT=0`, 0 `error:`. One `warning:` line is `lto-wrapper: using serial compilation of 8 LTRANS jobs` (linker note, not a compiler warning).
- `ctest -L 'units|integration|api' -j12 -E system`: `100% tests passed, 0 tests failed out of 278`.

Not run: the `system` suite (forbidden by `AGENTS.md`). Not verified: the facade path for a negative `Index(char)`, and null-owner indices from `freeIndices`/`dummyIndices` into `TensorParent::checkIndexRequest` (`src/csl/indicial.cpp:1558-1581`).

## Work package WP5a parity and numeric

Round 5, implementer. Status: WP5a done and verified. WP5 overall is in progress (WP5b, the port table, not started). Spec criteria 1, 2 and 3 are not claimed as met by this section.

Files changed:

- `python/tests/symbolic/test_parity.py` (new):
  - `GOLDEN_BUILDERS`: 36 builders, one per golden line of `tests/api/data/symbolic/expressions.txt`, in golden order (mirrors `goldenExpressions()` in `tests/api/src/symbolic_golden.cpp`).
  - `test_golden_printing_builders_match_golden_lines` (count of builders equals count of golden lines).
  - `test_golden_expressions_printing_matches_cpp` with `@pytest.mark.parity("SymbolicApi.GoldenExpressionsStable")`: `to_latex()` of each builder equals its golden line (`zip(..., strict=True)`).
  - `test_numeric_real_values_match_python_math` (12 cases) and `test_numeric_complex_values_match_python_math` (3 cases), relative tolerance 1e-12.
- `python/tests/symbolic/test_transform.py`: `test_collect_indexed_factor_raises` added with `@pytest.mark.parity("SymbolicApi.CollectIndexedFactorThrows")`. Uses `Space`, `tensor`, `index` and `MartyTypeError` (already imported). Passes on the first run.
- `python/tests/parity_exemptions.txt`: two lines removed, `SymbolicApi.GoldenExpressionsStable` (its Python test now exists) and `SymbolicApi.CollectIndexedFactorThrows` (ported; the stale reason "needs csl Space and Index" no longer applies).
- No change to `src/`, `tests/api/`, the golden file, the spec, or `python/examples/usage/symbolic.py`.

Decisions:

- Numeric reference: Python `math` (plus `fractions` for exact rationals and built-in `complex`), not the C++ library. No numeric golden file exists. The cases cover `sqrt`, `exp`, `sin`, `cos`, `tan`, `log`, `asin`, products, rational arithmetic and complex values (`i**2`, `(1+2i)*(3-i)`, `exp(i)`).
- Printing criterion (3) needs 50 expressions. The golden file has 36, so criterion 3 is open (the 14-expression gap accepted in WP3 still applies). The printing test checks what exists; it does not claim 50.
- Parity count: `check_parity.py` moved from 39 with a counterpart and 24 exempted (log after WP4b) to 41 and 22. The two removed exemption lines each had no `@pytest.mark.parity` marker, so each was counted as exempted. Adding the markers moves them to the counterpart count, giving 24 - 2 = 22 exempted and 39 + 2 = 41 with a counterpart (63 total unchanged). The 22 remaining exempted C++ IDs are the three `ErrorApi.*` entries, the `SessionApi.*` and `TerminatingCallsApi.*` wildcard expansions, and `SymbolicApi.SubsCompoundKeyBehaviourPinned`. The WP3-to-WP4b change from 23 to 24 was not traced in this round.

Verification (`PYTHONPATH=/tmp/pymarty-build/python`, run from `/tmp` with `-c python/pyproject.toml --rootdir python`, because `python -m pytest` from `python/` imports the source `pymarty` package, which lacks the built `_pymarty` module):

- `pytest tests/symbolic/test_parity.py -k printing -m "not slow"`: `2 passed, 15 deselected`.
- `pytest tests/symbolic/test_parity.py -k numeric -m "not slow"`: `15 passed, 2 deselected`.
- `pytest tests/symbolic/test_parity.py tests/symbolic/test_transform.py -m "not slow"`: `45 passed`.
- `pytest tests -m "not slow"`: `139 passed`.
- `mypy --strict pymarty tests tools`: `Success: no issues found in 22 source files`.
- `ruff format --check .`: `25 files already formatted`.
- `ruff check .`: 1 error, `examples/usage/symbolic.py:15:1: I001 Import block is un-sorted`. This file is not part of WP5a (approved example, frozen; the import block is not edited here). `tests/symbolic/test_parity.py` had a similar I001 error, fixed with `ruff check --fix` on that file only.
- `python/tools/check_parity.py`: `parity: OK, 63 C++ API tests: 41 with a Python counterpart, 22 exempted`, exit 0.
- Build: `cmake --build . --target _pymarty` in `/tmp/pymarty-build`: exit 0, 0 `error:`.

Not run: `ctest -L api` and the C++ suites (no C++ change in this round); the `system` suite (forbidden by `AGENTS.md`). Not verified: criteria 1, 2 and 3 of `03-spec.md` section 9.

## WP5b port table

Round 5, implementer. Status: WP5b table done and verified. WP5 is not done: criterion 2 is open (see below). Source: section 6 of `01-investigation-tests-and-wiring.md`, each row checked against the C++ test body and the Python test body. Table: [port-table.md](port-table.md).

Files changed:

- `docs/architecture/pymarty/phase-1/port-table.md` (new): 133 rows, one per C++ unit test in `tests/units/csl/`, with class, status and destination. Corrections to the investigation are listed at its end.
- `python/tests/symbolic/test_port_table.py` (new): 5 tests and 1 strict xfail. Checks row count and order, that each C++ `TEST(suite, name)` exists, per-file counts, class and status totals, and that each ported destination is an existing Python test function. The xfail is criterion 2 (below 90%).
- This log: WP5 row updated to say WP5b is done. Nothing else changed. No Python code, golden file, `tests/units/`, or `src/` change.

Counts (from the table, verified by awk and by the new test):

- Rows: 133 (`test_equality.cpp` 5, `test_literals.cpp` 2, `test_numbers.cpp` 19, `test_latex.cpp` 63, `test_relabel_indices.cpp` 25, `test_signed_refresh.cpp` 5, `test_metric_delta_invariant.cpp` 7, `test_init_sanitizer.cpp` 7).
- Classes: P 54, P+ 55, L1 13, X 11 (sum 133). These match the investigation's stated totals once rows 37 and 66 are read correctly (see corrections).
- Statuses: ported 25, gap 41, deferred to phase 2 (A1 to A4) 44, covered by L1 only 10, n/a 13 (sum 133).
- Criterion 2 ratio: ported / (ported + gap) = 25 / 66 = 37.9%, below the 90% gate. The gate is not met.

Gap list (41 rows, by row number in the table): 1, 2, 3, 5, 6, 7, 10, 11, 13, 14, 15, 16, 17, 18, 20, 22, 23, 24, 26, 42, 66, 73, 87, 90 to 101, 103, 104, 107, 108, 109, 111. Each gap row names the missing assertion or the missing feature (abs, arg, pi, e, printing mode, LaTeX index names).

Decisions and rules applied:

- A row is `ported` only if a named Python test checks the same behaviour. Partial coverage is a `gap`, not a port. Gaps are reported, not filled with new Python tests.
- `integer_constants` and `expr_conv` were downgraded to gap after reading the Python bodies (exact values of `CSL_0`, `CSL_1`, `CSL_2`, `CSL_M_2` and `Expr(nullptr)` are not asserted).
- A1 to A4 rows are `deferred to phase 2` per spec 10.2, including the investigation's P+ labels for those areas. A9 is later. A11 is out of scope (`n/a`).
- `csl_numbers.errors` is `n/a`: the C++ test calls `addition_own` with a symbol, which the Python operators do not reach.
- `LatexNonCommuting.inversePowerOfCommutingFactorGoesToDenominator` is P, not P+ (no non-commuting symbol).
- `initSanitizer.operations` and `initSanitizer.math_function` were missing from investigation section 6.8. Added as `n/a` (internal).
- Investigation row 40 had `|a|` in its description, which broke the markdown table. The port table has no description column.

Open decisions for the user:

- Expose printing mode 2 (`bracketsDependOnMode`) in `to_latex()`? Currently `n/a`.
- Add `abs` and `arg` (A8) to the facade? Blocks the literal, function and composite gaps.
- Add `pi` and `e` to the facade? Blocks `float_constants`.
- Keep `csl_numbers.errors` as `n/a`?

Verification (`PYTHONPATH=/tmp/pymarty-build/python`, run from `/tmp` with `-c python/pyproject.toml --rootdir python`):

- `pytest tests/symbolic/test_port_table.py`: `5 passed, 1 xfailed`. The xfail is the criterion 2 gate (strict).
- `pytest tests -m "not slow"`: `144 passed, 1 xfailed` (was 139 passed before this round; +5 passed, +1 xfailed from the new file).
- `mypy --strict pymarty tests tools`: `Success: no issues found in 23 source files`.
- `ruff format --check .`: `26 files already formatted` (after formatting the new test file once).
- `ruff check tests tools pymarty`: `All checks passed!`. The pre-existing I001 in `examples/usage/symbolic.py:15` is outside these paths and was not edited.
- `python/tools/check_parity.py`: `parity: OK, 63 C++ API tests: 41 with a Python counterpart, 22 exempted`. Unchanged: the parity tool ignores CSL unit tests.
- Test discovery: the tests run from the source tree (`python/tests/`), so the new test file was picked up without a rebuild.

Not run: C++ suites (no C++ change); `system` suite (forbidden by `AGENTS.md`).

Not claimed: spec criteria 1, 2 and 3 of `03-spec.md` section 9. WP5 stays in progress until criterion 2 is met or the spec's denominator is re-agreed.

## Later round: gap closure, golden 50, modulus fix

Superseded status for the port table above. Verified figures for the current state:

- Port table (`port-table.md`): 133 rows; ported 61, gap 5 (rows 6, 7, 20, 87, 104), deferred 44, covered 10, n/a 13; in-scope 66. Criterion 2 ratio 61 / 66 = 0.924 (gate 90%).
- Golden expressions (`tests/api/src/symbolic_golden.cpp`): 50 entries, matching `tests/api/data/symbolic/expressions.txt`. `SymbolicApi.GoldenExpressionsStable` passes. Python parity builders in `python/tests/symbolic/test_parity.py` mirror the 50 entries; `test_golden_expressions_printing_matches_cpp` passes.
- CSL core: modulus of a negative real returns its absolute value (`src/csl/numerical.h`, `src/csl/numerical.cpp`). Facade not changed.
- Parity tests added for `PiAndEConstants`, `ModulusOfComplex`, `ModulusOfRealIsAbsoluteValue`, `ArgumentOfComplex`, `AbsoluteValue` (`python/tests/symbolic/test_numbers.py`).

Verification:

- `ctest -L 'units|integration|api' -E system`: 289 of 289 passed (after full rebuild of `/tmp/pymarty-build`).
- `pytest python/tests -m "not slow"`: 190 passed.
- `tools/check_parity.py`: `parity: OK, 68 C++ API tests: 46 with a Python counterpart, 22 exempted`.
- Doctests of `pymarty/symbolic.py` (run against the build tree): 3 attempted, 0 failed. Usage example `test_usage_examples.py -k symbolic`: 1 passed.
- `ruff check`, `ruff format --check`, `mypy --strict pymarty tests tools`, and `mypy --strict pymarty/symbolic.pyi`: clean.
- `clang-format --dry-run --Werror` on `symbolic_golden.cpp` and `symbolic_numbers.cpp`: clean.

Known limitations (not fixed, to report):

- Port-table gaps: row 104 (no derivative-index API); rows 6, 7, 20, 87 (complex-property symbols not reachable from Python).
- A rational base with a negative integer exponent is not evaluated (`(1/2)**-3` stays `2^3`), while `Expr(2)**-3 == Fraction(1, 8)`. CSL-level.
- Printed factor order of symbolic tensor products is pointer-based and may vary; tests use order-insensitive checks.
- `freeIndices` and `dummyIndices` return null-owner indices (Python space `None`).
- Relabelling resets index ids to 0.
- Symmetry group-closure conflicts are not detected.
- Not run: `ctest -L system` (forbidden by `AGENTS.md`); unfiltered `ctest`.

## G3 review round: API renames (2026-11-18)

Status: accepted by the user at G3 (2026-11-20). User input: "0 & 1 are done & accepted. Proceed with 2."

G2 AI review: not completed. The round 1 reviewer subagent failed three times (connection timeout to `api.business.githubcopilot.com:443`). The user skipped G2. No `05-ai-review-round-1.md` was written.

Amendment A1 (`03-spec.md` section 12): `symbolic.oo` is renamed `symbolic.infinity`, and `Expr.subs` is renamed `Expr.replace`. No alias is kept. The C++ names and the Q&A rows are unchanged.

Changed files:

- `python/src/symbolic.cpp`: method `replace`; module attribute `infinity` (the C++ `ExprHandle::infinity()` is unchanged).
- `python/pymarty/_pymarty.pyi`, `symbolic.pyi`, `symbolic.py`: stubs and re-exports.
- `python/tests/symbolic/test_transform.py`: calls and test names `test_replace_*`; `test_operators.py`: `symbolic.infinity`.
- `python/examples/usage/symbolic.py`: calls, comments and messages.
- `docs/architecture/pymarty/phase-1/03-spec.md` and `phase-1-symbolic.md`: Python-facing names.

Verification (after rebuilding the `_pymarty` target):

- `clang-format --dry-run --Werror python/src/symbolic.cpp`: clean.
- `pytest python/tests -m "not slow"`: 190 passed.
- `ruff check .`, `ruff format --check .`: clean. `mypy --strict pymarty tests tools`: 23 files clean.
- `python/tools/check_parity.py`: `parity: OK, 68 C++ API tests: 46 with a Python counterpart, 22 exempted`.
- Doctests of `pymarty/symbolic.py` (build tree): 3 attempted, 0 failed.
- `python/examples/usage/symbolic.py`: exit 0.
- No `.subs(` or `symbolic.oo` left under `python/`.

Known limitations: see the previous section. The port-table row selection `IndexIsRelabelledWithItsPair` (`port-table.md` line 128) is still open with the user.

