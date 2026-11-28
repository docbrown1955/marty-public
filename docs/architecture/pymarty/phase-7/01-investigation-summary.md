Status: READY
Updated: 2026-11-27
Author: orchestrator (consolidated from four read-only investigator reports; investigators cannot write files, so the orchestrator recorded their findings here and re-checked the lines below)

# Phase 7 investigation summary

## 0. Baseline (in progress)

- Configure: `/tmp/pymarty-build`, `MARTY_BUILD_PYTHON=ON`, venv `/tmp/pymarty-venv` (pytest, numpy, pybind11, ruff, mypy).
- Compiler: the process doc asks for GCC 12, which is not installed here. The build uses the default GCC 11.4
  (`gcc-11`, `g++-11`, `gfortran-11`), which meets AGENTS.md ("GCC 11 or newer"). Deviation recorded here.
- Full build started in background (`build.log`). Baseline test results are pending; see `03-spec.md` "Baseline".

## 1. Verified facts (orchestrator re-checks)

| Claim | Evidence (re-checked) |
|-------|------------------------|
| `sampleScalar` uses `Order::OneLoop` for `phi -> phi phi` | `examples/sampleScalar.cpp:35-38` |
| `Order { TreeLevel, OneLoop }` and both `computeAmplitude` overloads | `src/marty/api/modeling/modeling.h:148`, `modeling.cpp:953-970` (investigator) |
| `Session` snapshots MARTY/CSL flags and the current model | `src/marty/api/common/session.h:31-66` (investigator) |
| LHA reader stores real values only: `LHAElement` holds a `long double` (`FloatType`) | `src/marty/lha/lhaData.h:33-40`; `grep -n complex src/marty/lha/lhaData.h` returns nothing |
| Quad tests are gated by `QUADRUPLE_ENABLED`, not `MARTY_QUADRUPLE_ENABLED` | `tests/system/CMakeLists.txt:18-25`; `CMakeLists.txt:92-105` |
| Quad test variant of EWGauge is already in Python | `python/tests/system/test_ewgauge.py:159-176` (investigator) |
| Python system ports present: `test_c7_sm`, `test_c8_sm`, `test_widths_sm`, `test_model_width`, `test_ewgauge`, `test_sm_2idm`, `test_sm_triplet`, `test_zmumu`, `test_gg_to_tt`, `test_simple_spectrum` | `ls python/tests/system` |
| Python `calculation` exposes `WilsonSet`, `wilson_coefficients`, `chromo_magnetic_operator`, `chromo_magnetic_coefficient` | `python/pymarty/calculation/__init__.py:335-376` (investigator) |
| `src/marty/api/simulation/lha.h` does not exist; the reader is `src/marty/lha/lha.h` | `ls src/marty/api/simulation/` (investigator) |
| No quad-related Python API: `config.py` has no `quad_available` | `python/pymarty/config.py` (investigator) |
| `test_fullMSSM.cpp` has no reference comparison and is not in the system CMake list | `tests/system/src/test_fullMSSM.cpp:80-101`, `tests/system/CMakeLists.txt:32-57` |
| `freeQuarkAmplitudes` has no CTest target (commented out as unstable) | `tests/system/CMakeLists.txt:36-37` |
| Boltzmann-solver plugin exists in `src/marty-plugins/boltzmann-solver/` (three data classes, JSON export) | directory listing; `OVERVIEW.md` (investigator) |

## 2. Plan document (`phase-7-loops-wilson-widths.md`) versus reality

The plan is partly stale. Items that phase 6 already did or moved:

- `c7_sm`, `c8_sm`, `widths_sm`, `model_width` are Python ports already (phase 6, H1, H2, F). The plan still lists them as phase 7 work.
- `majoranaLoop`, `testConjugation`, `NLepto` moved to the C++ integration suite in phase 6 (H3, H5, H6). The plan still lists them. Remove them from phase 7.
- `gm2SM` and `gm2PMSSM` are **one-loop** (`Order::OneLoop`), not tree level (`tests/system/src/gm2SM.cpp:51`, `gm2PMSSM.cpp:35`).
- `sampleScalar` and `sample2HDM` are examples, not system tests. They have no reference file. Their Python port is a smoke test with no numbers, unless the user asks for a reference-based check.
- `FItests.cpp` is a test of Passarino-Veltman reduction on hand-built integrals. It does not use a model or `Order`. It has no output comparison.
- `test_fullMSSM.cpp` has no reference and no CTest target. Its plan entry ("full") is not a comparison.
- `EWGauge` quad variant is already in Python (`test_ewgauge.py:159-176`). Only the C++ quad CTest path remains to check.

## 3. One loop (7a)

- `OneLoop` is reachable through `Model::computeAmplitude` (`modeling.cpp:953-989`). The per-call `AmplitudeOptions`
  overload translates topology, particle filters and fermion order into `mty::FeynOptions`.
- Global flags are used in examples (`computeFirstIntegral`, `keepOnlyFirstMassInLoop`, `sampleScalar.cpp`,
  `sample2HDM.cpp:26-38`). The Python `calculation` module already accepts them as scoped global options
  (`python/pymarty/calculation/__init__.py:300-350`).
- LoopTools: `src/marty/core/looptools_init.h:26-52` initialises `ltini()` once and `ltexi()` at teardown.
  How generated code emits LoopTools calls, and the `-looptools` flag, is **not verified**.
- Existing one-loop Python coverage: C7/C8 SM, and one-loop API tests (`python/tests/calculation/test_calculation_api.py:175-199`).

## 4. Wilson coefficients and widths (7b, 7c)

C++ system tests that remain unported in Python (name-based, verified by the orchestrator only at the directory level):

| C++ test | Level | Reference | Notes |
|----------|-------|-----------|-------|
| `C7_PMSSM.cpp` | one loop | `data/output/C7_marty_tanb50_ss.txt` | `PMSSM_LEM`, chromomagnetic R/L |
| `C8_PMSSM.cpp` | one loop | `data/output/C8_marty_tanb50_ss.txt` | idem |
| `C9_MSSM.cpp` | one loop | `data/output/C9_marty_tanb50_ss.txt` | `PMSSM_LEM`, `b -> s mu mu` |
| `C9_SM.cpp` | one loop | `data/output/C9_SM.txt` | `SM_Model` |
| `BsMixingSM.cpp` | one loop | `data/output/BsMixingSM.txt` | box topology |
| `BsMixingVLQ_5_6`, `_7_8`, `_9_10`, `_10_11` | one loop | `bsmixingvlq_*.txt` | `VectorLike_Model`, shared helper `vectorlike.h` |
| `wilsons_GTHDM.cpp` | one loop | `data/output/gthdm.txt` | model built in test |
| `gm2SM.cpp`, `gm2PMSSM.cpp` | one loop | `gm2_sm.txt`, `gm2_pmssm.txt` | triangle topology |

Public Wilson API in Python (phase 6): `WilsonSet`, `wilson_coefficients`, `chromo_magnetic_operator`,
`chromo_magnetic_coefficient`. Core functions for four-fermion boxes and penguins exist in
`src/marty/core/model.h:265` and `src/marty/core/wilsonOperator.h:33`, but are **not exposed** in the
`calculation` facade (`src/marty/api/calculation/calculation.h:64`). The plan's "named option presets" for
topology filters are a new design item. No C++ API test for Wilson coefficients was found in `tests/api/src`.

Widths: `widthsSM` and `testModelWidth` are done in phase 6. Remaining width work is the `add_width` helper,
which is already a Python `addWidth`-style call in the `widthsSM` port. Not verified whether it is complete.

## 5. LHA and quad precision (7d, 7e)

- LHA: the C++ reader is `mty::lha::Reader::readFile` (`src/marty/lha/lha.h:39-72`). It returns `LHAFileData`
  with real `long double` values (`lhaData.h:33-40, 340-389`). **No complex-valued LHA element exists.** The
  plan's `read_lha(path) -> Mapping[str, Mapping[tuple, complex]]` is not supported by the C++ as it stands.
  Decision needed (Q1).
- LHA test: `test_lha_reader.cpp:22-40` reads `tests/system/data/input/test.lha`, checks STANDARD, SINGLE and
  MATRIX blocks at `1e-10`. This is a fast, reference-free test that can be ported.
- Parameters into generated libraries: no LHA-to-library or Python parameter API exists. `param_t` in the
  generator (`src/csl/librarygenerator.cpp:307`) is internal. Phase 6 made public parameter structs double only.
- Quad: `CMakeLists.txt:92-105` sets quad on for GNU compilers. Generated C uses `__float128`
  (`src/csl/librarygenerator.cpp:683-718`). Quad is internal to the C library (phase 6 section 9). The
  `ADD_QUAD_SYSTEM_TEST` macro registers nine tests when quad is on: `EWGauge`, `gm2SM`, `gm2PMSSM`, `C7_PMSSM`,
  `C8_PMSSM`, `C9_SM`, `C9_MSSM`, `wilsons_GTHDM`, `Bgenesis` (`tests/system/CMakeLists.txt`). None has a `slow`
  marker in CMake. Python quad markers: only `test_ewgauge.py` today.
- `Config.quad_available` (plan 7e) does not exist. Must be added, or the check done via the build's
  `Config`. Decision needed (Q3).

## 6. Remaining tests and plugins (7f)

- `Bgenesis.cpp` (`:322`) is a quad-only system test with no Python port. It checks only that the binary builds.
- `SM_2IDM`, `SM_triplet`: Python ports exist (build-and-load only), so they are done.
- `freeQuarkAmplitudes.cpp`: tree-level `gg -> t tbar` squared amplitude, 18 then 162 operators
  (`:43-57`). No CTest (commented out, `tests/system/CMakeLists.txt:36-37`). Not ported.
- `vectorlike.h`: shared helper for the four VLQ tests. `ckm.cpp`: copied into `C9_MSSM` at `C9_MSSM.cpp:179-181`.
  Both should move to `pymarty.testing` or the C++ API, not be copied (plan section 6).
- Boltzmann-solver plugin: `src/marty-plugins/boltzmann-solver/` (object library, shared and static targets,
  links `marty`/`marty-static`, `CMakeLists.txt:1-14`). Its `OVERVIEW.md` says it uses GRAFED's `jsonLoader.h`,
  which conflicts with the layering rule. Its C++ facade must live in `src/marty-plugins/.../api/`
  (plan section 7f). No Python plugin module exists (`python/pymarty/plugins`).
- `testutility.h`'s `assert_binary` is the only check in four active model tests (`testutility.h:96-106`).
  Those tests do not compare numbers.
