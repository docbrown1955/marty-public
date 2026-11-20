# Phase 3 port table: `test_flavor_groups.cpp` to `pymarty.group_theory`

Source: the group-theory part of `tests/api/src/test_flavor_groups.cpp`. The flavour (U(1)) charge part
is ported to `python/tests/group_theory/test_flavor_groups_group_part.py`. Scope and acceptance
criteria: [phase-3-group-theory.md](../phase-3-group-theory.md).

Status values: `ported` (a named Python test checks the same behaviour), `ported (C++ gtest)` (the
behaviour is checked by a `GroupTheoryApi` gtest, mapped to Python by `@pytest.mark.parity`), `dropped`
(reason given).

| Source behaviour | Status | Destination or reason |
|------------------|--------|-----------------------|
| SU(n) fundamental and adjoint dimensions, Casimir | ported | `test_group_theory_api.py` (SU(3) decompositions, Casimir 4/9) |
| SU(3) fundamental product `3 x 3bar = 1 + 8` | ported (C++ gtest) | `GroupTheoryApi.*` decomposition tests; `test_group_theory_api.py` |
| Dynkin index `T(fund SU(3)) = 1/6` | ported | `test_group_theory_api.py`, `examples/usage/group_theory.py` (convention chosen by the user) |
| U(1) charge group construction | ported | `test_flavor_groups_group_part.py` (3 tests) |
| SO(10) spinor and vector products | ported (C++ gtest) | `GroupTheoryApi` decomposition tests |
| Sp(n), F4, E6, E7, E8, G2 decompositions and dimensions | new coverage | `GroupTheoryApi.SymplecticAndExceptionalDecompositions`, `SymplecticDimensionIsValidated`, `ConjugationFollowsDiagramAutomorphism` |
| SO(3) | dropped | the core aborts the process; the facade requires SO(n) with n >= 4 |
| U(1) Casimir and index | dropped | the core segfaults or throws an uncaught error; the facade raises `NotImplementedError` |

The literal decomposition tables are covered by the parity tests. No separate golden-data file is
produced.
