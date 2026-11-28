Status: ANSWERED (Q0 to Q3 recorded; Q4 to Q6 open)
Updated: 2026-11-20
Author: orchestrator

# Phase 2 (`pymarty.diracology`): questions for the user

Evidence: [01-investigation-gamma-cpp.md](01-investigation-gamma-cpp.md),
[01-investigation-old-binding.md](01-investigation-old-binding.md),
[01-investigation-python-wiring.md](01-investigation-python-wiring.md).

## Q0. Prerequisites (blocking)

Phase 2 needs Phase 0 accepted. The repository has no G3 record for Phase 0 (no `phase-0/` folder, no
acceptance in the docs or git log). Phase 1 is at G3 and not accepted yet (`phase-1/04-implementation-log.md`).

Options:
- A. Keep Steps 1 and 2 (investigation, Q&A, spec) for Phase 2 now. Hold any implementation until Phase 0 and
  Phase 1 are accepted. **Recommended**: the process says no implementation before the previous phase is
  accepted (`phase-N` plan section 6, `process/README.md` section 5).
- B. Start implementation now, in the same tree, with Phase 0 and 1 still open.
- C. You confirm that Phase 0 is accepted (and say when). Phase 1 still blocks implementation if you keep the rule.

Answer (user, 2026-11-20): "0 & 1 are done & accepted. Proceed with 2." Phase 0 and Phase 1 are accepted.

## Q1. `GammaAPIError` versus `mty::api::Error`

`GammaAPIError` derives from `std::exception`, not from `mty::api::Error` (`src/marty/api/gamma_api_error.h:6`).
`sgl::MathError` escapes untranslated and is asserted in `tests/integration/src/gammaAPI.cpp:411-417`.

Options:
- A. Keep `GammaAPIError` unchanged (public behaviour of `gamma.h` stays). The new `diracology` API translates
  `GammaAPIError` and `sgl::MathError` into `mty::api::ValueError` / `NotImplementedError` at its boundary.
  **Recommended**: `api-conventions.md` section 1 rule 11 (existing headers stay intact).
- B. Make `GammaAPIError` derive from `mty::api::Error`. Changes the existing class, needs the `common/error.h`
  dependency in `gamma_api_error.h`.

Answer (user, 2026-11-20): A.

## Q2. Equality and hashing of `DiracExpr`

The old binding compares through `sgl_to_csl` (`origin/api/gamma:src/pybinding/gamma_api.cpp:64-115`).

Options:
- A. `__eq__` by canonical comparison in C++; `__hash__` = None (unhashable). **Recommended**: the plan's
  section 7 default; no hash consistent with canonical equality is available without a canonical key.
- B. Add a C++ canonical string key and hash on it (new API function, more work, risk of inconsistency).

Answer (user, 2026-11-20): A.

## Q3. Fierz return format and names

The old binding returns a dict `{"rhs", "lhs"}` from `single_fierz` / `double_fierz`, and hard-codes indices 1..4 in
the list overloads (`origin/api/gamma:src/pybinding/gamma_api.cpp:201-251`). It names the enum `Standard` / `Chiral`
and has `/` and `print_latex` (not in the plan). The plan names them `STANDARD` / `CHIRAL`, has no `/`, and
does not mention `print_latex`.

Options:
- A. Follow the plan: `FierzBasis.STANDARD` / `CHIRAL`, `single_fierz` returns the `(rhs, lhs)` pair as a
  named tuple with the same keys, indices passed explicitly. Drop the hard-coded 1..4. **Recommended**.
- B. Keep the old names and dict return exactly (compatibility with the notebook and old tests).

Answer (user, 2026-11-20): A.

## Q4. Phase 1 deferred items A1 to A4 (new, from the Phase 1 log)

The Phase 1 port table defers 44 cases to Phase 2 (`phase-1/port-table.md`, rows marked `deferred to phase 2 (A1..A4)`).
They cover matrix printing (A1), non-commuting symbols (A2), LaTeX index names (A3) and signed indices (A4):
generic CSL printing, not Dirac algebra.

Options:
- A. Include them in Phase 2 (larger scope; needs CSL printing changes, which the layering rules allow).
- B. Leave them for a later phase and record that in the port table. **Recommended**: the Phase 2 plan does not list them.

Answer (user, 2026-11-20): B.

## Q5. Symbolic coefficients

The plan allows symbolic objects with `__marty_expr__` as coefficients. The SGL path for them is not investigated.

Options:
- A. In scope: investigate and implement in this phase.
- B. Out of scope: `NotImplementedError` with a clear message, documented. **Recommended** until the investigation is done.

Answer (user, 2026-11-20): reply "Can reuse pymarty.symbolic?". Decision: a WP1 spike checks whether SGL coefficients accept a CSL expression. If yes, `__marty_expr__` objects are supported (duck typing only, no import of `pymarty.symbolic` in the module). If no, `NotImplementedError` with a documented message.

No `CHANGELOG*` file exists at the repository root, but the plan's exit artifacts ask for a CHANGELOG entry.

Options:
- A. Skip the CHANGELOG entry and note it in the implementation log.
- B. Create `CHANGELOG.md` with this entry.

Answer (user, 2026-11-20): A, skip the CHANGELOG entry and note it in the implementation log.
