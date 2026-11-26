# Phase 7: loops, Wilson coefficients, widths, spectra, LHA, MSSM, quad precision, plugins

Extends calculation and simulation to the full set of examples and system tests. It is split into
sub-milestones that can be accepted one by one. Each sub-milestone has its own acceptance criteria and
reuses the existing C++ reference data.

Prerequisites: Phase 6 accepted.

## 1. Sub-milestones

| ID | Topic | Reproduces | Needs |
|----|-------|-----------|-------|
| 7a | One-loop amplitudes and LoopTools functions in generated libraries | `sampleScalar`, `sample2HDM`, `majoranaLoop`, `FItests` | LoopTools |
| 7b | Wilson coefficients (`WilsonSet`, operators, penguins, boxes) | `C7_SM`, `C8_SM`, `C9_SM`, `C7_PMSSM`, `C8_PMSSM`, `C9_MSSM`, `BsMixingSM`, `BsMixingVLQ_*`, `wilsons_GTHDM` | 7a |
| 7c | Decay widths and model widths, spectrum generation | `widthsSM`, `testModelWidth` (full), `gm2SM`, `gm2PMSSM` | 7a/7b as needed |
| 7d | LHA input and parameter import into libraries | `test_lha_reader`, `test_fullMSSM`, MSSM/pMSSM libraries | 7c |
| 7e | Quadruple precision | `ADD_QUAD_SYSTEM_TEST` cases (for example `EWGauge` quad variant) | quad enabled build |
| 7f | Remaining system tests and plugins | `Bgenesis`, `NLepto`, `SM_2IDM`, `SM_triplet`, `freeQuarkAmplitudes`, `testConjugation`, `vectorlike.h` users, Boltzmann-solver plugin | all above |

Before implementing a sub-milestone, list the exact C++ file, the reference file, the tolerance and the
recorded C++ wall time (from `build/Testing/Temporary/LastTest.log`) in the PR.

## 2. Work packages

### 7a One loop
- API: `Order::OneLoop` support in `compute_amplitude` with its option flags (dimensional regularisation,
  tadpoles, ...) all via per-call options and the `Session` guard.
- Loop integrals: how LoopTools functions appear in generated code, initialisation
  (`marty/core/looptools_init.h` included by `mty::Library`), library flag `-looptools`, `Config`
  discovery of `libooptools` in the wheel.
- Evaluate generated one-loop libraries from Python (extends `LoadedLibrary`, loop initialisation inside
  the ABI: `marty_init()`).
- `FItests`: port of the Feynman-integral data handler and kinematics helpers; reference data read with
  `pymarty.testing`.

### 7b Wilson coefficients
- API `src/marty/api/calculation/wilson.h`: `WilsonSet`, `Wilson`, operator basis selectors
  (`dimension6Operator` and the `computeWilsonCoefficients` variants for two-fermion-one-vector, boxes
  and penguins), `get_wilson_coefficient(set, operator)`, expressions as symbolic handles, plain printing.
- Topology filters, fermion order and particle filters used by the C* examples become named
  option presets in Python tested against the C++ ones.

### 7c Widths and spectra
- `compute_width`, `compute_model_widths(order)`, `Library.add_spectrum(model)` extended,
  `Library.add_width(...)` helper (the `addWidth` helper of `widthsSM`).

### 7d LHA
- API `src/marty/api/simulation/lha.h`: `read_lha(path)` returning blocks (`Mapping[str, Mapping[tuple,
  complex]]`), `Library.import_lha()` equivalent, `mty::lha::Reader::readFile` behind it.
- File access: path validated, file only read.

### 7e Quad precision
- Only when the build was configured with quadruple precision (`MARTY_QUADRUPLE_ENABLED=1`).
  `quad` marker; `Config.quad_available`; clear `NotImplementedError` otherwise.

### 7f Plugins
- `pymarty.plugins.boltzmann_solver` if wanted: wrapping `src/marty-plugins/boltzmann-solver`
  (processes with initial/final particles and quantum numbers, JSON export). Separate C++ API under
  `src/marty/api/plugins/` (the plugin depends on MARTY, MARTY never depends on it, so the facade
  lives with the plugin build, in `src/marty-plugins/.../api/`). Scope confirmed with the maintainers before
  starting.

## 3. Test plan

For every sub-milestone:
- L1: C++ API tests in `tests/api/calculation/` and `tests/api/simulation/` with the same data as the
  corresponding C++ system test (the API path and the legacy direct path must yield the same expression strings or
  the same numbers).
- L2: parity tests.
- L5: Python port in `python/tests/system/`, comparing with `tests/system/data/output/<file>` using the tolerance from
  the C++ test. Markers `slow`, `loops`, `needs_compiler`, `quad`.

Table of ports (to be completed when each sub-milestone starts; the C++ test and its reference file
are the ground truth):

| Python port | C++ source | Reference | Marker | Notes |
|-------------|------------|-----------|--------|-------|
| `test_sample_scalar.py` | `examples/sampleScalar.cpp` | symbolic, compared with C++ printout | `loops` | one loop scalar |
| `test_sample_2hdm.py` | `examples/sample2HDM.cpp` | idem | `loops` | uses `FeynOptions` filters |
| `test_c7_sm.py`, `test_c8_sm.py`, `test_c9_sm.py` | `C7_SM.cpp`, `C8_SM.cpp`, `C9_SM.cpp` | `data/output/*` | `slow`, `loops`, `needs_compiler` | very slow |
| `test_c7_pmssm.py`, `test_c8_pmssm.py`, `test_c9_mssm.py` | `C7_PMSSM.cpp`, `C8_PMSSM.cpp`, `C9_MSSM.cpp` | idem | `slow`, `loops`, `needs_compiler` | loads PMSSM |
| `test_bs_mixing_*.py` | `BsMixingSM.cpp`, `BsMixingVLQ_5_6.cpp` ... | idem | `slow`, `loops`, `needs_compiler` | shares `vectorlike.h` logic |
| `test_widths_sm.py` | `widthsSM.cpp` | idem | `slow`, `needs_compiler` | |
| `test_gm2_*.py` | `gm2SM.cpp`, `gm2PMSSM.cpp` | idem | `slow`, `loops` | |
| `test_lha_reader.py` | `test_lha_reader.cpp` | idem | none | fast |
| `test_full_mssm.py` | `test_fullMSSM.cpp` | idem | `slow` | |
| `test_fi_tests.py` | `FItests.cpp` | `data/` | `loops` | |
| `test_majorana_loop.py` | `majoranaLoop.cpp` | moved to C++ integration (`tests/integration/src/test_majorana_loop.cpp`), no Python test | | |
| `test_ew_gauge_quad.py` | quad variants | idem | `quad`, `slow` | |
| others | `Bgenesis`, `NLepto` (now C++ integration), `SM_2IDM`, `SM_triplet`, `freeQuarkAmplitudes`, `testConjugation` (now C++ integration), `wilsons_GTHDM` | idem | per test | |

## 4. Acceptance criteria

Per sub-milestone (7a to 7f), all of:
1. Each listed Python port that is not marked `slow` reproduces its reference file within the tolerance used
   by the C++ test, and `tests/system/data/output/` is unchanged (`git diff` empty for this PR). Ports marked
   `slow` are informational: their status goes in the port coverage table and they are run manually by the
   maintainer (testing-strategy.md section 1.1).
2. The API path and the legacy direct path give identical expressions (string equality) or numbers to
   1e-12 relative in the paired C++ API test.
3. All options needed by the C++ test are expressible by Python keyword (listing attached) and the
   `Session` leaves state unchanged after success and failure.
4. Time: for ports that run in the PR, Python wall time is at most 1.5x the C++ wall time plus 5 s for the same scenario, measured
   in Release (the symbolic computation dominates; overhead should be negligible). Larger deviations
   require an explanation in the PR.
5. Missing prerequisites (LoopTools, compiler, quad build) skip locally with a clear message and fail
   in the manual full run where they are required.
6. Memory: informational for `slow` ports (the largest one, the one with the most diagrams, should complete
   within the memory observed for the C++ test plus 25%, measured with `/usr/bin/time -v`).
7. Parity, stubs, doctests, sanitizers (L1 part) and quality gates pass. L0 suites unchanged.

8. Usage examples: the `calculation` and `simulation` examples are extended with the loop-level, Wilson
   coefficient and width snippets of the sub-milestone. The extension is approved by a human before the
   sub-milestone's implementation starts (testing-strategy.md section 1.2), and the examples still run in
   `test_usage_examples.py` (L4b).

Phase 7 is complete when all sub-milestones are accepted, or the remaining ones are listed in an issue
tracker with their status and a clear `NotImplementedError` in the API.

## 5. Exit artifacts

Port coverage table (C++ example/test to Python port, status), timing table, updated stubs,
documentation of every option, notebooks reproducing the main physics results (SM Wilson coefficients
and widths).

## 6. Risks and open questions

- Very long runs and memory usage: `slow` markers, manual runs by the maintainer with generous timeouts, results cached.
- LoopTools initialisation and global state: handled by the generated library ABI and tested
  with two libraries in one process.
- Some system tests have hard-coded paths, helper headers (`vectorlike.h`, `ckm.cpp`) or reference data
  with test-specific formats. Shared helpers move to the C++ API (`pymarty.testing`) rather than being copied.
- Plugin facade placement (layering) needs agreement before 7f.
