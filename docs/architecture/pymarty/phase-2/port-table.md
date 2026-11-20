# Phase 2 port table: old `pymarty.gamma_api` tests to `pymarty.diracology`

Source: the four old Python test files on the `api/gamma` branch (commit `39f288f`):
`python/tests/{test_import,test_objects,test_simple_api,test_simplifications}.py`.
Scope rules: [03-spec.md](03-spec.md) sections 6 and 7 (acceptance 2, and keep/drop rules).

Status values: `ported` (a named test checks the same behaviour), `ported (parity)` (the behaviour is
checked by a `marty_gamma_api` C++ case, mapped by `@pytest.mark.parity`), `dropped` (reason given).

## test_import.py

| Old test | Status | Destination or reason |
|----------|--------|-----------------------|
| `test_import_basics` (`Expr`, `FierzBasis`, `set_symbolic_dimension`) | ported | `test_diracology_api.py::test_import_without_other_pymarty_modules`; `Expr` is `DiracExpr`; `set_symbolic_dimension` is kept as a plain setter, and the `symbolic_dimension` context manager is preferred (spec section 4) |
| `test_import_objects` (`g`, `eps`, `gamma`, `P_L`, `P_R`, `gamma5`, `C`, `D`) | ported | same test |
| `test_import_functions` (`current`, `trace`, `single_fierz`, `double_fierz`) | ported | same test; `print_latex` dropped: spec section 4 excludes it (`to_latex()` replaces it) |

## test_objects.py

| Old test | Status | Destination or reason |
|----------|--------|-----------------------|
| `test_basis` (`FierzBasis.Chiral != Standard`) | dropped | checks enum inequality, which is language behaviour |
| `_test_same`, `_test_diff` (`<`, `<=`, `>`, `>=`) | dropped | `DiracExpr` has no ordering in the Python API (spec section 4 lists `==` only); the C++ `less` is not bound |
| `_test_zero_len` (`len`, indexing) | dropped | `size` and `argument` are C++ introspection, not bound in Python |
| `test_simple_expr` (`Expr(0)`, `Expr(1.5)`, `D` equality) | dropped | the `Expr` constructor is not exposed; zero comparisons are in `test_diracology_api.py::test_zero_comparisons_with_numbers`; `D` equality is in `test_gamma_api.py::test_equality_distinguishes_structures` |

## test_simple_api.py

| Old test | Status | Destination or reason |
|----------|--------|-----------------------|
| `test_set_symbolic_dimension` | ported | `test_diracology_api.py::test_symbolic_dimension_is_restored_after_exception`, and `test_gamma_api.py::test_equality_distinguishes_structures` (`D != 4` under `keep=True`, `D.simplify() == 4` under `keep=False`) |
| `test_basic_expressions`: `g` and `eps` symmetry and equality | ported (parity) | `marty_gamma_api.metric_contractions`, `marty_gamma_api.epsilon_contractions`, `test_gamma_api.py` |
| `test_basic_expressions`: `eps(0,0,1,3) == 0`, `!= 0` | ported | `test_diracology_api.py::test_zero_comparisons_with_numbers` |
| `test_basic_expressions`: currents compare unequal when they differ | ported | `test_gamma_api.py::test_equality_distinguishes_structures` |

## test_simplifications.py

| Old test | Status | Destination or reason |
|----------|--------|-----------------------|
| `test_basis` | dropped | see `test_objects.py` above |
| `test_traces`: `Tr(γ0γ1)`, projector traces, `Tr(γ5)`, `Tr(γ0γ1γ2γ3γ5)` | ported (parity) | `marty_gamma_api.simple_traces`, `marty_gamma_api.complex_traces` |
| `test_simplifications`: `γ0γ0`, `CC`, `γ5` with projectors, `P_R P_L = 0`, `γ0γ1γ0`, `order()` of `γ5 γ0` | ported (parity) | `marty_gamma_api.simple_simplification`, `marty_gamma_api.gamma_contractions`, `marty_gamma_api.conjugation_identities`; `test_gamma_api.py::test_simple_simplification` |
| `test_simplifications`: `project(basis=CHIRAL)` of `γ01 γ5` | ported (parity) | `marty_gamma_api.basis_projection` |
| `test_simplifications`: `single_fierz` and `double_fierz` with explicit currents | ported (parity) | `marty_gamma_api.simple_fierz_identities`, `marty_gamma_api.double_fierz_identities` |
| `test_simplifications`: `single_fierz` and `double_fierz` with lists and fixed indices 1..4 | dropped | hard-coded indices (spec section 7, Q3 answer A); the explicit-index forms above replace them |
| `print(...)` calls | dropped | output checks, not assertions |

## Identity list (spec section 6, criterion 3)

| Identity | Status | Destination |
|----------|--------|-------------|
| `gamma5()*gamma5() == 1` | adapted | bare `gamma5()` is rejected outside a chain (RuntimeError). Checked as `current([gamma5(), gamma5()], 0, 1).simplify() == current([], 0, 1)` in `python/pymarty/diracology.py` (doctest) and `test_gamma_api.py` |
| `P_L()*P_R() == 0`, `P_L()*P_L() == P_L()` | ported | `test_diracology_api.py::test_projector_and_trace_identities` |
| `P_L()+P_R()` is not simplified to `1` | deliberate | spec section 6, criterion 3: canonical chiral decomposition |
| `trace([gamma(0), gamma(1)]) == 4*g(0,1)`, odd trace is 0 | ported | `test_diracology_api.py::test_projector_and_trace_identities`, `test_gamma_api.py::test_simple_traces` |
| `eps(0,1,2,3)*eps(0,1,2,4) == -6*g(3,4)` | ported | `test_gamma_api.py::test_epsilon_contractions` |

## Invalid-input matrix (spec section 6, criterion 7)

Reachable throw sites in `src/marty/api/diracology.cpp`:

| Throw site | Python test |
|------------|-------------|
| `requireNonNegative` (negative index) | `test_diracology_api.py::test_negative_indices_are_value_errors`, `test_negative_index_message_names_the_index` |
| `translateFierzErrors` (bare gamma matrices in Fierz) | `test_diracology_api.py::test_fierz_on_bare_gamma_matrices_is_value_error` |
| non-pure current in Fierz (`GammaAPIError`, sum argument) | `test_diracology_api.py::test_fierz_on_sum_argument_is_value_error` |
| `fromDouble` (non-finite scalar) | `test_diracology_api.py::test_nan_scalar_is_value_error`, `test_infinite_scalar_is_value_error` |

Not reachable from Python, so no Python test:

| Throw site | Reason |
|------------|--------|
| `argument()` out of range (`LookupError`) | `DiracExpr` exposes no argument accessor; pinned by the C++ test `DiracologyApi.ArgumentOutOfRangeIsLookupError` |
| `fromRational` zero denominator | `fractions.Fraction` cannot hold a zero denominator |
| null expression, and the other non-pure-current branches | the binding never builds a null `DiracExpr`; the branches are unreachable through the typed Python constructors |

Trace errors (`trace([g(0,1)])`, `trace([C()])`) are raised by the algebra core as `sgl::MathError`
and reach Python as `MartyValueError`. `rethrowAsApiError()` converts `sgl` and `std` exceptions to
typed API errors, so no bare `RuntimeError` escapes the binding. Tested in
`test_diracology_api.py::test_trace_of_non_gamma_index_chain_is_value_error` and `test_errors.py`.
