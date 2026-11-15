# AGENTS.md

Guidance for AI coding agents and new contributors working on MARTY.

## What the project is

MARTY is a C++ code generator for perturbative calculations in Beyond-the-Standard-Model
physics. A model is written in C++ (particles, gauge groups, Lagrangian, symmetry breaking).
MARTY then computes Feynman rules, diagrams, amplitudes, squared amplitudes, decay widths and
Wilson coefficients symbolically, and writes them as a standalone C++ library.

The code base has four main parts:

- **CSL** (`src/csl/`): the computer algebra kernel (expressions, tensors, simplification,
  numerical library generation).
- **MARTY** (`src/marty/`): the physics layer (models, Lagrangian, Feynman rules, diagrams,
  amplitudes, Wilson coefficients), with sub-modules SGL (Dirac algebra), UFO export, LHA
  input and a gamma API.
- **Plugins** (`src/marty-plugins/`): separate libraries that depend on MARTY.
- **GRAFED** (`src/grafed/`): Qt 5 GUI for drawing and displaying Feynman diagrams.

Supporting folders: `tests/` (gtest units, integration, and physics system tests), `examples/`,
`dep/` (vendored LoopTools), `docs/` (website, manuals, and developer docs).

Language: C++23 (GNU compilers, GCC 11 or newer, with matching `gfortran`). Build: CMake 3.22+.

## Documentation map

- Architecture and main functionalities: [docs/architecture/README.md](docs/architecture/README.md)
  - [Functionalities and code map](docs/architecture/functionalities.md)
  - [Build system and test suites](docs/architecture/build-and-tests.md)
- User manual: `docs/doc/marty-manual.pdf`; CSL manual: `docs/doc/csl-manual.pdf`
- Generated API docs: `docs/doc/marty/html/`, `docs/doc/csl/html/`
- Getting started example: `examples/gettingStarted.cpp`
- Installation and dependencies: [README.md](README.md)
- Contribution workflow: [CONTRIBUTING.md](CONTRIBUTING.md)

Every subfolder has its own short description. Start from the one closest to your change:

- `src/csl/` ([OVERVIEW.md](src/csl/OVERVIEW.md)) with `core/`, `utils/`, `algorithms/`,
  `numerical/`, `library/`
- `src/marty/` ([OVERVIEW.md](src/marty/OVERVIEW.md)) with `core/`, `models/`,
  `models/pmssm_lem/`, `sgl/`, `ufo/`, `lha/`, `api/`
- `src/marty-plugins/` ([OVERVIEW.md](src/marty-plugins/OVERVIEW.md)) with `boltzmann-solver/`
- `src/grafed/` with `core/`, `gui/`, `gui/ressources/`, `jsonparser/`
- `src/doc/` with `csl/`, `marty/`, `common/`
- `tests/` ([README.md](tests/README.md)) with `units/`, `integration/`, `system/`
- `examples/`, `dep/`, `docs/`, `assets/`, `.github/`, `.devcontainer/`

## Working rules for agents

- Respect the layering: CSL must not depend on MARTY; MARTY must not depend on its plugins;
  GRAFED is only reached through `Show()` (a `system()` call).
- Build out of tree (`mkdir build && cd build && cmake .. && make`), and run `ctest` before
  declaring a change done. Use `ctest -L units` (or `integration`, `system`) for a faster loop.
- Tests are slow, especially the system tests (very slow in Debug builds). Run only the tests
  relevant to your change (for example a single target such as `test_latex.x`, or
  `ctest -R <name>`), and avoid the full suite unless it is really needed.
- Test programs compare numbers against reference files in `tests/system/data/output/`. Do not
  edit reference data to make a test pass unless the physics change is intended and explained.
- Do not edit generated or third-party content: `build/`, `dep/LoopTools-2.16/`, and the
  Doxygen HTML under `docs/doc/`.
- Keep the Doxygen mainpages intact: `src/csl/README.md` and `src/marty/README.md` are used as
  the Doxygen main pages (`USE_MDFILE_AS_MAINPAGE`). Developer notes for those folders go in
  `OVERVIEW.md`.
- Code style: `.clang-format` at the repository root. Comment only code that needs
  clarification.
