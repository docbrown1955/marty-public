# Phase 2: `pymarty.diracology`

Pure Dirac / gamma-matrix algebra on top of SGL, usable on its own. The C++ facade already exists in
`src/marty/api/gamma.{h,cpp}` (namespace `mty::gamma_api`, errors as `GammaAPIError`) and is tested by
`tests/integration/src/gammaAPI.cpp`. A previous pybind11 binding exists in git history (branch
`api/gamma`, commit `39f288f`: `src/pybinding/gamma_api.cpp`, `python/` package, stubs, notebook).

Prerequisites: Phase 0 accepted. Phase 1 is not required (duck typing).

Usage example gate: before any C++ API or binding work starts, `python/examples/usage/diracology.py` is
written and approved by a human reviewer ([testing-strategy.md](testing-strategy.md) section 1.2). It fixes
the Python API of this phase (and decides what of the old `api/gamma` binding is kept) and is the first
deliverable.

## 1. Scope

In scope:
- Everything in `gamma.h`: `g`, `eps`, `gamma(mu)`, `gamma(mu, nu)`, `gamma5`, `P_L`, `P_R`, `C`, `DMinko`,
  `imaginaryUnit`, `current`, `trace`, `ordered`, `simplified`, `project`, `applySingleFierz`,
  `applyDoubleFierz`, `generateString`, `generateLatex`, `FierzBasis`, `keepSymbolic4D`.
- Arithmetic on expressions (`+ - *`, unary minus, scalar coefficients), equality, ordering, indexing
  into an expression (`__len__`, `__getitem__`, iteration), copy/deepcopy.
- Scalar coefficients: ints, floats, `Fraction`, and (by duck typing) symbolic objects exposing
  `__marty_expr__`.
- Context manager replacing the global symbolic-dimension switch.

Out of scope for this phase (candidates, each needing a C++ API addition and its own tests if added
later): momenta and slashed momenta (`MomentumIndex`), `sigma^{mu nu}` helpers, named indices
(v1 uses integers like the C++ API), conversion to `csl::Expr` tensors (belongs to modeling).

## 2. C++ API work (src/marty/api/)

The existing facade is kept as is (installed header, existing tests, public behaviour). New work:

| Item | Description |
|------|-------------|
| Alias | `namespace mty::api::diracology = mty::gamma_api;` and a header `diracology/diracology.h` forwarding to `gamma.h`, so that all modules have the same layout |
| Introspection | `size(expr)`, `argument(expr, i)` (the old binding used `expr->size()` / `expr->argument(i)` directly: move that into the API) |
| Comparison | `equals(a, b)`, `less(a, b)` (the old binding converted through `sgl_to_csl` with `defaultSGLTensorSet()` in the binding: move into the API) |
| Scalars | `from_int`, `from_rational`, `from_double`, `scale(expr, Rational)`; coefficient from `csl::Expr` where the build links CSL |
| Copy | `deep_copy(expr)` |
| Dimension | scoped switch: `ScopedSymbolicDimension` using `Session` (the global `keepSymbolic4D(bool)` stays) |
| Errors | `GammaAPIError` derives from or is translated to `mty::api::Error` subclass (`ValueError` for invalid input, `NotImplementedError` where SGL cannot handle an expression) |
| Hardening | Review `gamma.cpp` throw sites (lines near 200, 206, 216, 347, 406) so that every invalid input produces a clear typed message and never a crash. Add input validation for negative indices, `trace` of an empty list, `current` index collisions |

`tests/integration/src/gammaAPI.cpp` is kept in place and registered as an API test source for parity.

## 3. Python layer

| C++ | Python |
|-----|--------|
| `mty::gamma_api::Expr` | `diracology.DiracExpr` (the old package called it `Expr`) |
| `g(mu, nu)`, `eps(...)` | `g(0, 1)`, `eps(0, 1, 2, 3)` |
| `gamma(mu)`, `gamma(mu, nu)` | `gamma(0)`, `gamma(0, 1)` |
| `gamma5()`, `P_L()`, `P_R()`, `C()` | `gamma5()`, `P_L()`, `P_R()`, `C()` |
| `DMinko()`, `imaginaryUnit()` | `D`, `I` module constants |
| `current(chain, iL, iR)`, `trace(chain)` | `current([...], 0, 1)`, `trace([...])` |
| `simplified`, `ordered`, `project` | `.simplify()`, `.order()`, `.project(basis)` |
| `applySingleFierz`, `applyDoubleFierz` | `single_fierz(c1, c2, basis=...)`, `double_fierz(...)` |
| `FierzBasis::{Standard, Chiral}` | `FierzBasis.STANDARD`, `FierzBasis.CHIRAL` (an `enum.Enum`) |
| `keepSymbolic4D(bool)` | `set_symbolic_dimension(keep_symbolic=...)` and `with symbolic_dimension():` |
| `generateString`, `generateLatex` | `str(e)`, `.to_latex()`, `_repr_latex_()` |

Duck typing: indices accept anything with `__index__`; coefficients accept `int`, `float`,
`Fraction`, or an object with `__marty_expr__`. `chain` arguments accept any iterable of `DiracExpr`.
The module must work without importing `pymarty.symbolic`.

## 4. Test plan

L1 (C++ API): the existing cases in `tests/integration/src/gammaAPI.cpp` (equality, epsilon algebra,
Fierz identities, LaTeX generation; the exact count is taken from the file) plus new ones in
`tests/api/diracology/`:
- `DiracologyApi.IntrospectionSizeAndArgument`, `.EqualsAndLess`, `.ScalarCoefficients`,
  `.DeepCopyIndependent`
- `DiracologyApi.ScopedSymbolicDimensionRestores`, `.SymbolicDimensionDoesNotLeakIntoOtherCalls`
- `DiracologyApi.InvalidInputsRaiseTypedErrors` (one case per throw site of `gamma.cpp`)
- Identities, with the values asserted as literals from standard references and checked against the
  SGL convention actually implemented (convention-specific signs are copied from the existing
  `gammaAPI.cpp` assertions, not re-derived):
  - `g(mu,mu) = D` (4 when not symbolic), `{gamma^mu, gamma^nu} = 2 g^{mu nu}`
  - `gamma5^2 = 1`, `P_L + P_R = 1`, `P_L P_R = 0`, `P_L^2 = P_L`
  - `Tr(gamma^mu gamma^nu) = 4 g^{mu nu}`, trace of an odd number of gammas is 0
  - contraction of two epsilons (`eps(0,1,2,3) * eps(0,1,2,4)` simplifies to `-6 g(3,4)` as in the
    earlier Python test `test_basic_expressions`)
  - single and double Fierz transformations on standard and chiral basis: apply twice returns the
    original (where mathematically expected)

L2 (`python/tests/diracology/`): port of the four test files of the old branch
(`test_import.py`, `test_objects.py`, `test_simple_api.py`, `test_simplifications.py`, retrieved with
`git show 39f288f:python/tests/<file>`), adapted to the new layout, plus parity tests for all L1 cases,
and:
- duck typing: coefficient classes with only `__float__`, only `__index__`, `Fraction`, and a fake
  `__marty_expr__`; chains passed as tuples, lists and generators
- operator semantics: `-e`, `e - e == 0`, `0 == eps(0,0,1,3)` (reflected comparison), `2 * g(0,1)`
- iteration and indexing (`len`, `[i]`, out-of-range raises `IndexError`, not a core error)
- `with symbolic_dimension():` restores the flag on exit and on exception
- `copy`, `deepcopy`
- `_repr_latex_` and `to_latex` agree with C++ strings from golden data
- independence: importing `pymarty.diracology` in a subprocess where `pymarty.modeling`,
  `calculation`, `simulation`, `group_theory` imports are blocked succeeds

L4: notebook `python/examples/diracology_basics.ipynb` (revived from the `api/gamma` branch notebook),
executed by `nbmake` or equivalent in the examples job, no output change allowed without review.

## 5. Acceptance criteria

1. Every case of `tests/integration/src/gammaAPI.cpp` has a Python counterpart through the parity
   checker (exemptions need a reason).
2. The four old Python test files are ported, and every assertion of them is present or has a
   documented reason for dropping.
3. Mathematical identity checks listed above pass in C++ and in Python.
4. The binding contains no algebra: the diff review shows that `python/bindings/diracology.cpp` has no
   code that calls `sgl::` or `csl::` directly (everything goes through the C++ API). The old
   binding's `sgl_to_csl` comparison code has moved into the API.
5. State neutrality: `set_symbolic_dimension` and the context manager leave the flag as found after
   any test (fixture check), including after errors.
6. Independence test passes. No import of other pymarty modules in the module's source.
7. Invalid-input matrix: for each throw site in `gamma.cpp` there is a Python test that triggers it and
   checks both the exception class and the message.
8. Existing `ctest -L integration` still green (including the unchanged `gammaAPI` test).
9. Stubs and doctests pass. Quality gates pass.
10. `python/examples/usage/diracology.py` was approved by a human before implementation started (reviewer and
    date in the PR), is unchanged since or its changes were re-approved, and runs in
    `test_usage_examples.py` (L4b).

## 6. Exit artifacts

Stubs, notebook, docstrings with examples, parity mapping for the legacy test file, a CHANGELOG entry
on the relation to the earlier `api/gamma` prototype.

## 7. Risks and open questions

- Equality semantics in the old binding went through `sgl_to_csl` (equality up to canonicalisation).
  Is it consistent with `__hash__`? Decision: `DiracExpr` is unhashable unless a consistent canonical
  key is available.
- SGL asserts or crashes on unusual input: found by the invalid-input matrix; fixes belong in the API.
- Index conventions (ints only) may be too limited for later phases. The extension to named indices is
  a C++ API addition and is deferred until a use case requires it (Phase 4 and 5 will tell).
