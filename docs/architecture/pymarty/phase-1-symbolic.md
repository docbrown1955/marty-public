# Phase 1: `pymarty.symbolic`

Python access to CSL expressions: numbers, symbols, functions, algebraic transformations, and the
index/tensor objects that later phases need.

Prerequisites: Phase 0 accepted.

## 1. Scope

In scope:
- `Expr` with arithmetic, comparison, printing (text and LaTeX), numeric conversion.
- Constructors for numbers (exact integers, rationals, floats, complex), constants and variables.
- Mathematical functions (`sqrt`, `exp`, `log`, `sin`, `cos`, `tan`, `pow`, hyperbolic and inverse ones
  that CSL provides).
- Transformations: `expand`, `deep_expand`, `factor`, `collect`, `diff`, `subs`, `simplify`,
  `evaluate`, `conjugate`, `real`, `imag`, `depends_on`, `free_symbols`.
- Level-2 objects needed by modeling and diracology: `Space`, `Index`, `Tensor`, element access,
  contraction semantics of `ISum`/`IProd`, `delta`, `metric`, `epsilon`, index relabelling.
- Copy semantics, hashing, equality.

Out of scope (explicit `NotImplementedError` or not exposed): callback-taking functions (`ExpandIf`,
`DistributeIf`, predicates), integrals, `Abbrev` management, `LibraryGenerator` (Phase 6), vector and
matrix types beyond what Phase 3/4 need, Python-side pickling, sympy conversion (the
`__marty_expr__` protocol makes third-party interop possible, but no bridge is shipped).

## 2. C++ API (src/csl/api/)

Header set (proposal): `expr.h`, `functions.h`, `transform.h`, `tensor.h`.

| Concern | C++ API (sketch) | Python spelling |
|---------|------------------|-----------------|
| Handle | `class ExprHandle` wrapping `csl::Expr`; value semantics, never mutated after construction | `Expr` |
| Numbers | `integer(int64)`, `rational(Rational)`, `real(double)`, `complex(re, im)`, `imaginary_unit()` | `Expr(3)`, `Expr(Fraction(1,3))`, `Expr(0.5)`, `Expr(1+2j)`, `I` |
| Symbols | `constant(name)`, `variable(name)`; name validation | `constant("g")`, `variable("x")` |
| Operators | `add`, `sub`, `mul`, `div`, `neg`, `pow` | `+ - * / ** unary -` |
| Functions | `exp`, `log`, `sqrt`, `sin`, `cos`, ... | `symbolic.sin(x)` |
| Transform | `expanded(e, full)`, `factored(e)`, `collected(e, factors)`, `derived(e, var)`, `replaced(e, old, new)`, `simplified(e)` | `.expand()`, `.factor()`, `.collect(...)`, `.diff(x)`, `.subs(...)`, `.simplify()` |
| Evaluation | `evaluated(e, mode)`, `to_complex(e)` (error if free symbols), `to_double(e)` | `.evaluate(**values)`, `complex(e)`, `float(e)` |
| Analysis | `depends_on(e, sym)`, `free_symbols(e)`, `type_of(e)` | `.depends_on(s)`, `.free_symbols`, `.kind` |
| Printing | `to_string(e)`, `to_latex(e)`, `to_code(e)` | `str(e)`, `.to_latex()`, `._repr_latex_()` |
| Equality | `equals(a, b)` (documented meaning), `hash(e)` | `==`, `hash(e)` |
| Copy | `deep_copy(e)` | `copy.copy`, `copy.deepcopy` |
| Index objects | `SpaceHandle`, `IndexHandle`, `TensorHandle`; `tensor(name, spaces, symmetry)`; element creation validates index spaces; `delta(space)`, `metric(space)`, `epsilon(space)`; `relabel_indices(e)` | `Space`, `Index`, `Tensor`, `T(i, j)`, `delta`, `metric`, `epsilon`, `relabel(e)` |
| Properties | symbol properties (real, positive, ...) as an enum | `constant("m", real=True)` |

Facade requirements specific to this phase:
- `subs` accepts a mapping and applies it as a simultaneous replacement; the old CSL `Replace` order
  semantics are documented and tested.
- `evaluate` with values substitutes then evaluates; it never changes the original object.
- A numeric conversion of an expression that still has free symbols throws `ValueError` listing them.
- Equality: the C++ API documents whether it is structural or CSL's comparison (verify in the spike of
  this phase and record in the header). `hash` must be consistent with it. If a consistent hash
  cannot be guaranteed, `Expr` is made unhashable (`__hash__ = None`) and a separate `structural_key()`
  is offered. This decision is recorded in `tests/api/symbolic/` as a test.
- Indices: spaces are long-lived (static) in CSL, and `Index` holds a `const Space*`. The handle
  holds a `shared_ptr` or relies on documented static lifetime (to verify).

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

L1 (`tests/api/symbolic/`, gtest): one test file per concern, naming `TEST(SymbolicApi, <Behaviour>)`:
- Numbers: `IntegerArithmeticExact`, `RationalNormalisation`, `FloatContagion`, `ComplexNumbers`.
- Symbols: `ConstantNameValidation`, `SameNameSameSymbol` (verify CSL semantics), `Properties`.
- Operators and functions: `SumProductPowerCanonicalisation`, `FunctionsSpecialValues`
  (`sin(0)`, `exp(0)`, `log(1)`, `sqrt(4)`).
- Transforms: `ExpandBinomial` (`(a+b)^2`), `FactorRoundTrip`, `DiffPolynomial`, `DiffChainRule`,
  `SubsSimultaneous`, `SubsDoesNotMutateOriginal`, `ConjugateOfComplexConstant`,
  `EvaluateWithValues`, `EvaluateWithFreeSymbolsFails`.
- Equality and hash: `EqualityDocumentedSemantics`, `HashConsistentWithEquality` (or
  `ExprNotHashable`).
- Printing: `LatexMatchesExistingCslOutput` (compares with strings from `tests/units/csl/test_latex.cpp`),
  `CodeOutputStable`.
- Indices/tensors: `IndexSpaceCompatibility`, `RepeatedIndexContracts` (ISum/IProd), `MetricDeltaInvariant`,
  `RelabelIndicesCanonical`, `EpsilonAntisymmetry`, `SignedRefresh` (from the existing unit tests of the same names).
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
  derivatives, `expr.subs({}) == expr`, `expr.diff(x)` of a symbol independent of x is 0
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
   90% of the cases with an API counterpart are ported. Exemptions have reasons.
3. Round-trip property: for a defined set of 50 expressions (stored in `tests/api/data/symbolic/`),
   `str(e)` and `to_latex()` equal the C++ output byte for byte.
4. Numeric agreement: for the same set, `complex(e.evaluate(**values))` equals the C++ `evaluate`
   result within 1e-12 relative.
5. No observable mutation: a test builds `y = x.expand()` for shared sub-expressions and verifies the
   original expression string and hash are unchanged afterwards (C++ and Python).
6. The `Expr` equality/hash decision is recorded, tested, and documented in the docstring.
7. Python overhead check: a loop of 10 000 additions through Python is within 3x of the same loop in a C++
   test (recorded, not a hard performance guarantee, reported in the PR).
8. State neutrality, lifetime and sanitizer gates pass. Quality gates pass.
9. `symbolic.pyi` stub is complete (mypy strict clean), and the doctests pass.

## 6. Exit artifacts

`docs` page of the module (docstrings), a short example script
(`python/examples/symbolic_basics.py`), golden data, port table.

## 7. Risks and open questions

- **Equality and hashing** semantics of CSL expressions may not allow a sound `__hash__`.
- **Canonical form changes** during simplification: results are not necessarily what a user expects;
  documented as "CSL canonical form", and tests compare against the C++ output.
- **Global `Space` registry** and indices: the lifetime and thread-safety answer comes from spike B.
- CSL `Replace` semantics versus simultaneous substitution may differ in corner cases (the C++ test
  `SubsSimultaneous` pins the behaviour).
