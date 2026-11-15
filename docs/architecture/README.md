# MARTY architecture documentation

This folder is the developer-oriented guide to the MARTY code base. It complements the
user manual (`docs/doc/marty-manual.pdf`), the CSL manual (`docs/doc/csl-manual.pdf`) and
the generated Doxygen pages (`docs/doc/marty/html`, `docs/doc/csl/html`).

## Contents

- [functionalities.md](functionalities.md): the main features of MARTY, the data flow from a
  model to a generated numerical library, and the source files implementing each step.
- [build-and-tests.md](build-and-tests.md): CMake targets, external dependencies, and the
  unit / integration / system test suites.

## Reading order

1. Root [AGENTS.md](../../AGENTS.md) for a short summary of the project.
2. [functionalities.md](functionalities.md) to understand the pipeline.
3. The `README.md` (or `OVERVIEW.md`) in the folder you are working in. Every subfolder of
   the project has one.
4. The Doxygen pages for the exact class interfaces.

## Repository layout in one view

| Path | Role |
|------|------|
| `src/csl/` | CSL, the computer algebra system (symbolic expressions, tensors, simplification, numerical library generation) |
| `src/marty/` | MARTY proper: QFT models, Feynman rules, diagrams, amplitudes, Wilson coefficients, SGL, UFO export, LHA input |
| `src/marty-plugins/` | Separate libraries that depend on MARTY (currently the Boltzmann solver data exporter) |
| `src/grafed/` | GRAFED, the Qt GUI to draw and display Feynman diagrams |
| `src/doc/` | Doxygen configuration for CSL and MARTY |
| `tests/` | Google Test unit and integration tests, and physics system tests |
| `examples/` | Stand-alone sample programs using the MARTY API |
| `dep/` | Vendored LoopTools (Fortran) used for one-loop integrals |
| `docs/` | Project website, manuals, talks, past releases, and this documentation |
