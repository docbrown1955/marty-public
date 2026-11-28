Status: DRAFT (not frozen; revised after the F3 gate)
Updated: 2026-11-28
Author: main agent (orchestrator)
Depends on: F3 (API stable). F4 and F5 are not required, but the release is cut after F5.

# F6 spec: gap filling

Scope: the open features of phases 4 to 7, in the order decided by Q7. Each item adds public API, so each one is
written against the polished conventions of F3.
Source: [01-inventory.md](./01-inventory.md) section E, [02-questions.md](./02-questions.md) Q7.

## 1. Decisions

- **Q7 (answered by the user, as recommended):** order (a):
  1. Wilson coefficient API refactor (`docs/architecture/todo/wilson-coefficient-api.md`). It changes names that the
     ports would otherwise use.
  2. The remaining ports, in one batch per group.
  3. Model serialisation, as its own phase (`docs/architecture/todo/model-serialization.md`). It unlocks MSSM,
     PMSSM and NMFV, which are large. It is not started in this F6 spec. It needs its own spec and gates.

## 2. Work groups

### G-A. Wilson coefficient API refactor (first)

- Source: `todo/wilson-coefficient-api.md`; decompose on operator basis, `getOperatorCoefficient`.
- Open points carried from phase 5: C7 and C8 evaluate to `0` in Python (cause not established,
  `phase-5-calculation.md` "Open points"). The refactor must either fix the cause or record why it is out of scope.
- Acceptance: the existing C7, C8 and C9 ports still pass with the same reference values
  (`tests/system/data/output/`). Changing reference data is not allowed unless the physics changes (AGENTS.md).

### G-B. Remaining ports (second)

Ports not yet in `tests/python/system/` (inventory E1, row 1):
- `BsMixingSM`, `BsMixingVLQ_5_6`, `BsMixingVLQ_7_8`, `BsMixingVLQ_9_10`, `BsMixingVLQ_10_11` (need G-A).
- `wilsons_GTHDM` (needs G-A).
- `gm2SM` (needs an `isInLoop` predicate filter, `phase-5-calculation.md` "Blocked").
- `FItests` (needs vector integrals and the Feynman integral reduction API, `phase-5-calculation.md` "Blocked").
- `Bgenesis` and `freeQuarkAmplitudes` (no blocker recorded; check each against the C++ test before starting).

Each port follows the porting rules (`python-porting-rules`), and gets a test in the phase 7 format
(`python/tests/system/`, now `tests/python/system/`) with a reference comparison.

### G-C. Other open items

- `keepOnlyFirstMassInLoop` as a global option: challenge its use and exposure (inventory E1 row 3).
- `add_amplitude` asserts non-empty, while C++ skips empty amplitudes (phase 5, unverified).
- Quad precision in the C++ CTest path (phase 7 summary section 5, partial).
- C9_MSSM, C7_PMSSM, C8_PMSSM, gm2PMSSM and `test_fullMSSM` wait for G-serialisation (later phase).

## 3. Steps

1. For each group, write a per-item spec in the same format as F1 (questions first if the behaviour is ambiguous).
2. Implement G-A, then G-B one port at a time, each with its test and reference comparison.
3. Re-run the slow system tests that the change touches, by name (`ctest -R <name>`); never the `system` suite.
4. Update the developer gaps page (F4) after each port.

## 4. Verification

- Each port passes its reference comparison; the port table in the phase 8 exit artefacts is updated.
- `mypy --strict` stays at the F3 baseline (zero errors).
- Fast Python suite (`-m "not slow"`) and C++ `units`, `integration`, `api` labels stay green.

## 5. Acceptance

- Per-item acceptance from the phase documents (table in F6 README row).
- No public name changed after F3 without an amendment and a deprecation entry (F5 policy).

## 6. Out of scope

- Model serialisation (its own phase, with its own spec and gates).
- Inline diagrams, GIL release and cooperative cancellation (deferred by Q4).
- ARM and extra platforms.

## Open points

- Whether `Bgenesis` and `freeQuarkAmplitudes` are blocked by anything not recorded (check before G-B).
- The C7 and C8 zero values may need a separate investigation if the G-A refactor does not explain them.

## Amendments

(none)
