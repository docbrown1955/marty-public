# AGENTS.md

Guidance for AI coding agents and new contributors working on MARTY.

## What the project is

MARTY is a C++ code generator for perturbative calculations in Beyond-the-Standard-Model
physics. A model is written in C++ (particles, gauge groups, Lagrangian, symmetry breaking).
MARTY then computes Feynman rules, diagrams, amplitudes, squared amplitudes, decay widths and
Wilson coefficients symbolically, and writes them as a standalone C++ library.

Repository layout:

| Path | Role |
|------|------|
| `src/csl/` | CSL, the computer algebra system (symbolic expressions, tensors, simplification, numerical library generation) |
| `src/marty/` | MARTY proper: QFT models, Feynman rules, diagrams, amplitudes, Wilson coefficients, SGL (Dirac algebra), UFO export, LHA input |
| `src/marty-plugins/` | Separate libraries that depend on MARTY (currently the Boltzmann solver data exporter) |
| `src/grafed/` | GRAFED, the Qt 5 GUI to draw and display Feynman diagrams |
| `src/doc/` | Doxygen configuration for CSL and MARTY |
| `tests/` | Google Test unit and integration tests, and physics system tests |
| `examples/` | Stand-alone sample programs using the MARTY API |
| `dep/` | Vendored LoopTools (Fortran) used for one-loop integrals |
| `docs/` | Project website, manuals, talks, past releases, and developer documentation |

Language: C++23 (GNU compilers, GCC 11 or newer, with matching `gfortran`). Build: CMake 3.22+.

## Documentation map

- Architecture: [functionalities and code map](docs/architecture/functionalities.md) (pipeline,
  source files per step), [build system and test suites](docs/architecture/build-and-tests.md)
- Reading order: this file, then `functionalities.md`, then the `OVERVIEW.md` of the folder you
  work in, then the Doxygen pages for exact class interfaces
- Python binding plan (pymarty): [docs/architecture/pymarty/README.md](docs/architecture/pymarty/README.md)
- How pymarty phases are run (subagent roles, gates, files):
  [docs/architecture/pymarty/process/README.md](docs/architecture/pymarty/process/README.md)
- User manual: `docs/doc/marty-manual.pdf`; CSL manual: `docs/doc/csl-manual.pdf`
- Generated API docs: `docs/doc/marty/html/`, `docs/doc/csl/html/`
- Getting started example: `examples/gettingStarted.cpp`
- Installation and dependencies: [README.md](README.md)
- Contribution workflow: [CONTRIBUTING.md](CONTRIBUTING.md)

Start from the overview closest to your change: [src/csl/OVERVIEW.md](src/csl/OVERVIEW.md),
[src/marty/OVERVIEW.md](src/marty/OVERVIEW.md), [src/marty-plugins/OVERVIEW.md](src/marty-plugins/OVERVIEW.md),
or [tests/README.md](tests/README.md).

## Working rules for agents

- Respect the layering: CSL must not depend on MARTY; MARTY must not depend on its plugins;
  GRAFED is only reached through `Show()` (a `system()` call).
- Build out of tree (`mkdir build && cd build && cmake .. && make`), and run `ctest` before
  declaring a change done. Use `ctest -L units` (or `integration`, `api`) for a faster loop.
- Tests are slow, especially the system tests (very slow in Debug builds). Run only the tests
  relevant to your change (for example a single target such as `test_latex.x`, or
  `ctest -R <name>`), and avoid the full suite unless it is really needed.
- Never run the `system` suite (`ctest -L system`, or an unfiltered `ctest`) and never schedule it in
  CI: the maintainer runs it by hand from time to time. Run a single named system test with
  `ctest -R <name>` only when debugging it, and list the system tests a change affects in the PR
  description instead.
- Test programs compare numbers against reference files in `tests/system/data/output/`. Do not
  edit reference data to make a test pass unless the physics change is intended and explained.
- Do not edit generated or third-party content: `build/`, `dep/LoopTools-2.16/`, and the
  Doxygen HTML under `docs/doc/`.
- Keep the Doxygen mainpages intact: `src/csl/README.md` and `src/marty/README.md` are used as
  the Doxygen main pages (`USE_MDFILE_AS_MAINPAGE`). Developer notes for those folders go in
  `OVERVIEW.md`.
- Code style: `.clang-format` at the repository root. Comment only code that needs
  clarification.

## Python binding (pymarty)

- Layout: extension `_pymarty` built from `python/src/module.cpp` and `python/src/symbolic.cpp`
  (pybind11); Python package in `python/pymarty/` (wrappers `symbolic.py`, `config.py`,
  `errors.py`; stubs `_pymarty.pyi`, `symbolic.pyi`, `py.typed`). The build copies the package to
  `<build>/python/pymarty/`.
- Build: configure with `-DMARTY_BUILD_PYTHON=ON -DPython_EXECUTABLE=<venv python>` (venv with pytest),
  then `cmake --build . --target _pymarty -j`. Source builds only; wheels are not published.
- Tests (from the build directory): `ctest -L python` runs the pytest suite and the parity check.
  Direct pytest: `cd /tmp && PYTHONPATH=<build>/python <venv>/bin/python -m pytest -c python/pyproject.toml
  --rootdir python python/tests -m "not slow" -q`. Never drop `-m "not slow"`.
- Gates (in `python/`): `ruff check .`, `ruff format --check .`, `mypy --strict pymarty tests tools`.
  Parity: `PYTHONPATH=<build>/python <venv>/bin/python python/tools/check_parity.py` from the repo root.
  C++ format: `clang-format --dry-run --Werror` on the edited `python/src/*.cpp`.
- Docstrings: every public class, function and property has a docstring. Examples use `>>>` and are
  executed by `python/tests/symbolic/test_docstrings.py`; keep them correct.
- Names: keep the C++ names internally; the Python names are `infinity` (C++ `oo`) and `replace`
  (C++ `subs`). Do not write `oo` or `subs` in Python docs. Printed output is CSL text (`INF`).
- Layering: CSL must not depend on MARTY. The facade `src/csl/api/` must not include MARTY headers.
- Stub sync: `_pymarty.pyi`, `symbolic.pyi` and `symbolic.py` must agree when a binding changes.
- `Expr.depends_on` follows CSL: variables may depend on each other, so a variable absent from a sum
  still reports `True`. Use a `constant()` to get `False`.
- Forbidden: `ctest -L system`, unfiltered `ctest`, and pytest without `-m "not slow"`.
