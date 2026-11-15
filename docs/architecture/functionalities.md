# MARTY: main functionalities and code map

MARTY is a C++ code generator. A user describes a Beyond-the-Standard-Model (BSM) theory in
C++ (fields, gauge groups, Lagrangian terms, symmetry breaking). MARTY then computes Feynman
rules, diagrams, amplitudes, squared amplitudes, decay widths and Wilson coefficients
symbolically, and writes the results as a self-contained C++ library that is compiled and
evaluated independently of MARTY.

This document describes *what the software does* and *where it lives in the code*. It does
not reproduce the physics formulae; those are in the manual and the publication
(arXiv:2011.02478).

## 1. Layers

```
 GRAFED (GUI, Qt)          Plugins (boltzmann-solver)        Examples / tests / user programs
        │                           │                                    │
        └───────────────┬───────────┴────────────────────────────────────┘
                        ▼
              MARTY physics layer  (src/marty)
   models · Lagrangian · Feynman rules · diagrams · amplitudes · Wilson · UFO · LHA
                        │                 │
                        ▼                 ▼
          SGL: Dirac / gamma-matrix   CSL: computer algebra kernel (src/csl)
          algebra (src/marty/sgl)     expressions, tensors, simplification,
                                      library generation
                        │
                        ▼
          Generated C++ library (outside MARTY, compiled by the user)
```

Dependencies only go downwards: CSL knows nothing about MARTY, MARTY knows nothing about its
plugins, and GRAFED reads files written by MARTY.

## 2. Main functionalities

### 2.1 Symbolic kernel: CSL

CSL (`src/csl/`) is a standalone computer algebra system. Its main concepts:

- **Expressions are shared pointers to a polymorphic base.** `csl::Expr` wraps a
  `std::shared_ptr<csl::Abstract>`. Concrete types are numbers, constants and variables,
  functions (`Exp`, `Cos`, `Log`, ...), sums (`Sum`), products (`Prod`), powers (`Pow`), and
  tensors (`TensorElement`) that point to a shared `TensorParent`. Types are listed in
  `enum.h`.
- **Indices and tensors.** `csl::Index` and `csl::IndexStructure` describe indices;
  `csl::Tensor` and `csl::TensorElement` describe indexed objects. `ISum` and `IProd` are the
  indicial versions that check free indices and contract repeated ones.
- **Simplification and transformation.** `simplification.cpp`, `algorithms/patternMatch.cpp`,
  `commutation.cpp`, `symmetry.cpp`, `replace.cpp`, and the expand / factor helpers in
  `interface.h`. `algo.h` offers generic walks that apply a user function to every
  sub-expression.
- **Linear algebra helpers.** `diagonalization.cpp` (used for mass-matrix diagonalisation and
  field rotations) and `linear_map.h`.
- **Code generation.** `librarygenerator.cpp` with `LibraryGroup`, `LibraryFunction`,
  `LibraryTensor`, `LibraryEvaluator` and `LibraryDependency`. These emit C++ source for a
  numerical library (double precision, or quadruple precision with GNU compilers) and drive
  its Makefile. Templates are embedded as `lib*_data.h` headers.
- **Memory and performance.** `allocator.h`, `alloc_monitor.cpp`, `memory_pool.h`,
  `timeMonitor.cpp`, `progressBar.h`.

The user-facing entry points are `csl.h` (whole library), `interface.h` and
`interface_indicial.h` (free functions), and `options.h`.

### 2.2 Model building

Implemented in `src/marty/core/` (`modelData.h`, `modelBuilder.h`, `model.h`).

`mty::Model` is built in three layers:

1. `mty::ModelData`: storage. Particles, gauged groups, the Lagrangian, coupling constants,
   masses, representations.
2. `mty::ModelBuilder` (inherits `ModelData`): transformations of the model. `addGaugedGroup`,
   `breakGaugeSymmetry`, `replace`, `rotateFields`, `birotateFields`, `addBosonicMass`,
   `addFermionicMass`, `renameParticle`, `refresh`.
3. `mty::Model` (inherits `ModelBuilder`): computations. Feynman rules, amplitudes, squared
   amplitudes, widths, Wilson coefficients.

Particles are `mty::Particle`, a shared pointer to `mty::QuantumFieldParent`. Its
specialisations are `ScalarBoson`, `WeylFermion`, `DiracFermion` (Majorana is a
self-conjugate Dirac fermion), `VectorBoson`, `GaugeBoson`, `GhostBoson` and `GoldstoneBoson`
(`quantumField.h`, `scalarField.h`, `vectorField.h`, `fermionicField.h`, `ghostField.h`,
`goldstoneField.h`). `mty::QuantumField` is the element used in expressions and inherits
`csl::TensorFieldElement`.

Gauge groups: `gaugedGroup.h`, `gauged.h`, `group.h` (group families SU, SO, Sp, U(1)),
`semiSimpleAlgebra.h`, `su_n_algebra.h`, `representation.h`, `generator.h`. `colorSpace.h`
and `traceIdentities.h` compute colour traces; `colorSimplification.cpp` applies them.

Built-in models live in `src/marty/models/`: SM, QED, QCD, a two-Higgs-doublet model (THDM),
a general THDM (GTHDM), NMFV, MSSM and pMSSM (`pmssm_lem/` is the split pMSSM
implementation). Each is a C++ class deriving from `mty::Model`.

### 2.3 Lagrangian and Feynman rules

- `lagrangian.h`: `mty::Lagrangian` holds the kinetic, mass and interaction terms.
- `interactionTerm.h`: `mty::InteractionTerm`, one term that knows which particles it
  contains.
- `feynmanRule.h`: `mty::FeynmanRule`, a vertex built from one interaction term.
- `Model::computeFeynmanRules()` builds all vertices; `Model::getFeynmanRules()` returns them
  (computed on first use). `filters.h` and `feynOptions.h` select rules.
- `propagator.h` and `quantumFieldTheory.h`: propagators and external-leg factors.

### 2.4 Diagram generation

- `expander.h` (`mty::Expander`): expands the Lagrangian to the requested order (`Order` is
  `TreeLevel` or `OneLoop`) and generates the vertex combinations compatible with the
  external legs.
- `graph.h` (namespace `wick`): `Graph`, `Vertex`, `Node`, `ConnectedComponent` and
  `WickCalculator`. This applies Wick contractions while avoiding recomputation of identical
  diagrams.
- `insertion.h`: `mty::Insertion` describes an external leg (incoming or outgoing particle,
  or its antiparticle). `Incoming("u")`, `Outgoing("d")` and `AntiPart(...)` are the
  user-facing helpers.
- `momentumConservation.h`, `feynruleMomentum.h` and `kinematics.h`: momenta and kinematic
  variables.
- `topology.h` and `penguinpatch.h`: topological helpers used by the Wilson coefficient code.

Output: `mty::FeynmanDiagram` (`feynmanDiagram.h`), one diagram with its tree/loop status and
its graph.

### 2.5 Amplitudes and squared amplitudes

- `Model::computeAmplitude(order, insertions[, kinematics][, options])` is the main entry
  point. It selects the rules or the raw Lagrangian (depending on `FeynOptions`), calls
  `Expander::getDiagrams`, then `AmplitudeInitializer::getAmplitude`
  (`amplitudeInitializer.h`).
- `mty::Amplitude` (`amplitude.h`) is the container of diagrams and the resulting
  expression. It also stores the options used.
- `amplitudeSimplification.h`: Dirac chains, Fierz rearrangements, colour, polarisation
  sums, and simplification through SGL.
- `Model::computeSquaredAmplitude(amplitude)` (and overloads for two amplitudes, or for
  Wilson sets) returns a `csl::Expr`. Polarisation sums are in `polarization.h`.
- `Model::computeWidth(...)` computes partial decay widths from a squared amplitude.
- `colorSpace.h`, `groupIndices.h`: colour algebra for amplitudes.
- `dimensionalRegularization.h`, `feynmanIntegral.h`, `looptools_*.h`: loop integrals. They
  are evaluated numerically through the vendored LoopTools (`dep/LoopTools-2.16`), with
  optional quadruple-precision extensions.

### 2.6 Wilson coefficients

`wilson.h`, `wilsonOperator.h`, `operatorBasis.h`, `simpleWilson.cpp` and `penguinpatch.h`.
Models expose `computeWilsonCoefficients(...)` and specialised variants for
two-fermion-one-vector, four-fermion boxes and four-fermion penguins. `WilsonSet` holds the
results. Used for flavour observables (for example B_s mixing and the `C7`, `C8`, `C9`
coefficients in the system tests).

### 2.7 Dirac algebra: SGL

`src/marty/sgl/` implements the Dirac / gamma-matrix algebra used by MARTY. It works on its
own objects and is connected to CSL by conversion functions.

- `indexchain.h` (`IndexChain`): chains of gamma matrices.
- `gammaindex.h`, `metricindex.h`, `epsilonindex.h`, `momentumindex.h`,
  `generalizedindex.h`: the index kinds that can appear in a chain.
- `contraction.h`, `fierz.h`, `tracer.h`, `simplify.h`, `sglcommutation.h`, `ordering.h`:
  contraction, Fierz identities, traces, simplification and ordering.
- `cslconverter.h`: `csl_to_sgl` and `sgl_to_csl`. MARTY converts CSL expressions to SGL
  objects, simplifies there, and converts back (used in `amplitudeSimplification.cpp`).

### 2.8 Interfaces for other tools

- **UFO export** (`src/marty/ufo/`, namespace `mty::experimental::ufo`): turns MARTY Feynman
  rules into simplified vertices (`Vertex`, `Field`, `LorentzCoupling`, `ColorCoupling`,
  `Charge`) for UFO-style Monte Carlo tooling. Entry point: `obtainSimplifiedVertices` in
  `simplifiedvertex.h`.
- **Gamma API** (`src/marty/api/`, namespace `mty::gamma_api`): a small free-function
  interface to Dirac-algebra expressions (`g`, `gamma`, `trace`, `project`, Fierz, and so on).
  Intended for language bindings. The `python/` folder is the planned home of those bindings
  but has no tracked sources yet.
- **LHA input** (`src/marty/lha/`, namespace `mty::lha`): reads Les Houches Accord parameter
  files (`lha.h`, `lhaData.h`, `lhaBlocks.h`), used for SM and SUSY input parameters.
  `core/lhaBuiltIn.h` provides built-in parameter blocks.
- **Plugins** (`src/marty-plugins/`): see section 2.12.

### 2.9 Output: generated libraries

`mty::Library` (`mtylibrary.h`) derives from `csl::LibraryGenerator`.

```cpp
mty::Library lib("toy", path);
lib.addFunction("squared_ampl", squared_ampl, "G");
lib.build();
```

`generateSpectrum(model)` writes masses and couplings from the model. `build(nJobs)` writes
the C++ sources and compiles them into a standalone library, which the user then links. The
generated code uses CSL-generated numerical routines and LoopTools when loops are involved.

### 2.10 Visualisation and display

- `Show(...)` (declared in `mrtInterface.h`, implemented in `drawer.cpp`) writes diagrams to
  `/tmp/graphs.json` and starts the external `grafed-gui` program with `system()`.
- `Display(...)` prints rules, amplitudes and Wilson coefficients to the terminal.
- `mrtlog.h` provides the log streams used by the MARTY algorithms.
- `mrtOptions.h`, `feynOptions.h` and `csl/options.h` hold the global switches.

### 2.11 Interactive diagram editor: GRAFED

`src/grafed/` is a separate Qt 5 application (`grafed-gui`). Its parts:

- `gui/`: `MainWindow`, `DiagramWidget` (a `QGraphicsView`), `Node`, `Edge`, `NodeForm`,
  `EdgeForm`, `Grid`, `Curvature`, `DialogLaTeX`, `PDFSetupDialog`, and the `.ui` forms.
- `core/`: `latexLink` (compiles LaTeX labels with `latex` / `lualatex` and converts them
  with `dvipng`) and `planargraph` (geometry for drawing diagrams).
- `jsonparser/`: reads and writes diagram files (`jsonLoader`, `jsonObject`).
- `gui/ressources/`: icons, logo and particle images.

### 2.12 Plugins

`src/marty-plugins/` holds extensions that depend on MARTY and never the reverse. The current
plugin is `boltzmann-solver`. It defines processes (initial and final particles plus quantum
numbers) and exports them to JSON for an external Boltzmann-equation solver. See its
`OVERVIEW.md`.

## 3. Typical data flow

1. **Build the model.** Create a `Model`, add gauge groups, add particles with
   representations, call `init()`, break symmetries, rotate fields and add masses
   (`ModelBuilder`).
2. **Get the Lagrangian and rules.** `refresh()` updates the model. `getFeynmanRules()`
   produces `FeynmanRule`s from the `Lagrangian` / `InteractionTerm`s.
3. **Generate diagrams.** `computeAmplitude(order, insertions, ...)` runs `Expander` and the
   Wick machinery (`graph.h`) to get `FeynmanDiagram`s.
4. **Build the amplitude.** `AmplitudeInitializer` creates the expression, and
   `amplitudeSimplification` applies Dirac, colour and Fierz simplifications (through SGL).
5. **Square and integrate.** `computeSquaredAmplitude` and `computeWidth`; loop integrals via
   LoopTools.
6. **Export.** `mty::Library::build()` writes and compiles a standalone C++ library.
   Optionally use `Show` / `Display` for visual and textual checks, or the UFO and JSON
   exporters.

## 4. Where to look for common tasks

| Task | Start with |
|------|-----------|
| Add a built-in model | `src/marty/models/` (copy `qed.cpp` or `sm.cpp`) |
| Change how particles are described | `src/marty/core/quantumField.h` / `.cpp`, and the `*Field.h` files |
| Change Lagrangian handling | `lagrangian.h`, `interactionTerm.h`, `modelBuilder.h` |
| Change diagram generation | `expander.h`, `graph.h`, `amplitudeInitializer.cpp` |
| Change Dirac / gamma algebra | `src/marty/sgl/` |
| Change simplification of symbolic expressions | `src/csl/simplification.cpp`, `src/csl/algorithms/` |
| Change how generated libraries look | `src/csl/librarygenerator.cpp`, `src/marty/core/mtylibrary.h` |
| Change the GUI | `src/grafed/gui/` |
| Add a plugin | `src/marty-plugins/` (see its `README.md`) |
