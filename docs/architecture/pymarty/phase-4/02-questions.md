Status: ANSWERED
Updated: 2026-11-21
Author: orchestrator

# Phase 4 (`pymarty.modeling`): questions for the user

Evidence: [01-investigation-core-model-api.md](01-investigation-core-model-api.md),
[01-investigation-model-current-audit.md](01-investigation-model-current-audit.md),
[01-investigation-tests-examples-models.md](01-investigation-tests-examples-models.md),
[01-investigation-bindings-conventions.md](01-investigation-bindings-conventions.md).

## Q0. Prerequisites (answered)

Phase 4 needs Phases 1 and 3. Phase 1 is accepted. Phase 3 is "COMPLETE, pending user acceptance"
(`phase-3/04-implementation-log.md:1`).

Answer (user, 2026-11-21): "0-2 are accepted, I'm finalizing 3 with another agent in parallel just for the
approval but implementation should be done. Start 4". Recorded as: Phase 4 steps 1 to 3 proceed; implementation
waits for G1 (this document's gate, not Phase 3 acceptance).

## Q1. Scope of built-in models

The plan lists `QED`, `SM`, `THDM`, `MSSM`, `PMSSM`, and says others (GTHDM, NMFV, QCD) only if public and tested.
The C++ classes are `QED_Model`, `SM_Model`, `THDM_Model<type>` (a template), `MSSM_Model`, `PMSSM_Model`
(`src/marty/models/*.h`, see investigation tests-examples-models, facts 5 and 7).

Options:
- A. The five models of the plan; `gthdm`, `nmfv`, `qcd` deferred to Phase 5 or later. **Recommended**: matches the plan.
- B. Also `QCD` and `GTHDM` in this phase (more tests, more golden data).
- C. Only `QED` and `SM` in this phase; `THDM`, `MSSM`, `PMSSM` in a follow-up (smaller, faster review).

Question for the user: which option?

## Q2. Which THDM variant

`THDM_Model<type>` is a template (`src/marty/models/thdm.h:48-67`). The Python `THDM()` needs one fixed choice.

Options:
- A. Expose the variant that `examples/sample2HDM*` or the tests use most (to be checked in the spec); the others deferred. **Recommended.**
- B. Expose each variant under its own name (`THDM_I()`, `THDM_II()`, ...), whichever the C++ code defines.

Question for the user: which option, and which variant if A?

## Q3. Particle handles: sharing or copies

The core `Particle` is a `shared_ptr`. A `ParticleHandle` could (A) refer to the same particle as the core, so
`rotate_fields` or `replace` seen through one Python object also changes others; or (B) hold a deep copy at
`add_particle` time and be read-only afterwards.

Options:
- A. Shared handles, as in the core. Matches the core and the gettingStarted flow (`Q_L.set_rep(...)` after
  `add_particle` is not needed, but `W_1 = model.particle("W_1")` must see the model state). **Recommended** by
  the plan (section 7, "handles refer to the same particle").
- B. Copies: simpler immutability, but a particle obtained from `model.particle(...)` no longer tracks later
  changes made by `rotate_fields`, which would surprise users.

Question for the user: confirm A (plan default) or choose B.

## Q4. Spacetime objects in this phase

The plan puts `pymarty.modeling.spacetime` (Minkowski and Dirac indices, `gamma`, `P_L`, `P_R`, metrics) in
scope because terms need them. `pymarty.diracology` (Phase 2, accepted) already wraps gamma matrices.

Options:
- A. Build `spacetime` on top of `symbolic` (Phase 1) and `diracology` only as needed; implement it in this phase. **Recommended**: the plan's scope.
- B. Defer `spacetime` to a later phase; the usage example then uses only `symbolic` objects and the QED/SM models
  (which need no user-written terms).

Question for the user: confirm A or choose B.

## Answers (user, 2026-11-21)

- Q1: **C**. Only `QED` and `SM` in this phase. `THDM`, `MSSM` and `PMSSM` move to a later phase.
- Q2: "See later". Moot under Q1=C; the THDM variant is decided with the THDM phase.
- Q3: **A**. Shared handles, as in the core.
- Q4: **A**. `pymarty.modeling.spacetime` is in scope.

## Blocking

None after the answers above. The spec (03-spec.md) is the next gate (G1).
