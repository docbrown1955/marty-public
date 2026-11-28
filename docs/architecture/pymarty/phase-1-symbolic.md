# Phase 1: `pymarty.symbolic`

Python access to CSL expressions: numbers, symbols, functions, algebraic transformations, and the
index/tensor objects that later phases need.

Prerequisites: Phase 0 accepted.

Usage example gate: before any C++ API or binding work starts, `python/examples/usage/symbolic.py` is written
and approved by a human reviewer ([testing-strategy.md](testing-strategy.md) section 1.2). It fixes the
Python API of this phase and is the first deliverable.

## 1. Scope

In scope:
- `Expr` with arithmetic, comparison, printing (text and LaTeX), numeric conversion.
- Constructors for numbers (exact integers, rationals, floats, complex), constants and variables.
- Mathematical functions (`sqrt`, `exp`, `log`, `sin`, `cos`, `tan`, `pow`, hyperbolic and inverse ones
  that CSL provides).
- Transformations with a fixed signature and no callback: `expand`, `factor`, `collect`, `diff`, `replace`,
  `evaluate`, `conjugate`, `real_part`, `imag_part`, `depends_on`, `free_symbols`. `simplify` is not exposed
  (`csl::Simplify` has no definition, Q14) and `deep_expand` is deferred.
- Level-2 objects needed by modeling and diracology: `Space` (user-created, integer dimension, plus
  `Minkowski` and `Euclid_R2..R4`), `Index` (free, fixed, sign flip), `Tensor` with full or pairwise
  (anti)symmetry, element access, contraction through the product operator, `free_indices`,
  `dummy_indices`, `delta`, `metric`, `epsilon`, index relabelling.
- Copy semantics, hashing (symbols and numbers only), equality.

Out of scope (explicit `NotImplementedError` or not exposed): callback-taking functions (`ExpandIf`,
`DistributeIf`, predicates), the general transformation machinery, `simplify` and `deep_expand`, integrals,
`Abbrev` management, `LibraryGenerator` (Phase 6), `ISum`/`IProd` objects, general symmetry permutations,
signed user spaces and symbolic-dimension spaces, vector and matrix types beyond what Phase 3/4 need,
Python-side pickling, sympy conversion (the `__marty_expr__` protocol makes third-party interop possible,
but no bridge is shipped).

## 2. C++ API (src/csl/api/)

Header set (proposal): `expr.h`, `functions.h`, `transform.h`, `tensor.h`.

| Concern | C++ API (sketch) | Python spelling |
|---------|------------------|-----------------|
| Handle | `class ExprHandle` wrapping `csl::Expr`; value semantics, never mutated after construction | `Expr` |
| Numbers | `integer(int64)`, `rational(Rational)`, `real(double)`, `complex(re, im)`, `imaginary_unit()`, `infinity()` | `Expr(3)`, `Expr(Fraction(1,3))`, `Expr(0.5)`, `Expr(1+2j)`, `I`, `infinity` |
| Symbols | `constant(name)`, `variable(name)`; name validation | `constant("g")`, `variable("x")` |
| Operators | `add`, `sub`, `mul`, `div`, `neg`, `pow` | `+ - * / ** unary -` |
| Functions | `exp`, `log`, `sqrt`, `sin`, `cos`, ... | `symbolic.sin(x)` |
| Transform | `expanded(e)`, `factored(e, by, full)`, `collected(e, factors, full)`, `derived(e, var)`, `replaced(e, mapping)`, `conjugated(e)`, `real_part(e)`, `imag_part(e)` | `.expand()`, `.factor()`, `.collect(...)`, `.diff(x)`, `.replace(...)`, `.conjugate()`, `.real_part()`, `.imag_part()` |
| Evaluation | `evaluated(e, mode)`, `to_complex(e)` (error if free symbols), `to_double(e)` | `.evaluate(**values)`, `complex(e)`, `float(e)` |
| Analysis | `depends_on(e, sym)`, `free_symbols(e)`, `type_of(e)` | `.depends_on(s)`, `.free_symbols`, `.kind` |
| Printing | `to_string(e)`, `to_latex(e)`, `to_code(e)` | `str(e)`, `.to_latex()`, `._repr_latex_()` |
| Equality | `equals(a, b)` (documented meaning), `hash(e)` | `==`, `hash(e)` |
| Copy | `deep_copy(e)` | `copy.copy`, `copy.deepcopy` |
| Index objects | `SpaceHandle`, `IndexHandle`, `TensorHandle`; `tensor(name, spaces, symmetric, antisymmetric)`; element creation validates index spaces; `delta(i, j)`, `metric(i, j)`, `epsilon(indices)` (the space comes from the indices); `relabel_indices(e, renaming, relabel_free)` as an Expr method | `Space`, `Index`, `tensor`, `T(i, j)`, `delta`, `metric`, `epsilon`, `e.relabel_indices(renaming=None, *, relabel_free=True)`, `space.index()`, `space.indices(n)` |
| Properties | symbol properties (real, positive, ...) as an enum | `constant("m", real=True)` |

Facade requirements specific to this phase:
- `replace` accepts a mapping and passes all its pairs to CSL `Replace` (decided, Q7): one bottom-up pass,
  behaviour for compound keys is whatever `Replace` does and is pinned by tests. It is not guaranteed to be
  a simultaneous substitution beyond what the tests pin.
- `evaluate` with values substitutes by replacement (no `setValue` on the shared parent), never changes the
  original object, and leaves free symbols in place (decided, Q8). `complex()` and `float()` raise.
- A numeric conversion of an expression that still has free symbols throws `ValueError` listing them.
- Powers (decided, Q2, Q3 and Q15, C++ core change, spec section 5): base `oo` raises `ValueError`; base 0
  with a negative numerical exponent raises `MartyZeroDivisionError` (Q17); `pow_s` returns exact results
  for perfect powers (`sqrt(4)` is `2`, `4^(3/2)` is `8`). The C++ `SIGSEGV` paths in
  `src/csl/operations.cpp` (around lines 3158-3170) are replaced. A division `x / 0` keeps the CSL
  behaviour (infinity, Q16).
- Equality (decided, Q1): CSL identity. Constants and variables with the same name are different symbols
  (`literal.cpp:239-250`). Compound expressions compare structurally. Hash (decided, Q10): symbols and
  numbers are hashable; a sum, product, power or function raises `TypeError`; `structural_key()` is offered
  for those (spec section 4).
- Indices (decided, Q4): the handle keeps its owner alive. `Index` holds its `Space` handle, and a
  `Space` handle holds a reference to its C++ owner (a static space needs no ownership).
- C++ cleanup (decided, Q14): the undefined declarations of `src/csl/simplification.h` (`Simplify` and the
  others without definition and caller) are removed.

## 3. Python layer

- `Expr.__init__(value)` coercion order: `Expr` > `__marty_expr__` > `__index__` > rational-like >
  `__complex__` if imaginary part non-zero > `__float__`. `bool` is rejected (`TypeError`), to avoid
  silent surprises.
- Reflected operators for all numeric types (`2 * x`, `Fraction(1,3) * x`).
- `__pow__` supports integer, rational and expression exponents.
- `Expr.__eq__` returns `bool`; comparison with non-expression numbers coerces.
- `__float__`, `__complex__` use `to_double`/`to_complex`.
- `__repr__` is `Expr('<str>')` style for debugging, `__str__` is CSL text, `_repr_latex_` for notebooks.
- Immutability: no method mutates. `__copy__` and `__deepcopy__` call `deep_copy`.
- `Tensor` objects are callable with any iterable of `Index`; element objects are `Expr`.
- Module-level constants `symbolic.pi`, `symbolic.e`, `symbolic.I` as `Expr`.

## 4. Test plan

L1 (`tests/api/src/symbolic_*.cpp`, gtest, flat files as `ADD_API_TEST` requires; spec section 6): one test file per concern, naming `TEST(SymbolicApi, <Behaviour>)`:
- Numbers: `IntegerArithmeticExact`, `RationalNormalisation`, `FloatContagion`, `ComplexNumbers`.
- Symbols: `ConstantNameValidation`, `SameNameDifferentSymbol` (CSL identity, Q1), `Properties`.
- Operators and functions: `SumProductPowerCanonicalisation`, `FunctionsSpecialValues`
  (`sin(0)`, `exp(0)`, `log(1)`, `sqrt(4)` is `2`), `PowInfinityBaseThrows`, `PowZeroBaseNegativeExponentThrows`.
- Transforms: `ExpandBinomial` (`(a+b)^2`), `FactorCommonFactor`, `CollectBySymbol`, `DiffPolynomial`,
  `DiffChainRule`, `SubsSymbolSwap`, `SubsDoesNotMutateOriginal`, `ConjugateInvolution`,
  `RealPartOfRealSymbol`, `ImagPartOfRealSymbol`, `EvaluateWithValues`, `EvaluateWithFreeSymbolsKeepsThem`.
- Equality and hash: `EqualityDocumentedSemantics`, `HashSymbolsAndNumbers`, `HashCompoundRaises`.
- Printing: `LatexMatchesExistingCslOutput` (compares with strings from `tests/units/csl/test_latex.cpp`),
  `CodeOutputStable`.
- Indices/tensors: `IndexSpaceCompatibility`, `RepeatedIndexContracts` (product operator), `MetricDeltaInvariant`,
  `RelabelIndicesCanonical`, `EpsilonAntisymmetry`, `SignedRefresh` (from the existing unit tests of the same names),
  `SymmetricTensorPair`, `AntisymmetricTensorPair`, `ConflictingSymmetryRaises` (new: no existing test covers symmetries).
- State and ownership: `OperationsLeaveGlobalOptionsUntouched`, `HandlesAreImmutable` (mutate-via-core
  attempts do not show through other handles), `IndexOutlivesScope`.
- Errors: each failing call raises a typed error with the exact message documented.

L2 (`python/tests/symbolic/`):
- Parity tests for every L1 test above.
- Duck typing: `Fraction`, `numpy`-free fake number classes implementing only `__index__` /
  `__float__` / `__complex__`; objects with `__marty_expr__`; rejection of `bool` and `str` where
  documented.
- Operators with reflected operands, `sum(...)` builtin over expressions, `functools.reduce`.
- `repr`, `str`, `_repr_latex_`, `copy`, `deepcopy`, doctests of every docstring.
- Properties: `(a+b)**2 - (a**2 + 2*a*b + b**2)` expands to 0, derivative of sum equals sum of
  derivatives, `expr.replace({}) == expr`, `expr.diff(x)` of a symbol independent of x is 0
  (parametrised with `hypothesis` if adopted as a dev-only dependency; otherwise a fixed grid).
- Lifetime: tensor element outlives its `Tensor` object; gc loop of expressions does not grow the live
  object counter.
- `pymarty.symbolic` import does not import `modeling`, `calculation`, etc.

L4/L5: none yet. The existing C++ unit tests are the reference:
- `tests/units/csl/test_equality.cpp`, `test_literals.cpp`, `test_numbers.cpp`, `test_latex.cpp`,
  `test_relabel_indices.cpp`, `test_signed_refresh.cpp`, `test_metric_delta_invariant.cpp`,
  `test_init_sanitizer.cpp`: each is ported to Python **where the API exposes the feature**; a table in
  the PR lists every test case as "ported", "covered by L1 only" or "not applicable
  (reason)". `InitSanitizer` is an internal C++ utility: exempt.

## 5. Acceptance criteria

1. All L1 and L2 tests in section 4 pass, with parity checker green for the module.
2. The port table of the eight CSL unit test files is complete: every case is classified, and at least
   90% of the cases whose feature is in this phase's table are ported (spec section 9.1). Deferred cases
   are listed with the phase that needs them. Exemptions have reasons.
3. Round-trip property: for a defined set of 50 expressions (stored in `tests/api/data/symbolic/`),
   `str(e)` and `to_latex()` equal the C++ output byte for byte.
4. Numeric agreement: for the same set, `complex(e.evaluate(**values))` equals the C++ `evaluate`
   result within 1e-12 relative.
5. No observable mutation: a test builds `y = x.expand()` for shared sub-expressions and verifies the
   original expression string and `structural_key()` are unchanged afterwards (C++ and Python).
6. The `Expr` equality and hash decision is recorded, tested, and documented in the docstring.
7. Python overhead check: a loop of 10 000 additions through Python is within 3x of the same loop in a C++
   test (recorded, not a hard performance guarantee, reported in the PR).
8. State neutrality, lifetime and sanitizer gates pass. Quality gates pass.
9. `symbolic.pyi` stub is complete (mypy strict clean), and the doctests pass.
10. `python/examples/usage/symbolic.py` was approved by a human before implementation started (reviewer and
    date in the PR), is unchanged since or its changes were re-approved, and runs in
    `test_usage_examples.py` (L4b).

## 6. Exit artifacts

`docs` page of the module (docstrings), a short example script
(`python/examples/usage/symbolic.py`, the usage example approved at gate G1; the earlier path `python/examples/symbolic_basics.py` is dropped), golden data, port table.

## 7. Risks and open questions

- **Equality and hashing**: decided (Q1, Q10), spec section 4. Compound expressions are not hashable.
- **Canonical form changes** during simplification: results are not necessarily what a user expects;
  documented as "CSL canonical form", and tests compare against the C++ output.
- **Global `Space` registry** and indices: the lifetime and thread-safety answer comes from spike B.
- CSL `Replace` semantics: `replace` is not a guaranteed simultaneous substitution (Q7); the C++ tests
  `SubsSymbolSwap` and `SubsCompoundKeyBehaviourPinned` pin the behaviour.
