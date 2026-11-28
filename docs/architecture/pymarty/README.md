# pymarty: Python wrapper for MARTY, master plan

Status: plan only. No implementation has started.

`pymarty` is a Python package on top of the MARTY C++ libraries. This folder is the plan. Read
this file first, then [api-conventions.md](api-conventions.md) and
[testing-strategy.md](testing-strategy.md), then the phase document you are working on. Phases 1 to 8
are run with the process of [process/README.md](process/README.md); each phase keeps its working
documents in `phase-N/` next to its plan document.

| Document | Content |
|----------|---------|
| [api-conventions.md](api-conventions.md) | Rules for the C++ API layer and for the Python layer (duck typing, errors, state, ownership) |
| [python-porting-rules.md](python-porting-rules.md) | Ground rules for porting a C++/CSL feature to Python: thin layers, files to touch, tests first, Expr methods vs free functions, callback algorithms, accessor error model |
| [unported-features.md](unported-features.md) | Living list of C++ features not yet exposed in Python, with status and phase |
| [phase-1-symbolic-addendum.md](phase-1-symbolic-addendum.md) | Addendum to phase 1: relabel_indices, replace and index replacement, accessors, parent accessors, callback algorithms, eval mode |
| [testing-strategy.md](testing-strategy.md) | Test levels, parity rule, shared data, markers, quality gates, global "definition of done" |
| [process/README.md](process/README.md) | How phases 1 to 8 are run: investigation, Q&A, spec, implementation, AI review, human review; roles and gates; the orchestrator, investigator, implementer and reviewer instructions |
| [phase-0-foundations.md](phase-0-foundations.md) | Build, packaging, error/state hardening, spikes, test harness |
| [phase-1-symbolic.md](phase-1-symbolic.md) | `pymarty.symbolic` (CSL expressions, indices, tensors) |
| [phase-2-diracology.md](phase-2-diracology.md) | `pymarty.diracology` (SGL / gamma matrices) |
| [phase-3-group-theory.md](phase-3-group-theory.md) | `pymarty.group_theory` |
| [phase-4-modeling.md](phase-4-modeling.md) | `pymarty.modeling` (model building, built-in models, Feynman rules) |
| [phase-5-calculation.md](phase-5-calculation.md) | `pymarty.calculation` (tree level) and `pymarty.visualization` (export and GRAFED launcher) |
| [phase-6-simulation.md](phase-6-simulation.md) | `pymarty.simulation` (generate, build, load and call generated libraries) |
| [phase-7-loops-wilson-widths.md](phase-7-loops-wilson-widths.md) | One loop, Wilson coefficients, widths, spectra, LHA, MSSM, quad precision, plugins |
| [phase-8-polish-release.md](phase-8-polish-release.md) | Stubs, docs, notebooks, inline diagrams, wheels matrix, release |

## 1. Goals

1. A Python package named `pymarty`, installed from wheels.
2. Concentrate on the main MARTY API first: model building, amplitudes, generated libraries.
3. All logic lives in C++ APIs. The Python layer only handles Python syntax (operators, iteration,
   keyword arguments, duck typing, exceptions, `repr`).
4. Development is test driven: C++ API tests are the specification, and Python tests check that the
   binding is faithful and viable.
5. Duck typing everywhere in the Python layer.
6. Long term: every C++ example in `examples/` and every test in `tests/` can be reproduced from
   Python.

## 2. Modules

| Python module | Purpose | May depend on |
|---------------|---------|---------------|
| `pymarty.symbolic` | CSL `Expr`, constants, functions, indices, spaces, tensors | nothing |
| `pymarty.group_theory` | Pure group theory (groups, irreps, products, Casimir, Dynkin index, generators) | nothing |
| `pymarty.diracology` | Pure SGL / gamma-matrix calculations | nothing at import time; symbolic coefficients are accepted by duck typing, so `symbolic` is optional |
| `pymarty.modeling` | Model building (gauge groups, particles, Lagrangian, symmetry breaking), built-in models, Feynman rules | symbolic, group_theory |
| `pymarty.calculation` | Amplitudes, squared amplitudes, options and filters, Wilson coefficients and widths (phase 7) | modeling, symbolic |
| `pymarty.simulation` | Generate, build, load and evaluate C/C++ libraries produced from expressions; kinematics helpers | symbolic, calculation (optional) |
| `pymarty.visualization` | Export diagrams to the GRAFED JSON format; launch GRAFED; `pymarty.visualization.grafed` is the GUI backend | calculation |
| `pymarty.testing` | Helpers used by the test suites (reference-data comparison) | none |
| `pymarty.config` | Run-time discovery of include/lib paths, compiler, LoopTools, GRAFED | none |

`group_theory` and `diracology` must be importable and usable without importing `modeling`,
`calculation` or `simulation` (enforced by a test, see [testing-strategy.md](testing-strategy.md)).

## 3. Decisions taken

| # | Decision | Reason |
|---|----------|--------|
| D1 | One compiled extension `pymarty._pymarty`, Python submodules re-export it | CSL and MARTY have process-wide global state (allocators, Minkowski space, `dirac4`, `Model::current`, option flags). Several extensions each linking their own copy would not share types. The wheel layout can move to "one shared library, several extensions" later. |
| D2 | pybind11 (>= 2.11, fetched by CMake `FetchContent`, version pinned) | Already used for the earlier gamma binding (git branch `api/gamma`), good `shared_ptr` support, mature. The thin C++ API layer keeps a later move to nanobind cheap. |
| D3 | Phase 0 may change the core: `CallHEPError`, `exit`/`abort` calls, `Model::current`, option flags | The API layer cannot be reliable otherwise. Changes must not alter existing C++ behaviour. |
| D4 | C++ facades live in `src/csl/api/` and `src/marty/api/<module>/` | Keeps the layering rule: CSL does not know MARTY. `src/marty/api/gamma.*` already follows this pattern. |
| D5 | Distribution by wheels (Linux, manylinux; macOS support was removed from the repository) | Requirement. Constrains Phase 0 (relocatable paths, bundled shared libraries, license compliance). |
| D6 | numpy is not a dependency | Not needed. Plain sequences, `fractions.Fraction` and `complex` are used. An optional `pymarty[numpy]` extra may accept buffer-protocol objects later. |
| D7 | Diagram display is a separate module, `pymarty.visualization`, with a GRAFED backend `pymarty.visualization.grafed` | Export must work without Qt. The GUI is optional and not bundled in the wheel. |
| D8 | Existing C++ behaviour, test reference data and public headers stay unchanged unless a phase says otherwise | `AGENTS.md`: do not edit reference data to make a test pass. |
| D9 | Each module has one human-readable usage example, reviewed and approved before its implementation starts; errors are imported from `pymarty.errors` | [testing-strategy.md](testing-strategy.md) section 1.2; `python/examples/usage/`. |

## 4. Layered architecture

```
Python    pymarty.{symbolic, group_theory, diracology, modeling, calculation,
                   simulation, visualization, testing, config}           syntax only
            |   pybind11 bindings (python/bindings/<module>.cpp): coercion, operators,
            |   iterators, exception translation, stream redirection
C++ API   src/csl/api/                      symbolic
          src/marty/api/common/             errors, Session guard, Config (Rational: Phase 1)
          src/marty/api/{group_theory, diracology (gamma.h), modeling,
                         calculation, simulation, visualization}/       ALL the logic
            |
Core      CSL (src/csl), SGL and MARTY (src/marty), GRAFED core + JSON parser
```

Dependencies only go downwards, as in [../functionalities.md](../functionalities.md).

## 5. Proposed repository layout

```
python/
  pyproject.toml            scikit-build-core, cibuildwheel configuration
  CMakeLists.txt            target _pymarty (option MARTY_BUILD_PYTHON, default OFF)
  bindings/                 one .cpp per Python module, plus module.cpp
  src/pymarty/              pure Python package, type stubs (.pyi), py.typed
  tests/<module>/           pytest tests
  tests/system/             ports of tests/system (slow)
  examples/                 ports of examples/*.cpp
  examples/usage/           one human-reviewed usage example per module (errors, symbolic, ...), run by the tests
  tools/check_parity.py     C++/Python test parity checker (Phase 0 or 1, see testing-strategy.md)
src/csl/api/                C++ API for symbolic
src/marty/api/<module>/     C++ API per module
tests/api/<module>/         C++ API tests (gtest, ctest label "api"; flat in tests/api/src/ during Phase 0)
tests/api/data/             golden data shared by C++ and Python tests
docs/architecture/pymarty/  this plan
```

## 6. Phase overview

Each phase ends only when its acceptance criteria are met. A later phase must not start its
implementation before the previous phase is accepted, except for explicitly listed spikes.

| Phase | Deliverable | Depends on | Reproduces (long-term target) |
|-------|-------------|------------|-------------------------------|
| 0 | Build/packaging skeleton, typed errors, state guard, test harness, spikes | none | none |
| 1 | `symbolic` | 0 | `tests/units/csl/*` |
| 2 | `diracology` | 0 (1 optional) | `tests/integration/src/gammaAPI.cpp` |
| 3 | `group_theory` | 0 | group parts of `test_flavor_groups.cpp` |
| 4 | `modeling` | 1, 3 | `simpleSpectrum`, `sampleSMFeynmanRulesLatex`, `test_feynman_rules_latex`, `test_physics_latex`, `test_specific_gauge_couplings`, first half of `gettingStarted` (models QED and SM only) |
| 5 | `calculation` (tree level) + `visualization` | 4 | `sampleQED`, `sampleSQED`, `sampleQCDSextet`, `test_flavor_groups`, symbolic parts of `Zmumu`, `gg_to_tt`, `EWGauge`, `testConjugation` |
| 6 | `simulation` | 5 | full `gettingStarted`, `Zmumu`, `gg_to_tt`, `EWGauge`, `simpleSpectrum`, `testModelWidth` |
| 7 | One loop, Wilson, widths, spectra, LHA, MSSM, quad, plugins | 6 | `sampleScalar`, `sample2HDM`, `C7/C8/C9`, `BsMixing*`, `widthsSM`, `gm2*`, `wilsons_GTHDM`, `FItests`, `test_lha_reader`, `NLepto`, `Bgenesis`, `test_fullMSSM`, `majoranaLoop`, `SM_*` |
| 8 | Stubs, docs, notebooks, inline diagrams, wheel matrix, release | 7 (partly earlier) | release |

Phase 2 and 3 are independent of 1 and each other and can be developed in parallel once phase 0 is
accepted.

## 7. Cross-cutting risks

| Risk | Mitigation | Where |
|------|------------|-------|
| Process-wide state (`Model::current`, ~40 `mty::option::*` flags, `csl::option::*`, symbolic 4D flag of the gamma API) | `Session` guard and explicit per-call option objects; every API function is state-neutral on exit | Phase 0 |
| Core errors are a bare enum thrown after printing to stderr; 12 `exit`/`abort` sites outside `src/marty/ufo` (count to re-verify) | Typed exception, quiet mode, replace terminating calls on API paths | Phase 0 |
| `Expr`, `Particle` are `shared_ptr` with shallow copies and in-place mutating functions | Python-visible objects are immutable in practice: facade deep-copies before any in-place core call | Phase 1 |
| Model and Particle lifetime cycles | Particle handles hold their model alive; tested with `gc` / `weakref` | Phase 4 |
| Slow and heavy computations (one loop, Debug builds) | Test markers, Release CI, `system`/`slow` tests run manually by the maintainer only | Test strategy |
| Generated libraries need compiler and LoopTools at run time, and wheels bake in no usable compiler path | Run-time `Config`, clear `BuildError`, documented requirements | Phases 0 and 6 |
| Executing generated code and shelling out with user-controlled names | Name validation, no shell strings, `dlopen` only of files MARTY just built or the user explicitly passed | Phase 6 |
| `Show()` uses a predictable `/tmp/graphs.json` and `system()` | Replaced by a JSON-string API and a safe launcher | Phase 5 |
| C++/Python semantic drift | Parity checker, shared golden data | Test strategy |
| GPL v3 license obligations when bundling shared libraries (GSL, libgfortran) in wheels | License and source-offer handling in the release phase | Phase 8 |

## 8. Open items (to settle before or during the named phase)

- Distribution (manylinux image, wheel contents, minimum Python version, version policy, PyPI name,
  licensing): deferred, tracked in [distribution-deferred.md](distribution-deferred.md).
- Exact pinned pybind11 version, Phase 0 (must build with 2.11 and 3.x).
- Whether the name `pymarty` is available on PyPI (do not publish before checking), see the deferred note.
- Which of the remote branches (`separating_global`, `making_lualatex_optional`) already contain
  useful work for global state and for making LaTeX tools optional at configure time, Phase 0.
- Headless diagram rendering for notebooks (GRAFED offscreen, or a pure Python SVG renderer from the
  JSON), Phase 8.
