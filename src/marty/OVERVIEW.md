# MARTY (physics layer)

`src/marty/` is the physics part of MARTY, built on top of CSL. Its main features:

- Model building: gauge groups, particles and their representations, symmetry breaking, field
  rotations, masses (`mty::Model`, `ModelBuilder`, `ModelData`).
- Lagrangian, interaction terms and Feynman rules.
- Diagram generation with Wick contractions, and amplitude / squared-amplitude computation at
  tree and one-loop level.
- Decay widths and Wilson coefficients.
- Dirac algebra through SGL, colour algebra, loop integrals through LoopTools.
- Built-in models (SM, QED, QCD, THDM, GTHDM, NMFV, MSSM, pMSSM).
- UFO vertex export, LHA parameter input, and a gamma-matrix API.
- Generated standalone libraries (`mty::Library`) and Feynman-diagram display through GRAFED.

Sub-folders: `core/` (physics core), `models/` (built-in models), `sgl/` (Dirac algebra),
`ufo/` (UFO export), `lha/` (Les Houches input), `api/` (gamma API).

The pMSSM is split into `models/pmssm_lem/` translation units (`content`, `kinetic`, `mass`,
`interaction`, plus `pmssm_data.h`); edit only the part you need. LHA input defaults for built-in
models live in `core/lhaBuiltIn.h`.

The `README.md` in this folder is the Doxygen main page for MARTY, so it is kept as is.

Build targets: `marty` (shared) and `marty-static`, combining the object libraries of the
sub-folders, plus `marty-lha` and its static variant.

Start with [../../docs/architecture/functionalities.md](../../docs/architecture/functionalities.md)
for the full pipeline and the user manual `docs/doc/marty-manual.pdf`.
