Status: READY
Updated: 2026-11-27
Author: orchestrator

# Phase 7 spec (step 3): loops, Wilson coefficients, widths

Inputs: [01-investigation-summary.md](./01-investigation-summary.md), [02-questions.md](./02-questions.md).
Plan: [phase-7-loops-wilson-widths.md](../phase-7-loops-wilson-widths.md).

## Baseline

| Check | Result |
|-------|--------|
| Build | `/tmp/pymarty-build`, GCC 11.4 (GCC 12 not installed; AGENTS.md accepts 11+), Release, `MARTY_BUILD_PYTHON=ON` |
| `ctest -L 'units\|integration\|api'` | 385 of 385 passed |
| Fast Python suite `-m "not slow"` | Red before the fix below: 5 failed, 1 error, all from a missing `MARTY_INCLUDE_PATH` |
| Same tests with `MARTY_INCLUDE_PATH=/workspaces/marty-public/src MARTY_LIBRARY_PATH=/tmp/pymarty-build` | 14 passed, 4 deselected (no regression) |

The baseline failures are environmental, not code regressions. Phase 7 gates must set both variables.

## Decisions (from Q&A)

| # | Question | Decision |
|---|----------|----------|
| Q1 | LHA value type | Real values only (`lha.h:39-72` and `lhaData.h:33-40` store `long double`); the plan's complex return is dropped. |
| Q2 | Scope | 7a to 7e, plus Bgenesis and freeQuarkAmplitudes from 7f. Boltzmann plugin deferred (layering conflict: it uses GRAFED's `jsonLoader.h`). |
| Q3 | Quad gating | Add `Config.quad_available`. C++ gate is `QUADRUPLE_ENABLED` (`tests/system/CMakeLists.txt:18-25`), not `MARTY_QUADRUPLE_ENABLED`. |
| Q4 | Wilson options | Keyword arguments mirroring the C++ options. |

## Scope

- 7a one-loop: `Order.ONE_LOOP` in `compute_amplitude` (already works: QED psi A -> psi A gives 18 diagrams, probed in this phase).
- 7b Wilson: `wilson_coefficients` and `chromo_magnetic_coefficient` for the C7/C8/C9/BsMixing/GTHDM ports; `gm2SM` and `gm2PMSSM` are one-loop, not tree level.
- 7c widths: `model_width` and `widths_sm` were ported in phase 6. Added `compute_width(model, particle, order, options)` in `pymarty.calculation`: a facade `Model::computeWidth` over the core two-body width, returning a symbolic `Expr`. Numeric values stay in the compiled library path.
- 7d LHA: `read_lha` returns `Mapping[str, Mapping[tuple, float]]`; test_lha_reader port.
- 7e quad: `Config.quad_available`; quad tests skip when disabled.
- 7f: Bgenesis and freeQuarkAmplitudes (no CTest; freeQuarkAmplitudes has commented-out CTest at `tests/system/CMakeLists.txt:36-37`). Already done in phase 6: c7_sm, c8_sm, widths_sm, model_width. Moved to the C++ integration suite: majoranaLoop, testConjugation, NLepto. `test_fullMSSM.cpp` has no reference comparison and is not in the system list; excluded.

## Postponed

- SUSY models (`C7_PMSSM`, `C8_PMSSM`, `C9_MSSM`, `gm2PMSSM`, `test_fullMSSM`, NMFV) wait for model (de)serialization, see [model-serialization.md](../../todo/model-serialization.md). They are very long to build.
- Rule: a model whose C++ system test takes more than about 15 min to build is not ported to pymarty in this phase.

Build times come from `CTestCostData.txt` in existing build trees: C7_SM 7 to 30 s, C8_SM 7 to 33 s, C9_SM 12 to 91 s, GTHDM Wilson 21 to 103 s, C7/C8/C9 PMSSM 55 to 330 s. None of these exceeds 15 min, so the rule does not block them. `BsMixing*`, `gm2SM`, `Bgenesis` and `freeQuarkAmplitudes` have no recorded time and must be measured before porting.

## Python API

- `compute_amplitude(model, Order.ONE_LOOP, insertions, kinematics=None, options=None)`: existing signature.
- `wilson_coefficients(model, amplitude, operator_coefficient=None)`: existing signature, returns `WilsonSet`.
- Wilson options as keyword arguments (Q4), named after the C++ options; no new positional parameters.
- `read_lha(path) -> Mapping[str, Mapping[tuple, float]]`.
- `Config.quad_available: bool`, read from the build configuration.
- Stubs (`_pymarty.pyi`, `symbolic.pyi`) and wrappers must agree when a binding changes (AGENTS.md).

## Usage example

`python/examples/usage/calculation.py` sections 12 and 13 (added in this phase):

```python
one_loop = compute_amplitude(qed, Order.ONE_LOOP, [incoming(PSI), incoming("A"), outgoing(PSI), outgoing("A")])
wilsons = wilson_coefficients(qed, one_loop)
```

Verified: the example exits 0, prints `one-loop diagrams: 18` and `Wilson coefficients type: WilsonSet`, and passes `ruff check` and `ruff format --check`.

## Work packages

1. WP-7a: one-loop Python tests (`python/tests/calculation/`), no new C++.
2. WP-7b: Wilson ports for the remaining system tests, with reference data from `tests/system/data/output/`.
3. WP-7d: `read_lha` binding and LHA test.
4. WP-7e: `Config.quad_available` and quad skip markers.
5. WP-7f: Bgenesis and freeQuarkAmplitudes ports.

## Acceptance

- `ctest -L 'units|integration|api'` green.
- `ctest -L python` green with both `MARTY_INCLUDE_PATH` and `MARTY_LIBRARY_PATH` set; `-m "not slow"` kept.
- `ruff check .`, `ruff format --check .`, `mypy --strict pymarty tests tools` green.
- `check_parity.py` green.
- Never run `ctest -L system`, unfiltered `ctest`, or pytest without `-m "not slow"`.

## Risks

- GCC 11 instead of 12 for the baseline; re-check on GCC 12 if the user requires it.
- Build takes about 27 min with -j8.
- Reference data for the Wilson ports must match the C++ output; any mismatch is reported, not edited.
