Status: APPROVED
Updated: answers recorded, pending G1 spec approval
Author: orchestrator

# Phase 5 questions

Evidence is in [01-investigation-summary.md](01-investigation-summary.md). Answers are recorded below as they come.

## Q1. Wilson coefficients in phase 5 or phase 7?

Context: the request lists Wilson among the objects to port in phase 5. The plan puts Wilson in phase 7 (7b, after
7a loops), and all Wilson examples in the repository are one-loop (`BsMixingSM`, `C9_SM`, `sample2HDM`).

Options:
- A. Keep Wilson in phase 7 (recommended). Phase 5 covers amplitudes, squared amplitudes, FeynmanRules and options.
- B. Phase 5 adds a tree-level Wilson API only. Its value is unverified: no tree-level Wilson example was found.
- C. Phase 5 adds the Wilson API, with one-loop calls rejected until phase 7. Adds surface with no test coverage.

Recommendation: A. It keeps phase 5 testable against C++ output and leaves loops in one phase.
Blocks: section 4 of the spec (Wilson API) and the acceptance criteria.

## Q2. Where does the calculation API live, and what does `Model` return?

Context: the request forbids extending `modeling.Model` with calculation methods. The plan currently specifies
`Model.amplitude(...)`. `Model.finalize()` returns nothing today (`python/src/modeling.cpp:534-540`).

Options:
- A. Module `pymarty.calculation` with functions that take the model explicitly: `amplitude(model, order, insertions, ...)`,
  `squared(model, amp)`. `finalize()` is unchanged (recommended).
- B. A calculation object `Calculation(model)` in `pymarty.calculation`, with the same methods. Same as A, but stateful.
- C. `finalize()` returns a calculation object (the idea of the request). Changes the accepted phase 4 return type
  and couples modeling to calculation. The simulation package does not exist yet (phase 6).

Recommendation: A, with B as the object if you prefer a method style. C can be added later, once phase 6 exists.
Blocks: the usage example `python/examples/usage/calculation.py` and the plan document.

## Q3. Visualization (GRAFED export and launcher) in phase 5?

Context: the plan includes WP5.6 to WP5.8. The request does not mention visualization. The legacy `Show()` writes
`/tmp/graphs.json` and calls `system()` (`src/marty/core/drawer.cpp:162-163`). Fixing it changes C++ behaviour.

Options:
- A. Defer visualization to a separate phase (recommended). Phase 5 stays on calculation, as the request says.
- B. Include JSON export only (`diagrams_json`, `rules_json`, `save`), with no launcher.
- C. Include the full WP5.6 to WP5.8 package as the plan says.

Recommendation: A. It keeps the phase focused and avoids a C++ `Show()` rewrite in this phase.
Blocks: the visualization section of the plan and the `visualization` module.

## Q4. Custom filters written as Python callbacks

Context: the C++ `LagrangianFilter` and `DiagramFilter` are `std::function` (`src/marty/core/feynOptions.h:54, 62`).
The request says callbacks cannot be bound. Data filters (`disableParticles`, `forceParticle`, `forbidParticle`) are
expressible as particle lists.

Options:
- A. Data filters are converted to C++ filters. Python callbacks are not supported in phase 5 (recommended).
- B. Data filters in C++, plus Python diagram-level callbacks applied to the returned diagrams after C++ returns.
  Lagrangian-level callbacks are not supported. Results would differ from C++ filters: documented limitation.
- C. Python callbacks run during generation through a C++ hook. Not feasible without binding `std::function`, which you ruled out.

Recommendation: A. Option B may be added later; it is not equivalent to the C++ result and needs its own tests.
Blocks: the filter API and the tests `FilterForceAndDisable`.

## Batch 1 answers (2026-11-23, user)

- Q1 Wilson: **A**. Wilson coefficients stay in phase 7.
- Q2 Calculation API: **A**. `pymarty.calculation` functions take the model explicitly; `modeling.Model` is not extended; `finalize()` is unchanged.
- Q3 Visualization: **C**. The full WP5.6 to WP5.8 package is in phase 5 (JSON export, `save`, GRAFED locator and launcher, C++ `Show()` reimplemented without `/tmp/graphs.json` or `system()`).
- Q4 Filters: **B**. Data filters are converted to C++. Python diagram-level callbacks are applied to the returned diagrams. Lagrangian-level callbacks are not supported (documented limitation).

## Batch 2 answers (2026-11-23, user)

- Q4 follow-up (effect of a Python diagram filter on the expression): **A**. The expression is unchanged; the filter selects
  which diagrams are returned and shown. Documented.
- Q5 FeynmanRules: **B**. Read-only view plus vertices and propagators as data objects.
- Q6 Examples acceptance set: **A**. QED Compton, e+e- to mu+mu-, `sampleSQED`, `sampleQCDSextet`, Nf flavour (tree part),
  `gg_to_tt` tree part. One-loop parts are phase 7.

Status: all questions answered. The spec and the usage example are the next step (G1). must drop `Model.amplitude` and `Model.squared` (section 1 and section 3), and the
Wilson items move to phase 7. The phase 7 document must list Wilson as its own item (7b already does).
