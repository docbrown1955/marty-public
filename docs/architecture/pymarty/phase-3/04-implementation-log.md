Status: COMPLETE, pending user acceptance and commit.

# Phase 3 implementation log

Scope and acceptance criteria: [phase-3-group-theory.md](../phase-3-group-theory.md). Old-to-new test
mapping: [port-table.md](port-table.md). Capability record: [../spike-3-0-group-capabilities.md](../spike-3-0-group-capabilities.md).

## Packages

| Package | Title | Status |
|---------|-------|--------|
| WP3.1 | C++ `mty::api::group_theory` facade and gtests | done |
| WP3.2 | `pymarty.group_theory` binding, wrappers, stubs, parity tests | done |
| WP3.3 | Port of the U(1) charge part of `test_flavor_groups.cpp` | done |
| Families | Sp(n), E7, E8 (untested), F4, G2 added to facade, binding and tests | done |
| Core fix | `Irrep::getConjugatedRep` per algebra type | done |
| Exit artifacts | usage example, `group_theory_basics.py`, port table, this log | done |

## Decisions recorded during implementation

- Dynkin index convention: `T(r) = C2(r) dim(r) / dim(G)`, so `T(fund SU(3)) = 1/6` (user decision).
  The usage example asserts 1/6.
- Casimir convention: the core convention (user decision, spike 3.0).
- The core `Irrep::operator==` and `<` compare dimensions only. The facade keys on highest weights, and
  `isSelfConjugate` compares highest weights, not core equality.
- Core conjugation was wrong for every algebra except SU(n): it reversed the Dynkin labels. The fix
  (user decision A) is per algebra type in `src/marty/core/representation.cpp`:
  A reverses; D of odd rank swaps the spinor nodes; E6 swaps 0 and 4 and 1 and 3; B, C, E7, E8, F4, G2
  are self-conjugate. Probed before and after: Sp(4) 4 to 5 became 4 to 4; SO(10) 10 to 16 became 10 to 10;
  E6 27 to 78 became 27 to 27bar; G2 7 to 14 became 7 to 7.
  The same helper is used by model field conjugation, so non-SU model behaviour may change. System tests were
  not run (the policy forbids the system suite in this session); this must be checked by the maintainer.
- G2 `fundamental()` is a facade override returning `highestWeightRep({1,0})`. The core default was not used.
- Sp(n) requires even fundamental dimension n >= 4 in the facade. The core `C()` needs rank > 1.
- U(1) Casimir and index raise `NotImplementedError` in the facade, because the core segfaults.
- SO(3) is rejected in the facade, because the core aborts the process.
- Group dimension: `Group::getDim()` returns 1 for exceptional groups, so the facade uses the adjoint dimension.
- Integer conversion: csl `Integer::getNum()` returns 0, so `toRational` uses `std::llround(evaluateScalar())`.

## Known defect (not fixed, flagged)

- E8: the core gives dimension 3875 for both `getDefiningRep()` and `getAdjointRep()`. The adjoint should be 248.
  E8 is exposed in the API and binding but has no tests (user decision). A core fix needs a separate decision.
- Spike record: the E6 27 x 27bar row in the spike (`27 = 27 + 351 + 1728`, dim 2106) was wrong. The correct
  decomposition is 1 + 78 + 650 (dim 729). See the correction note in the spike record.

## Verification

- `ctest -L 'units|integration|api|python'`: 347/347 passed. Never ran the `system` suite.
- `pytest -m "not slow"` on `python/tests`: 380 passed.
- Gates: `ruff check`, `ruff format --check`, `mypy --strict pymarty tests tools` and parity
  (116 C++ API tests: 94 with a Python counterpart, 22 exempted) all pass. clang-format is clean on the edited C++ files.
- `python/examples/group_theory_basics.py` runs and passes (SU(2), SU(3), SU(5), Sp(4), F4, E6, E7, G2 dimensions,
  SU(5) 5 x 5bar, E6 27 x 27bar).
