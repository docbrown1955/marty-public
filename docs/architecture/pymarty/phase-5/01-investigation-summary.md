Status: READY
Updated: 2026-11-23
Author: orchestrator (consolidated from six read-only investigator reports; investigators could not write files, so the orchestrator recorded their findings here and re-checked the lines below)

# Phase 5 investigation summary

Baseline (orchestrator, 2026-11-23, `/tmp/pymarty-build`): `ctest -L 'units|integration' -j12` 203/203 passed;
`ctest -L '^(api|python)$' -j12` 163 api and 3 python tests run (see [03-spec.md](03-spec.md) when written).

## 1. Amplitude core (C++)

1. `Model::computeAmplitude` has four overloads taking `int order`, `std::vector<mty::Insertion>`, optional
   `Kinematics const&` and `FeynOptions&`. `src/marty/core/model.h:163-175` (verified).
2. The core returns `mty::Amplitude` (diagrams, expression, options, kinematics). `src/marty/core/amplitude.h:32-40, 67-75, 110-123`.
3. `Incoming`, `Outgoing`, `AntiPart` copy an insertion and toggle direction or particle flag; `Left`/`Right`
   select Weyl components and need a Dirac fermion. `src/marty/core/insertion.cpp:141-190`.
4. Squared amplitude: one-amplitude form delegates to the two-amplitude form; the two-amplitude form computes
   Wilson coefficients and aligns kinematics. `src/marty/core/model.h:221-231`, `src/marty/core/model.cpp:203-237`.
5. The modeling C++ API exposes `Order`, `Leg`, `Amplitude`, `computeAmplitude(Order, vector<Leg>)` and a
   single-amplitude `computeSquaredAmplitude`; calls run inside `Session`
   (`src/marty/api/modeling/modeling.h:141-167, 335-341`, `src/marty/api/modeling/modeling.cpp:771-797`,
   `src/marty/api/common/session.h:37-70`).

## 2. FeynOptions, topology, filters, global flags

1. `FeynOptions` public boolean fields (`orderInsertions`, `simplifyAmplitudes`, `discardLowerOrders`,
   `partialCalculation`, ...) and setters (`setTopology`, `setFermionOrder`, `setLoopOrder`, ...).
   `src/marty/core/feynOptions.h:398-526`.
2. Topology is a bitmask (`Tadpole`, `Mass`, `Triangle`, `Box`, `Pentagon`, `Any`). `src/marty/core/topology.h:33-57`.
3. Filters are `std::function` callbacks: `LagrangianFilter` (`src/marty/core/feynOptions.h:54`) and
   `DiagramFilter` (`src/marty/core/feynOptions.h:62`). Built-ins such as `disableParticles`, `forceParticle`
   capture particle lists (`src/marty/core/filters.h:98-169, 282-368, 386-444`). Built-ins are expressible as
   data (particle names); arbitrary predicates are not.
4. Lagrangian filters act on interaction terms before diagram generation; diagram filters act after Wick
   contraction (`src/marty/core/feynOptions.h:204-258`).
5. Global flags are listed by X-macros `MTY_MARTY_OPTION_FLAGS` and `MTY_CSL_OPTION_FLAGS`
   (`src/marty/core/mrtOptions.h:91-132`, `src/csl/options.h:42-58`), used by the `Session` snapshot
   (`src/marty/api/common/session.h:51-56`).

## 3. FeynmanRules and Wilson

1. The API exposes a read-only `FeynmanRules` view: `size()`, `str(index)`, `latex(index)`, `latexDocument()`;
   private constructor on `Model` (`src/marty/api/modeling/feynmanRules.h:35-56`).
2. Creating the view does not compute; first query calls the core getter inside `Session`; out-of-range
   indexed access throws `LookupError` (`src/marty/api/modeling/feynmanRules.cpp:32-75`).
3. Core stores `std::vector<mty::FeynmanRule>`; `getFeynmanRules()` computes lazily from the Lagrangian,
   removes empty, zero and duplicate rules, sorts (`src/marty/core/model.h:144-161`, `src/marty/core/model.cpp:1074-1123`).
4. Wilson types: `WilsonCoefficient`, `WilsonOperator`, `Wilson`, `WilsonSet` in `src/marty/core/wilson.h:33-131`;
   entry points `getWilsonCoefficients`, `computeWilsonCoefficients` (`src/marty/core/model.h:265-303`).
5. Python parity exemptions say Feynman rules are not exposed to Python
   (`python/tests/parity_exemptions.txt:11-12`).
6. Phase 7 places Wilson coefficients in 7b, depending on 7a (one-loop, LoopTools)
   (`docs/architecture/pymarty/phase-7-loops-wilson-widths.md:14, 36-38`). The Wilson examples named in the
   task (`BsMixingSM`, `C9_SM`, `sample2HDM`) use one-loop Wilson coefficients (`tests/system/src/BsMixingSM.cpp:17-30`,
   `tests/system/src/C9_SM.cpp:112-126`, `examples/sample2HDM.cpp:26-43`). Conflict with the request: see Q1.

## 4. Python layer

1. `Model.finalize()` is bound as returning nothing and is idempotent (`python/src/modeling.cpp:534-540`).
2. The phase 5 plan document still specifies `Model.amplitude(...)` and `Model.squared(...)`
   (`docs/architecture/pymarty/phase-5-calculation.md` section 1, section 3). This conflicts with the request that
   `modeling.Model` must not be extended. Plan to be updated at G1.
3. C++ modeling handles share `ModelState` owning the core `mty::Model`; `Session` sets and restores `Model::current`
   (`src/marty/api/modeling/modeling.h:53-60, 346-350`).
4. `ExprHandle` is already a registered pybind type accepted by the modeling binding, so a new module can consume
   symbolic expressions (`python/src/modeling.cpp:35-38, 108-117`, `python/src/symbolic.cpp:45-56`).
5. No `pymarty.calculation` or `pymarty.simulation` package exists (`python/pymarty/__init__.py:3-14`).
   The top-level docstring lists only symbolic, diracology, config, group_theory, modeling, errors.
6. Phase 6 expects amplitudes and a finalized model as input (`docs/architecture/pymarty/phase-6-simulation.md` sections 1-2).

## 5. Visualization (spike WP5.6)

1. `LatexLinker::saveMultiple` builds JSON nodes and writes them; it does not run LaTeX or any external tool
   (`src/grafed/core/latexLink.cpp:677-688`, declaration `src/grafed/core/latexLink.h:185-188`).
2. JSON root has `diagrams`; each diagram has `name`, `scaleFactor`, nodes and edges with fields the parser reads
   (`src/grafed/core/latexLink.cpp:499-572, 595-645, 677-688`).
3. `Show()` writes `/tmp/graphs.json` and calls `system("grafed-gui /tmp/graphs.json ...")`
   (`src/marty/core/drawer.cpp:162-163`, verified). The phase 5 plan asks to remove both; this is a C++ behaviour
   change (Q3).
4. Top-level CMake always builds `src/grafed` (JSON parser and core); the GUI is conditional on `MARTY_BUILD_GRAFED_GUI`
   (`CMakeLists.txt:111-114`, `src/grafed/CMakeLists.txt:1-5`).
5. No direct test of the JSON output exists; examples call `Show()` (`examples/sampleQED.cpp:20,28,36`).

## 6. Examples and scenarios

1. Tree-level scenarios in scope: QED Compton and e+e- to mu+mu- (tests), `sampleQED`, `sampleSQED`,
   `sampleQCDSextet`, the tree part of `gg_to_tt` (`tests/system/src/gg_to_tt.cpp:47-57, 79-81`). `gg_to_tt`
   continues into library generation after the tree part, which is phase 6.
2. `test_flavor_groups.cpp` computes an amplitude and squared amplitude (`tests/integration/src/test_flavor_groups.cpp:29-47`)
   and also a one-loop case, which is out of scope.
3. `BsMixingSM` and `C9_SM` need one-loop Wilson coefficients and library generation
   (`tests/system/src/BsMixingSM.cpp:17-30`, `tests/system/src/C9_SM.cpp:112-126, 140-154, 217-218`).

## Not verified

- Full filter semantics of each built-in in `filters.h`, beyond the grep-level claims above.
- Whether tree-level Wilson coefficients exist at all (no example found; not verified).
- The exact current Python parity id list for `ModelingApi.*` beyond the two lines above.
