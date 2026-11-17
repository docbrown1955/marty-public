Status: READY
Updated: 2026-11-17
Author: orchestrator
Reviewer (G1): _pending_
Usage example: `python/examples/usage/symbolic.py` (status READY, approval pending)

Inputs: `01-investigation-core-classes.md`, `01-investigation-equality-hash-printing.md`, `01-investigation-tests-and-wiring.md`, `01-investigation-transforms.md`, `01-investigation-transforms-2.md`, `01-investigation-indices-tensors.md` (all READY; each carries a "Verified by orchestrator" section with what was re-checked and what was not), `02-questions.md` (Q1 to Q17, all answered), `../phase-1-symbolic.md` (plan, to be reconciled with this spec, section 11).

# Phase 1 spec: `pymarty.symbolic`

## 1. Baseline

- Git: branch `pymarty`. Working tree before this phase: `docs/architecture/pymarty/phase-1-symbolic.md` modified, `docs/architecture/pymarty/phase-1/` and `python/examples/usage/symbolic.py` untracked. No code is changed yet.
- Build directory: `/tmp/pymarty-build` (GCC 12, CMake, gtest). `/home/gregoire/marty-public/build` is not used.
- Baseline run, 2026-11-17: `ctest -L 'units|integration'` plus the `api` and `python` labels in `/tmp/pymarty-build`: 216 of 216 passed (log `/tmp/p1-baseline.log`). Green, so any later red result belongs to the phase.
- The `system` suite is never run (AGENTS.md).

## 2. Decisions taken in the Q&A

| # | Question | Answer | Consequence |
|---|----------|--------|-------------|
| Q1 | Equality of same-named symbols | CSL identity | `constant("a") == constant("a")` is `False`. |
| Q2 | `pow` with base `oo`, or base 0 with negative exponent | change `pow_s` to throw | **C++ core change** (section 5.1). Base `oo`: `ValueError`. Base 0 with a negative exponent: `ZeroDivisionError`. |
| Q3 | Output form of `sqrt(4)` | change CSL so that perfect powers are exact | **C++ core change** (section 5.2). |
| Q4 | Lifetime of spaces and indices | the handle keeps its owner alive | `Space`, `Index` and tensor handles hold shared ownership. |
| Q5 | Transform set | `expand`, `diff`, `subs`, `evaluate`, `depends_on`, `free_symbols`, `conjugate`, `real`, `imag`, `factor`, `collect`, `simplify`. Fixed signatures, no callbacks. | `deep_expand` was not selected, so it is deferred. `simplify` is dropped by Q14. |
| Q6 | Indices and tensors | in scope, with asserted usage example | WP4. |
| Q7 | `subs` semantics | call `Replace` as is | One bottom-up pass; behaviour for compound keys is what `Replace` does and is pinned by tests (section 7.3). |
| Q8 | `evaluate` with free symbols | return the partially evaluated `Expr` | `complex()` and `float()` raise `MartyValueError`. |
| Q9 | Spaces exposed | user space (name, integer dimension, index names), plus `Minkowski`, `Euclid_R2`, `Euclid_R3`, `Euclid_R4` | Signed user spaces and symbolic-dimension spaces are deferred. |
| Q10 | Hash | symbols and numbers hashable; compound expressions raise `TypeError` | `free_symbols` is a `frozenset`; `{x: y}` works as `subs` argument. |
| Q11 | Symmetries | full symmetric, full antisymmetric, index pairs | General permutations deferred. A conflicting declaration raises instead of CSL's silent no-op. |
| Q12 | Contraction | through the product operator; `free_indices` and `dummy_indices` queries | No `ISum` or `IProd` objects. |
| Q13 | Index creation | single index with optional name; list of indices | `Space.index(name=None)` and `Space.indices(n, name=None)` (section 6.3). |
| Q14 | `simplify` | not in Python; remove the stale C++ declarations | **C++ header change** (section 5.3). |
| Q15 | Perfect-power rule | full rule for any `p/q` | `sqrt(4)=2`, `4^(3/2)=8`. |
| Q16 | `x / 0` | keep CSL (returns infinity) | Two behaviours in CSL: `x / 0` gives `oo`, `0 ** -1` raises (section 5.1). Asked again at G1. |
| Q17 | Class for zero to a negative power | `MartyZeroDivisionError(MartyError, ZeroDivisionError)` | New error class (section 6.4). |

Q2, Q3 and Q14 change C++ code for every MARTY user and need explicit approval at G1 (section 5).

## 3. Scope

### 3.1 In scope

- `Expr`: arithmetic, comparison, text and LaTeX printing, numeric conversion, copy, hash (section 4).
- Constructors for integers, rationals, floats, complex numbers, the imaginary unit, constants and variables.
- Mathematical functions CSL provides without callback: `sqrt`, `exp`, `log`, `sin`, `cos`, `tan` and the hyperbolic and inverse ones (the exact list is fixed in WP2 from `src/csl/interface.h`).
- Transformations: `expand`, `factor`, `collect`, `diff`, `subs`, `evaluate`, `conjugate`, `real_part`, `imag_part`, `depends_on`, `free_symbols` (section 6.2).
- Spaces: user-created (integer dimension, index names) and the predefined `Minkowski`, `Euclid_R2`, `Euclid_R3`, `Euclid_R4`.
- `Index` (free, fixed, sign flip), `tensor(...)` with symmetries, element access, `delta`, `metric`, `epsilon`, contraction by product, `free_indices`, `dummy_indices`, `relabel`.

### 3.2 Out of scope (not exposed; no stub)

`simplify` (Q14), `deep_expand`, callback-taking functions (`ExpandIf`, `DistributeIf`, predicates), integrals, abbreviation management, `LibraryGenerator` (Phase 6), matrix and vector types, `ISum` and `IProd` objects, general symmetry permutations, signed user spaces, symbolic-dimension spaces, `Space.resetIndexNumber`, pickling, a sympy bridge.

### 3.3 Deferred inside this phase

- `pow` of a negative base with a fractional exponent beyond what CSL returns today. CSL's form is kept.
- The `Integer::operator==` `int` cast (section 4.3) is recorded as a limitation, not fixed.

## 4. Equality and hash

### 4.1 Equality (Q1)

`Expr.__eq__` is CSL equality (`Abstract::operator==`). Constants and variables compare by parent identity (`src/csl/literal.cpp:239-250`), so two calls of `constant("a")` give two different symbols. Compound expressions compare structurally. The result depends on `csl::option::checkCommutations` (`01-investigation-equality-hash-printing.md`, point 10), which the module never changes.

### 4.2 Hash (Q10)

- `hash(symbol)` is based on the identity of the symbol's parent, consistent with `==`.
- `hash(number)` is the hash of the exact value (Python `int`, `Fraction`, `float` or `complex` equivalent), so that numbers that CSL treats as equal hash equally. WP2 tests `NumberHashMatchesEquality` on the cross-type pairs that CSL equality treats as equal (to verify; not yet run).
- `hash()` of a sum, product, power, function or any indexed expression raises `TypeError` with a message pointing to `structural_key()`.
- `structural_key()` returns a canonical, hashable key for any expression: order independent for sums and products, numbers by value, symbols by parent identity (a name-based key would merge distinct symbols). Tests: `HashSymbolsAndNumbers`, `HashCompoundRaises`, `StructuralKeyOrderIndependent`.
- `Index` hash: space identity, name and ID. Sign is ignored, as in `Index::operator==` (`src/csl/index.cpp:270-280`, not re-read). `Index.__lt__` is not exposed, because CSL ordering compares space pointers (`src/csl/index.cpp:320`) and is not reproducible across runs.

### 4.3 Known limitation

`Integer::operator==` casts `evaluateScalar()` to `int` (`src/csl/numerical.cpp:338`), which is undefined for values beyond the `int` range. Recorded as a limitation (G1 item 9).

## 5. C++ core changes (require G1 approval)

### 5.1 `pow_s` errors (Q2)

Current behaviour (`src/csl/operations.cpp:3156-3171`, verified): `pow_s` calls `std::raise(SIGSEGV)` for base `oo` and for base 0 with a negative real exponent, then returns `CSL_INF`.

- Base `oo`: `CALL_SMERROR(CSLError::ValueError)` with a message.
- Base 0 with a negative numerical exponent: `CALL_SMERROR(CSLError::ZeroDivisionError)` with a message. `CSLError::ZeroDivisionError` already exists (`src/csl/error.h:54`, verified); the facade translates it to `csl::api::ZeroDivisionError`.
- Behaviour change for C++ callers: a program that reached these paths used to abort with `SIGSEGV`; it now gets an exception through the existing error machinery.

Division is a different path. `div_s` prints "Warning division by zero" to `std::cerr` and returns `CSL_INF` (`src/csl/operations.cpp:2520-2529`, verified), and `intfraction_s(5, 0)` is `CSL_INF` (`tests/units/csl/test_numbers.cpp:119-120`, verified). Q16 keeps this. So in Python `x / 0` returns `oo` and writes the C++ warning to stderr, while `Expr(0) ** -1` raises `MartyZeroDivisionError`. G1 item 4 asks whether this split is intended.

### 5.2 Exact perfect powers (Q3 and Q15)

Current behaviour (`src/csl/operations.cpp:3122-3140`): `pow_s(4, 1/2)` returns `Pow(4, 1/2)`; `pow_s(4, 3/2)` returns `4 * Pow(4, 1/2)`.

Rule: for an integer base `a > 0` and an exponent `p/q` in lowest terms with `q > 1`, if `a = r^q` with an integer `r > 0` the result is `r^p` (an integer, or an `IntFraction` when `p < 0`). Otherwise the current code path is unchanged. Examples: `sqrt(4)=2`, `sqrt(9)=3`, `4^(3/2)=8`, `8^(2/3)=4`, `sqrt(2)` stays `Pow(2, 1/2)`. Negative bases are unchanged.

### 5.3 Remove stale declarations (Q14)

`src/csl/simplification.h` declares functions that have no definition: `Simplify` (line 124) has none in the repo, and `src/csl/algorithms/simplification.cpp` and `patternMatch.cpp` are empty. The orchestrator also found no definition and no caller outside the header for `getRecursiveAlternateForms`, `internalRecursiveAlternateForms`, `numberOfMutatingNodes`, `insertSortMutant` and `addMutants`. The other names between lines 63 and 137 (`printVector`, `addAlternateForm`, `reduceAlternate`, `clearRedundancyAlternate`, `getRandomMutation`, `naturalSelection`, `evolve`) are not checked yet.

WP1b removes a declaration, with its Doxygen block, only if `grep` over `src tests examples python` finds zero definitions and zero callers. The grep output is kept in the log. A declaration that has a definition or a caller stays and is reported. `sgl::Simplify` in `src/marty/` is another function and is untouched. No behaviour change; the full `make` and `ctest -L 'units|integration'` stay green.

### 5.4 Impact of 5.1 and 5.2 on tests and data

- Code search (`tests/`, `examples/`, `src/`): no `sqrt` or `pow` call on a perfect power was found, and no unit test asserts `Pow(4, 1/2)`-style output. WP1a repeats the search with the final rule.
- Not searched: golden text in `tests/system/data/`. WP1a greps it for perfect-power `Pow(` forms and reports the result. Reference data is never edited silently (AGENTS.md); the system suite is not run, so the PR description lists the system tests that the change may affect.

## 6. Python API

### 6.1 Module layout

Flat `python/pymarty/symbolic.py` and `symbolic.pyi` (the CMake copy step only copies top-level files, `python/CMakeLists.txt:29-36`). Everything is backed by the single `_pymarty` extension.

### 6.2 Expressions and transformations

| C++ facade (sketch) | Python | Signature (proposed) | Returns | Raises |
|---|---|---|---|---|
| `ExprHandle` | `symbolic.Expr` | `Expr(value)`; coercion order: `Expr`, `__marty_expr__`, `__index__`, rational-like (`Fraction`), `__complex__` with non-zero imaginary part, `__float__` | `Expr` | `TypeError` for `bool` and `str`; `ValueError` if not convertible |
| numbers | `Expr(3)`, `Expr(Fraction(1, 3))`, `Expr(0.5)`, `Expr(1 + 2j)`, `symbolic.I`, `symbolic.oo` | `oo` is CSL's infinity (`CSL_INF`) | `Expr` | |
| `constant`, `variable` | `symbolic.constant(name)`, `symbolic.variable(name)` | `(name: str) -> Expr`; each call makes a new symbol (Q1) | `Expr` | `ValueError` for an invalid name |
| operators | `+ - * / ** unary -`, reflected | | `Expr` | `MartyZeroDivisionError` for `0 ** -1`; `ValueError` for base `oo` |
| functions | `symbolic.sqrt(e)`, `exp`, `log`, `sin`, ... | `(e) -> Expr` (an `Expr` or a number) | `Expr` | |
| `.expand()` | `expand` | `expand() -> Expr` | `Expr` | |
| `.factor(...)` | `factor` | `factor(by: Expr \| None = None, *, full: bool = False)` | `Expr` (the input if nothing factors out) | |
| `.collect(...)` | `collect` | `collect(factors: Iterable[Expr], *, full: bool = False)` | `Expr` | `MartyTypeError` for an indexed factor (CSL asserts `TypeError`, `operations.cpp:782-786`) |
| `.diff(x)` | `diff` | `diff(symbol: Expr) -> Expr` | `Expr` | |
| `.subs(mapping)` | `subs` | `subs(mapping: Mapping[Expr, Expr \| number])`, passed to `Replace` in dict order, one pass (Q7) | `Expr` | |
| `.evaluate(...)` | `evaluate` | `evaluate(mapping=None, /, **by_name)`; values are `Expr` or numbers; a name that matches several distinct symbols raises `MartyValueError`; free symbols stay (Q8); applied by replacement, never by `setValue` | `Expr` | |
| numeric conversion | `complex(e)`, `float(e)`, `int(e)` where exact | | `complex`, `float` | `MartyValueError` listing the free symbols |
| `.conjugate()`, `.real_part()`, `.imag_part()` | | no arguments | `Expr` | |
| `.depends_on(s)`, `.free_symbols` | | `depends_on(symbol) -> bool`; `free_symbols -> frozenset[Expr]` | | |
| `.kind`, `.name` | | `kind -> str`; `name -> str` for a symbol | | `MartyTypeError` for `name` on a non-symbol |
| printing | `str(e)`, `repr(e)`, `.to_latex()`, `._repr_latex_()` | | `str` | |
| equality, hash | `==`, `hash(e)`, `.structural_key()` | section 4 | | `TypeError` for a compound `hash` |
| copy | `copy.copy`, `copy.deepcopy` | | `Expr` | |

Python names `real_part` and `imag_part` are chosen to avoid the `.real` and `.imag` attributes of Python numbers (G1 item 3). C++ asymmetry kept and documented: for nodes without a conjugate rule `GetComplexConjugate` and `GetRealPart` return the input, `GetImaginaryPart` returns `0` (`src/csl/abstract.cpp:645-667`, verified), so `imag_part` of such a node is `0`.

Duck typing (not yet investigated, section 10): `Fraction`, and any object with `__index__`, `__float__` or `__complex__`, are accepted; `bool` and `str` are rejected where the table says so.

Module state: the module changes no `mty::option::*`, no `csl::option::*`, and not `Model::current`. Covered by `OperationsLeaveGlobalOptionsUntouched`.

### 6.3 Spaces, indices, tensors

| Python | Signature (proposed) | Notes |
|---|---|---|
| `symbolic.Space` | `Space(name: str, dimension: int, index_names: Sequence[str] = DEFAULT_INDEX_NAMES)` | Non-empty `index_names`, as CSL asserts. Default tuple fixed in WP4 (proposal: `("i", "j", "k", "l", "m", "n")`). Names may be `"name;latex"` as in CSL (`test_latex.cpp:514`). The `Space` keeps its owner alive (Q4). `ValueError` for `dimension < 1`. |
| `symbolic.Minkowski`, `Euclid_R2`, `Euclid_R3`, `Euclid_R4` | module constants | Static in C++. `Minkowski` is signed, so sign rules apply. |
| `Space.index(name=None)` | `-> Index` | CSL `generateIndex(name)` or, without name, `generateIndex()`, which cycles through the default names (`src/csl/space.h:400-416`, verified). The ID is incremented per name (`getID`, `space.h:383`), so two indices with the same name are distinct. |
| `Space.indices(n, name=None)` | `-> list[Index]` | CSL `generateIndices(n)` and `generateIndices(n, name)` (`space.h:419-435`, verified). Use `idx[5]` in expressions. |
| `Space.fixed_index(value)` | `-> Index` | Checks `0 <= value < dimension` in Python before the C++ call, so the null-space paths of `Index::setValue` are never reached (Option C of the investigation). `MartyValueError` otherwise. |
| `Index.name`, `Index.space`, `Index.flipped()` | | `flipped()` returns an index with the other sign (CSL `getFlipped`). Needed for `Minkowski`. |
| `symbolic.tensor` | `tensor(name, spaces, *, symmetric=False, antisymmetric=False)`; `symmetric` and `antisymmetric` are `bool` (full) or a sequence of index-position pairs | `T(i, j) -> Expr`. Fully symmetric together with any antisymmetric declaration, or fully antisymmetric together with any symmetric one, raises `MartyValueError` (CSL ignores it silently, `src/csl/indicial.cpp:1095-1096`). Wrong number or spaces of indices: `MartyValueError`. |
| `symbolic.delta`, `metric`, `epsilon` | `delta(i, j)`, `metric(i, j)`, `epsilon(*indices)` | The space is taken from the indices; mixed spaces raise `MartyValueError`. `epsilon` needs `2 <= dimension < 5` (`src/csl/space.cpp:182`). Not the `delta(space)` of the plan; plan to be updated. |
| contraction | `T(i) * U(i)` | Contracts by CSL's product rule; no `ISum` or `IProd` objects. A lower-lower or upper-upper contraction in a signed space raises (`src/csl/index.cpp:231-234`, `RuntimeError`), mapped to `MartyValueError`. |
| `Expr.free_indices`, `Expr.dummy_indices` | `-> tuple[Index, ...]` | From `getFreeIndexStructure()` and `getIndexStructure()` (`src/csl/abstract.h:416-426`). Order: CSL structure order (to be pinned by a test). |
| `symbolic.relabel` | `relabel(expr, *, relabel_free=True) -> Expr` | CSL `RelabelIndices` (`src/csl/replace.cpp:545-621`); works on a copy (test `inputIsNotModified`, `test_relabel_indices.cpp:183-196`). |

Space state: `generateIndex` advances per-space counters inside the C++ `Space` (`availableIndices`, `defaultName`). Expressions stay immutable; the space is not. This is documented in the docstring.

### 6.4 Errors

- `MartyZeroDivisionError(MartyError, ZeroDivisionError)`: new. WP2 adds the C++ class (`csl::api::ZeroDivisionError` in `src/csl/api/error.h`), the translation in `python/src/module.cpp` (`translateException`, lines 71-111), the export in `python/pymarty/errors.py`, and its doctest.
- Core errors are a bare `CSLError` enum without a message, and the message is printed to `std::cerr` only when `csl::option::printErrors` is set (`src/csl/error.cpp:76-96`). The facade catches `CSLError` and rethrows with its own message; WP2 tests it.

## 7. C++ facade

### 7.1 Headers

Facade under `src/csl/api/` (standing decision):

- `expr.h`: `class ExprHandle` (value semantics, never mutated), numbers and symbols.
- `functions.h`: mathematical functions; each doc comment names the Python spelling.
- `transform.h`: `expanded`, `factored`, `collected`, `derived`, `replaced`, `evaluated`, `conjugated`, `real_part`, `imag_part`, `depends_on`, `free_symbols`, numeric conversion.
- `tensor.h`: `SpaceHandle`, `IndexHandle`, `tensor`, `delta`, `metric`, `epsilon`, `relabel`, with `std::shared_ptr` ownership (Q4).

### 7.2 Layout (proposals, G1 item 7)

- No `Session` guard. `mty::api::Session` includes MARTY headers (`src/marty/api/common/session.h:19-25`), so a CSL-only facade cannot use it. The CSL facade snapshots `csl::option::*` only (`MTY_CSL_OPTION_FLAGS`), and `clean_state` covers both lists.
- C++ tests: `ADD_API_TEST(NAME)` builds `src/${NAME}.cpp` only (`tests/api/CMakeLists.txt:1-6`). Flat files `tests/api/src/symbolic_<topic>.cpp`. The macro is not changed.
- Binding: `python/src/symbolic.cpp` added to the `_pymarty` target (`pybind11_add_module(_pymarty ...)`, `python/CMakeLists.txt:20`). The README's `python/bindings/` layout is not used.

### 7.3 Test status of the CSL behaviours the Python API relies on

From the investigations: `Collect`, `Collected`, `DeepCollect` have no test under `tests/`; `Factor` appears only as an identity assertion in a shared macro (`tests/units/csl/csltestdata.h:93`); symmetry, `ContractIndex`, `ISum` and `IProd` have no test. `GetComplexConjugate`, `GetRealPart` and `GetImaginaryPart` have tests with literal expectations (`test_numbers.cpp`, `test_latex.cpp`, `test_literals.cpp`). So each of the untested behaviours gets a pinning C++ test, written before its Python binding and listed in the work packages. Expected values of those tests are proposals marked "unrun" in the investigations; if the run disagrees with the proposal, the implementer reports it and does not edit the expectation to pass.

## 8. Work packages

Each package has one verification command. Order is fixed. The implementers are Haiku subagents, one per package, and the orchestrator verifies each result before the next package starts.

**WP1a: `pow_s` core change (C++).**
- Goal: section 5.1 and 5.2.
- Files: `src/csl/operations.cpp` (the `pow_s` function and its integer-base helper only), New `tests/units/csl/test_pow_special.cpp`, added to `tests/units/csl/CMakeLists.txt`.
- Tests: `PowInfinityBaseThrows`, `PowZeroBaseNegativeExponentThrows`, `SqrtPerfectSquare`, `PowPerfectPowerRational` (`4^(3/2)=8`, `8^(2/3)=4`), `PowNonPerfectUnchanged` (`sqrt(2)`), `PowNegativeBaseUnchanged`.
- Verify: `cd /tmp/pymarty-build && make -j$(nproc) && ctest -L 'units|integration'` and `ctest -R Pow`. Grep `tests/system/data/` and report.
- Depends on: none.

**WP1b: remove stale declarations (C++ header).**
- Goal: section 5.3.
- Files: `src/csl/simplification.h` only.
- Verify: full `make`, `ctest -L 'units|integration'`, and the grep log for each removed name.
- Depends on: none (may run after WP1a because both touch the build).

**WP2: `Expr`, numbers, symbols, operators, functions, errors.**
- Goal: section 6.2 up to the functions, section 4, `MartyZeroDivisionError` (section 6.4).
- Files: `src/csl/api/expr.*`, `functions.*`, `error.h`; `python/src/` binding and `module.cpp`; `python/pymarty/symbolic.py`, `symbolic.pyi`, `errors.py`; `python/tests/symbolic/test_numbers.py`, `test_operators.py`, `test_equality.py`, `test_errors_zero_division.py`; `tests/api/src/symbolic_numbers.cpp`, `symbolic_symbols.cpp`.
- Tests: `IntegerArithmeticExact`, `RationalNormalisation`, `FloatContagion`, `ComplexNumbers`, `ConstantNameValidation`, `SameNameDifferentSymbol` (Q1), `SumProductPowerCanonicalisation`, `FunctionsSpecialValues` (`sin(0)`, `exp(0)`, `log(1)`, `sqrt(4)`), `DivisionByZeroGivesInfinity` (Q16), `ZeroPowerNegativeRaises`, `EqualityDocumentedSemantics`, `HashSymbolsAndNumbers`, `NumberHashMatchesEquality`, `HashCompoundRaises`, `StructuralKeyOrderIndependent`, `CSLErrorMessageNotLost`.
- Verify: `ctest -L api`; `cd python && python -m pytest tests/symbolic -k "numbers or operators or equality or zero_division" -m "not slow"`.
- Depends on: WP1a, and the two investigations of section 10.

**WP3: transformations, evaluation, printing.**
- Goal: section 6.2 transformation and printing rows. No input is mutated. LaTeX and text equal the C++ output.
- Files: `src/csl/api/transform.*`; binding and `symbolic.py`/`.pyi`; `python/tests/symbolic/test_transform.py`, `test_printing.py`; `tests/api/src/symbolic_transform.cpp`, `symbolic_printing.cpp`; golden set `tests/api/data/symbolic/expressions.txt` (50 expressions, written by a C++ API test under `MARTY_UPDATE_GOLDEN=1`, testing-strategy section 3; the expressions come from existing strings in `tests/units/csl/test_latex.cpp`, never invented).
- Tests (written in C++ first for untested behaviour): `ExpandBinomial`, `FactorCommonFactor`, `FactorNoCommonFactorIsIdentity`, `CollectBySymbol`, `CollectIndexedFactorThrows`, `DiffPolynomial`, `DiffChainRule`, `SubsSymbolSwap`, `SubsDoesNotMutateOriginal`, `SubsCompoundKeyBehaviourPinned`, `EvaluateWithValues`, `EvaluateWithFreeSymbolsKeepsThem`, `EvaluateAmbiguousNameRaises`, `ConjugateInvolution`, `ConjugateOfImaginaryUnit`, `RealPartOfRealSymbol`, `ImagPartOfRealSymbol`, `LatexMatchesExistingCslOutput`, `CodeOutputStable`.
- Verify: `ctest -L api -R 'Subs|Evaluate|Latex|Expand|Factor|Collect|Diff|Conjugate|Real|Imag'`; `python -m pytest python/tests/symbolic/test_printing.py python/tests/symbolic/test_transform.py -m "not slow"`.
- Depends on: WP2.

**WP4: `Space`, `Index`, `tensor`, `delta`, `metric`, `epsilon`, contraction, relabelling.**
- Goal: section 6.3. Owner keep-alive (Q4): an `Index` created in a function still works after the function returns and after `gc.collect()`.
- Files: `src/csl/api/tensor.*`; binding; `symbolic.py`/`.pyi`; `python/tests/symbolic/test_indices.py`, `test_tensors.py`; `tests/api/src/symbolic_indices.cpp`, `symbolic_tensors.cpp`.
- Tests (C++ first): `IndexOutlivesScope`, `IndicesFromListAreDistinct` (10 indices on a space with fewer default names), `FixedIndexRange`, `IndexSpaceMismatchRaises`, `RepeatedIndexContracts`, `SignedSpaceSameSignContractionRaises`, `MetricDeltaInvariant`, `EpsilonAntisymmetry`, `SymmetricTensorPair`, `AntisymmetricTensorPair`, `FullySymmetricTensor`, `ConflictingSymmetryRaises`, `FreeAndDummyIndexQueries`, `RelabelIndicesCanonical` (literals from `test_relabel_indices.cpp` and `test_latex.cpp`), `RelabelDoesNotMutate`, `IndexHashIgnoresSign`.
- Verify: `ctest -L api -R 'Index|Tensor|Metric|Relabel|Epsilon|Symmetr'`; `python -m pytest python/tests/symbolic/test_indices.py python/tests/symbolic/test_tensors.py -m "not slow"`.
- Depends on: WP2.
- Note: `Index::setValue` (`src/csl/index.cpp:114-123`, verified) and `TensorParent::checkIndexRequest` (`src/csl/indicial.cpp:1558-1581`, not re-read) can dereference a null space. The Python API never builds an index without a space, so WP4 adds a C++ test that reproduces each reachable case before any fix, and fixes nothing in this phase unless the reproduction shows it is reachable from the facade.

**WP5: parity, port table, and L2 test suite.**
- Goal: acceptance criteria 1 to 4.
- Files: `python/tests/symbolic/test_parity.py` (reads `tests/api/data/symbolic/expressions.txt`), `test_port_table.py`, `docs/architecture/pymarty/phase-1/port-table.md` (one row per test case of the eight CSL unit files in the plan, 133 cases).
- Verify: `cd python && python -m pytest tests/symbolic/test_parity.py tests/symbolic/test_port_table.py -m "not slow"`.
- Depends on: WP3, WP4.

**WP6: stub, doctests, usage example, quality gates.**
- Goal: acceptance criteria 9 to 11.
- Files: `python/pymarty/symbolic.pyi` complete; docstrings with doctests; the existing harness `python/tests/test_usage_examples.py` runs `python/examples/usage/symbolic.py`.
- Verify: `mypy --strict python/pymarty/symbolic.pyi`; `python -m pytest --doctest-modules python/pymarty/symbolic.py`; `python -m pytest python/tests/test_usage_examples.py -k symbolic`; `ruff check` and `ruff format --check` on the new Python files.
- Depends on: WP5.

Final check (orchestrator): `ctest -L 'units|integration|api'` and `python -m pytest python/tests -m "not slow"`. The `system` suite is excluded.

## 9. Acceptance criteria

| # | Criterion | Command or test |
|---|-----------|-----------------|
| 1 | All L1 and L2 tests pass; parity checker green | `ctest -L api` and `python -m pytest python/tests/symbolic -m "not slow"` |
| 2 | Port table of the eight CSL unit files complete (133 cases); at least 90% of the cases whose feature is in this phase's table ported | `python -m pytest python/tests/symbolic/test_port_table.py` |
| 3 | `str(e)` and `to_latex()` equal C++ output for 50 expressions | `python -m pytest python/tests/symbolic/test_parity.py -k printing` |
| 4 | Numeric agreement within 1e-12 relative | `python -m pytest python/tests/symbolic/test_parity.py -k numeric` |
| 5 | No observable mutation (C++ and Python) | `ctest -R 'SubsDoesNotMutateOriginal|RelabelDoesNotMutate'` and `python -m pytest python/tests/symbolic -k mutation` |
| 6 | Equality and hash decision recorded, tested, documented | `ctest -R 'EqualityDocumentedSemantics|HashSymbolsAndNumbers|HashCompoundRaises'` and `python -m pytest python/tests/symbolic/test_equality.py` |
| 7 | Python overhead of 10 000 additions within 3x of C++ (reported, not a gate) | `python python/tests/symbolic/bench_add.py` |
| 8 | State neutrality, lifetime, sanitizer | `ctest -R OperationsLeaveGlobalOptionsUntouched`; ASan build in `/tmp/pymarty-asan` (`cmake -DCMAKE_CXX_FLAGS=-fsanitize=address`) running `ctest -L 'units|api'` |
| 9 | Stub complete, mypy strict clean, doctests pass | `mypy --strict python/pymarty/symbolic.pyi` and `python -m pytest --doctest-modules python/pymarty/symbolic.py` |
| 10 | Usage example approved, runs in the suite | section 11 and `python -m pytest python/tests/test_usage_examples.py -k symbolic` |
| 11 | `pow_s` change tested in C++ (Q2, Q3, Q15) | `ctest -R 'PowInfinityBaseThrows|PowZeroBaseNegativeExponentThrows|SqrtPerfectSquare|PowPerfectPowerRational|PowNonPerfectUnchanged'` |
| 12 | Stale declarations removed with grep evidence (Q14) | the log of WP1b, and `make` plus `ctest -L 'units|integration'` green |

## 10. Open items, port scope and risks

### 10.1 Investigations not yet done

- Naming and duck typing: coercion rules of `Expr(...)`, rejection of `bool` and `str`, handling of `Fraction` and numpy scalars (the plan lists them; no investigation covers them).
- `Model::current`, `exit`/`abort` and `Show()`: whether any facade call can reach them (Phase 0 scope). Expected: none.
- Short reads still open in the existing investigations: `src/csl/symmetry.cpp`, the tail of `addAntiSymmetry`, `IProd::selfCheckIndexStructure` after line 3720, `EpsilonParent`, `Sum::collect` after line 800.

Proposal (G1 item 8): run the first two as WP0 immediately after G1 and before WP2, with the findings checked by the orchestrator; if they produce a decision, the orchestrator asks the user then. The reads of the third bullet are done by the implementer of WP3 and WP4 as part of writing the pinning tests.

### 10.2 Port scope (proposal, G1 item 6)

The 133 cases of the eight CSL unit files are classified in `01-investigation-tests-and-wiring.md` section 6: P 54, P+ 55, L1 13, X 11 (not re-verified by the orchestrator). The P+ cases need features that the plan's table lacked (A1 to A12 of that file). Proposal:

- In this phase: A5 (index inspection, fixed indices), A6 (relabelling options), A7 (derivative nodes in relabelling), A8 (`abs`, `arg`, complex accessors of numbers, `conjugate`), A10 (division by zero: ported with the CSL behaviour of Q16).
- Deferred: A1 (matrix printing), A2 (non-commuting symbols), A3 (LaTeX names of symbols) go to Phase 2 (diracology). A4 (signed user spaces) is deferred by Q9; the `Minkowski` flip is in scope through `Index.flipped()`. A9 (complex-valued constants and variables) goes to a later phase. A11 (abbreviations) stays out of scope.
- Criterion 2 counts the cases whose feature is in this phase's table. Deferred cases stay in the port table, marked "deferred to phase N" with the reason. This changes how the 90% target is counted.

### 10.3 Risks

- Canonical form: printed text is CSL canonical form; tests compare with C++ output.
- Global flags: `csl::option::fullComparison` (default `true`) changes `<` and the argument order of sums and products; `checkCommutations` changes `==`. Both are in `MTY_CSL_OPTION_FLAGS`, so `clean_state` covers them.
- Tensor ordering is pointer based (`01-investigation-equality-hash-printing.md`, point 13, not run). Printed order may vary between runs; parity tests must not rely on it until verified.
- `subs` with compound keys is whatever `Replace` does (Q7); WP3 pins the cases and the docstring says "one pass".
- The `Space` is mutable (index counters) while expressions are not.
- `factor`, `collect`, `conjugate`, `real_part`, `imag_part` have thin test coverage (section 7.3); their semantics are pinned in WP3 from reading, not from existing tests.
- `src/csl/index_new.h` has suspicious code (`indices[0] == first;`, an `operator+` without a return) and is listed in `src/csl/CMakeLists.txt:45-46`; compile status unknown, not used by the facade.
- Static crash candidates in the investigation (facts 66 to 73 of `01-investigation-indices-tensors.md`) are unrun; only fact 13 was re-read.

## 11. Gate G1 request

Approval needed on:

1. C++ core changes: Q2 and Q15 (`pow_s` throws; exact perfect powers including `4^(3/2)=8`), and Q14 (removal of the undefined declarations in `src/csl/simplification.h`).
2. The Python API of sections 6.2 and 6.3, in particular `Space(name, dimension, index_names)`, `space.index` and `space.indices`, `fixed_index`, `tensor(..., symmetric=, antisymmetric=)`, `delta(i, j)` and `metric(i, j)` taking indices (not the `delta(space)` of the plan), and `Index.flipped()`.
3. Names `real_part` and `imag_part`, and the kept C++ asymmetry (`imag_part` of an unsupported node is `0`, `real_part` returns the input).
4. Division by zero: `x / 0` returns `oo` with a stderr warning (Q16), while `0 ** -1` raises `MartyZeroDivisionError` (Q2 and Q17). Keep the split, or also raise for `x / 0`?
5. `evaluate` rules: by name through keyword arguments, optional mapping with `Expr` keys, error when a name is ambiguous.
6. Port scope (section 10.2), including the new counting of the 90% target.
7. Layout (section 7.2): CSL-only facade without `Session`, flat `tests/api/src/symbolic_*.cpp`, `python/src/symbolic.cpp` in the `_pymarty` target, flat `python/pymarty/symbolic.py` and `.pyi`.
8. Running the two missing investigations (section 10.1) as WP0 after G1.
9. `Integer::operator==` int cast: record as a limitation (proposal) or fix in this phase.
10. The usage example `python/examples/usage/symbolic.py`.

Reviewer and date for the usage example are recorded here on approval.
