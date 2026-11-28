# pymarty finalization: plan after phase 7

Status: plan only (2026-11-28). No code changed by this work. Decisions are pending in
[02-questions.md](./02-questions.md).

This folder merges three sources into one work plan:

- [phase-8-polish-release.md](../phase-8-polish-release.md): stubs, docs, notebooks, `doctor`, wheels, release.
- [USER_FEEDBACK.md](../USER_FEEDBACK.md): layout, usage examples, API consistency, docs, delivery.
- Remaining work recorded in phases 4 to 7 (phase 7 spec "Remaining work", phase 4 and 5 "deferred" sections,
  and the `todo/` notes).

The evidence and status of every item are in [01-inventory.md](./01-inventory.md).

## Order of work

The order follows the "Proposed Process" of [USER_FEEDBACK.md](../USER_FEEDBACK.md), with gap filling
moved after the API polish, as requested. Each step is a separate PR and ends with a human review gate.

| Step | Work | Depends on | Gate |
|------|------|-----------|------|
| F1 | Restructure the layout (move only, no behaviour change): `src/pymarty`, `tests/python`, `usage_examples/`, `builtin_models/`, `dep/` to `third-party/` | none | all suites green after the move, same test counts |
| F2 | Usage examples: fewer, richer examples; all examples as notebooks (Q5), every example run in CI, C++ and Python | F1 | human review of the examples |
| F3 | API polish: remove namespace classes from the stubs, consistent structure, typing (`mypy --strict` clean), naming, error and output conventions, C++/Python duplication audit | F1, F2 | human review of the API surface |
| F4 | Documentation: user guide (install, getting started, module pages from docstrings with executed examples) and a developer page listing remaining gaps | F3 | human review |
| F5 | Delivery: wheel matrix, `doctor`, versioning and deprecation policy, licences and notices, wheel test CI, release rehearsal, local development path | F1, F4 | release rehearsal log |
| F6 | Gap filling: phase 7 remaining items, deferred ports (FItests, MSSM family, Bgenesis, freeQuarkAmplitudes, gm2SM, BsMixing, wilsons_GTHDM), inline diagrams, concurrency | F3 (API stable) | per-item acceptance from the phase documents |

Why F6 is last: gap filling adds public API (Wilson operators, LHA, MSSM, spectra). Doing it after F3
means the new functions follow the polished conventions instead of being retrofitted.

## Scope cut

Phase 8 items that are not needed for a first release are marked "optional" in the inventory (GIL release,
cooperative cancellation, inline diagrams). They are done only if the answers in
[02-questions.md](./02-questions.md) include them.

## Artefacts

- [01-inventory.md](./01-inventory.md): consolidated remaining work, with status and `file:line` evidence.
- [02-questions.md](./02-questions.md): decisions needed from the user before F1 to F5 are specified.
- Specs, one per step, following [../process/README.md](../process/README.md):
  - [03-spec.md](./03-spec.md): F1, READY, awaiting G1 approval.
  - [04-spec-f2-examples.md](./04-spec-f2-examples.md), [05-spec-f3-api.md](./05-spec-f3-api.md),
    [06-spec-f4-docs.md](./06-spec-f4-docs.md), [07-spec-f5-delivery.md](./07-spec-f5-delivery.md),
    [08-spec-f6-gaps.md](./08-spec-f6-gaps.md): DRAFT. Not frozen. Each is revised after the previous gate.
