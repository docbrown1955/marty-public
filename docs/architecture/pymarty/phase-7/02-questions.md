Status: ANSWERED
Updated: 2026-11-27
Author: orchestrator

# Phase 7 questions (step 2)

Evidence: [01-investigation-summary.md](./01-investigation-summary.md).

## Q1. LHA value type

- Context: `mty::lha::Reader` stores only real `long double` values; there is no complex LHA element
  (`src/marty/lha/lhaData.h:33-40`). The plan (`phase-7-loops-wilson-widths.md`, 7d) asks for `Mapping[tuple, complex]`.
- Options:
  - (a) Return real values, `Mapping[str, Mapping[tuple, float]]`, matching the C++ reader (no C++ change).
  - (b) Add complex parsing to the C++ reader (behaviour change in the LHA parser, no LHA format defines it).
- Recommendation: (a). It keeps the C++ untouched and matches the data that `test_lha_reader` checks.
- Blocks: 7d API and stubs.

## Q2. Scope of phase 7

- Context: the plan has six sub-milestones. Several items moved or were ported in phase 6 (see investigation section 2).
  The plugin (7f) needs a layering decision and an agreement with the maintainers (plan section 7f).
- Options:
  - (a) All of 7a to 7f in this phase, plugin included only as a facade placed in `src/marty-plugins/.../api/`.
  - (b) All of 7a to 7e, plugin (7f plugin part) deferred to a later phase. The other 7f items (`Bgenesis`, `freeQuarkAmplitudes`) stay in.
  - (c) Only 7a and 7b in this phase, others later.
- Recommendation: (b). It delivers all physics ports and leaves the plugin decision for the maintainers.
- Blocks: the work packages, and whether 7f writes any plugin code.

## Q3. Quad detection and test gating

- Context: the plan's acceptance criterion 5 says missing prerequisites skip with a clear message. Quad is set by
  `QUADRUPLE_ENABLED` (`CMakeLists.txt:92-105`). `Config.quad_available` does not exist (`python/pymarty/config.py`).
- Options:
  - (a) Add `Config.quad_available`, read from the build (a new CMake-generated value or a `_pymarty` function).
  - (b) Do the check in the test with a `pytest` skip based on a probe library build, no new public API.
- Recommendation: (a). It is a small public property, and the plan names it. Option (b) costs a library build per run.
- Blocks: 7e and the `quad` marker.

## Q4. Wilson topology and particle filter options

- Context: the C++ Wilson tests set topology, `forceParticles`, `disableParticles`, fermion order and
  `keepOnlyFirstMassInLoop` per call (`tests/system/src/C9_SM.cpp:116`, `BsMixingSM.cpp:17`). The plan (7b) asks for
  "named option presets" for these.
- Options:
  - (a) Keyword arguments that mirror the C++ options exactly, plus presets only where two ports use the same set.
  - (b) Named presets for every C++ test (for example `preset="c9_sm"`).
- Recommendation: (a). It matches the C++ calls one-to-one, is easier to test against the references, and avoids
  a preset list that is only used by the tests.
- Blocks: the 7b Python API and the test ports.

## Answers

Answered by the user (2026-11-27, via the question form):

| Question | Answer |
|----------|--------|
| Q1 LHA value type | (a) real values, no C++ change |
| Q2 Scope | (b) 7a to 7e, plus `Bgenesis` and `freeQuarkAmplitudes` in 7f; the Boltzmann plugin is deferred |
| Q3 Quad detection | (a) add `Config.quad_available` |
| Q4 Wilson options | (a) keyword arguments mirroring the C++ options, presets only where shared |

Consequences: `read_lha` returns `Mapping[str, Mapping[tuple, float]]`. The plugin is out of this phase, so the
plan's 7f plugin item becomes a later phase. Status set to ANSWERED.
