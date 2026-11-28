# pymarty finalization: questions

Status: OPEN (2026-11-28). Evidence: [01-inventory.md](./01-inventory.md). The questions are in two batches
of at most four, as in [../process/orchestrator.md](../process/orchestrator.md). Batch 2 is asked after batch 1
is answered, because some answers change it.

## Batch 1: layout and release baseline

### Q1. Restructure first, as a move-only PR?

- Context: the USER_FEEDBACK layout moves every path in the repository (`python/pymarty`, `python/tests`,
  `python/examples`, `examples/`, `dep/`). The phase documents, `AGENTS.md` and CI cite those paths
  (`AGENTS.md:23`, `AGENTS.md:62`, `CMakeLists.txt:111`).
- Options:
  - (a) F1 is a move only, no code or API change, with the same test counts before and after. Path references are
    updated in the same PR.
  - (b) Combine the move with the API renames of F3, one PR.
- Recommendation: (a). A move-only PR can be reviewed by diff and rolled back easily; it blocks F2 and F3.

### Q2. Name of the built-in models folder

- Context: USER_FEEDBACK says "I'm not a big fan of `builtin_models`, but `models` seems a bit vague". The Python
  built-in models are in `python/pymarty/modeling/models/` (`sm.py`, `gthdm.py`, ...).
- Options: (a) `builtin_models`; (b) `models`; (c) another name.
- Recommendation: (a). It is clear next to `pymarty.modeling.models`, which is the Python import path and would
  otherwise be confused with a folder of the same name.

### Q3. Minimum Python version and CPython matrix

- Context: `python/pyproject.toml:9` says `requires-python = ">=3.10"`. Phase 8 WP8.4 proposes CPython 3.9 to 3.13.
  Python 3.9 reached end of life in October 2025.
- Options:
  - (a) 3.10 to 3.13 (matches `pyproject.toml`, no 3.9 jobs).
  - (b) 3.9 to 3.13 (phase 8 proposal).
- Recommendation: (a). It removes an end-of-life runtime from the CI matrix and keeps the current floor.

### Q4. What is in the first release (scope of phase 8)

- Context: phase 8 lists items that are not started (`doctor`, inline diagrams, concurrency policy, notebooks
  executed in CI, sdist). Concurrency and cancellation touch core loops (phase 8 section 6). Diagram rendering
  depends on an open spike.
- Options (multi-select):
  - `doctor` command with text and JSON output
  - inline diagram rendering (`_repr_svg_` or `visualization.render`)
  - single-thread policy with a process-wide mutex
  - GIL release and cooperative cancellation for long calls
  - notebooks executed in CI
  - source distribution (sdist) built and tested in CI
- Recommendation: `doctor`, notebooks in CI, sdist, and the mutex. Defer diagrams and GIL release/cancellation to a
  later release: they change core behaviour and need their own spike. The mutex is a small, low-risk change.
- Blocks: F5 specification and the wheel test plan.

## Batch 2: examples, delivery and gaps (asked after batch 1)

### Q5. Which usage examples become notebooks

- Context: USER_FEEDBACK asks for notebooks where LaTeX output or diagrams help. Today there is one notebook
  (`python/notebooks/gthdm_investigation.ipynb`), and it is not executed by any test.
- Options: (a) only the complex examples (gettingStarted and the SM example) as notebooks, the three module
  examples as scripts; (b) all examples as notebooks; (c) all scripts, no notebooks.
- Recommendation: (a). The notebooks carry the LaTeX and diagram output; the short module examples stay scripts that
  run as tests.
- Blocks: F2.

### Q6. Wheel contents and compiler

- Context: generated libraries need a compiler and LoopTools at run time (phase 6, phase 8 WP8.4). The phase 8 test
  plan has a "no toolchain" job and a "toolchain" job.
- Options:
  - (a) Wheel without a compiler; features that need one raise `BuildError` with a clear message; LoopTools bundled
    if its licence allows it.
  - (b) Wheel bundles a compiler toolchain (large, licence and maintenance cost).
  - (c) Wheel without LoopTools; one-loop features are documented as needing a source build.
- Recommendation: (a). It matches the phase 8 acceptance criterion 1 and keeps the wheel small. Check the LoopTools
  licence before bundling it (not verified here).
- Blocks: F5.

### Q7. Gap-filling order after the API is polished (F6)

- Context: the open items are listed in [01-inventory.md](./01-inventory.md) section E. The Wilson API refactor
  (`docs/architecture/todo/wilson-coefficient-api.md`) is needed by the BsMixing and `wilsons_GTHDM` ports and changes
  public names. Model serialisation (`model-serialization.md`) is a large project that unblocks MSSM, PMSSM and NMFV.
- Options:
  - (a) Wilson API refactor first, then the remaining ports (BsMixing, wilsons_GTHDM, gm2SM, FItests, Bgenesis,
    freeQuarkAmplitudes), then model serialisation as its own phase.
  - (b) Model serialisation first, then the SUSY ports.
  - (c) Ports first, with the current Wilson API, and the refactor after.
- Recommendation: (a). The refactor changes names that the ports would otherwise use. Serialisation is independent
  and starts when the release is out.
- Blocks: F6 specification.

## Q6 follow-up: prebuilt artefacts and a Docker image

Proposal from the user: prebuilt binaries and/or a Docker image that ships the compilers needed to build generated
libraries, for maximal portability.

Two different things are being bundled, and they have different costs:

- The pymarty extension (`_pymarty`) is a compiled module. A wheel per platform and Python version is cheap to build
  (for example with cibuildwheel) and needs no compiler on the user side for the Python API itself.
- Generated libraries are C++ sources built at run time. They need a C++23 compiler (GCC 11 or newer), a matching
  `gfortran`, CMake, and LoopTools for one-loop integrals. A wheel alone does not solve this.

Options:

- (a) Docker image with GCC, gfortran, CMake, LoopTools, Python and MARTY. Portable on any system with Docker, reproducible,
  and it is the only option that covers the toolchain fully. Costs: image size, a rebuild for each release, amd64 and
  arm64 builds, and users on macOS or Windows need Docker Desktop.
- (b) Wheels for Linux x86_64 and arm64 without a compiler, plus the image for the toolchain. Wheels cover users who
  only drive the Python API; features that need a compiler point to the image. Costs: two artefacts to test.
- (c) Wheels bundling the toolchain. Rejected in practice: large, and it puts GCC and LoopTools licences into every wheel.
- (d) Source build only (current state).

Things to check before choosing: the LoopTools licence (not verified), and the GCC version range the generated code
needs (AGENTS.md says 11 or newer).

Recommendation: (b). The image is the portable runtime for generated libraries, and the wheels are a convenience. The
image's Dockerfile can be tested in CI as part of F5.

Blocks: F5 (packaging and the test plan).

## Q6b: LoopTools licence and variants (checked 2026-11-28)

- Licence: GNU LGPL v3, from `dep/LoopTools-2.16/COPYING:1-2`. Bundling is allowed, with conditions:
  - Ship the licence text and a notice with each wheel and image.
  - Ship the library as a shared object that users can replace. Do not statically link it into `_pymarty` or into a
    wheel-built binary without also providing object files or a relinking path (LGPL v3 section 4).
  - Source must remain available. It is vendored in `dep/LoopTools-2.16/`, so this is met.
- Variants: `dep/LoopTools-2.16/CMakeLists.txt:1-47` builds both double precision (`libooptools.so`, `-static.a`) and
  quad precision (`libooptools-quad.so`, `-quad-static.a`, configured with `--quad`). The top-level install
  (`CMakeLists.txt:126-129`) already installs both shared and static quad and double libraries. So the build already
  produces both variants; the wheel and image must ship both shared objects.
- Not verified: whether the quad library is built on every platform, or only when the Fortran compiler supports
  real(16). The CMake target is unconditional, so the build fails loudly if it does not. Check in F5.

Decision for F5: the wheel and image ship `libooptools.so` and `libooptools-quad.so`, plus the LGPL text. Static
variants are not shipped.

## Q6c: bundling gfortran runtime, quadmath and GSL (checked 2026-11-28)

Generated libraries link these at run time, so each one is needed in the wheel or the image:

- `src/csl/librarygenerator.cpp:874` adds `-lgsl -lgslcblas` to every generated makefile.
- `src/csl/libmakefile_data.h:61` adds `-lquadmath`.
- `src/csl/librarygenerator.cpp:869` adds `-lgfortran` when LoopTools is used.

| Component | Licence (Debian `copyright` files) | Bundling |
|-----------|-------------------------------------|----------|
| libgfortran5 | GPL v3 or later, with the GCC Runtime Library Exception 3.1 | Allowed. The exception lets it be distributed without making the rest GPL. |
| libquadmath0 | GPL v3 or later, with the GCC Runtime Library Exception 3.1 (some files are LGPL 2.1) | Allowed. Same as above. |
| GSL (libgsl27, libgslcblas0) | GPL v3 (no runtime exception, no LGPL option) | Allowed because MARTY is GPL-3.0-or-later (`LICENSE`, `python/pyproject.toml:10`). The whole distributed artefact is GPL-compatible. Ship the GPL text and the source offer. |
| LoopTools | LGPL v3 (see Q6b) | Shared object only. |

Consequences:

- The Docker image can bundle all four. Installing them from the distribution's packages is simplest, and the image
  already needs them for the build.
- For the wheel: bundling libgfortran, libquadmath and GSL shared objects is possible (for example with auditwheel
  or delocate, which copy them in and check the manylinux policy). The wheel is then no longer a small, compiler-free
  package. It remains a Linux x86_64 wheel as decided in Q6.
- Not verified: the exact sonames and whether the manylinux policy accepts libgfortran and GSL from the base image.
  Check in F5 with auditwheel.
- Source offer: MARTY's source is already public (this repository), so the GPL source requirement is met for GSL,
  libgfortran and libquadmath as long as the release points to the source.

Decision for F5 (proposed, to confirm): the Docker image bundles libgfortran, libquadmath, GSL and LoopTools, all
installed from the distribution. The Linux wheel bundles libgfortran, libquadmath, GSL and the two LoopTools shared
objects, with the GPL and LGPL texts in the wheel. No compiler is bundled.

## Q6d: GRAFED dependencies are optional for the wheel and the bare build (checked 2026-11-28)

Current state:

- `CMakeLists.txt:15-18`: `MARTY_REQUIRE_LATEX_TOOLS` defaults to ON, and `MARTY_BUILD_GRAFED_GUI` defaults to ON.
- `CMakeLists.txt:25-50`: with the strict default, a missing `lualatex`, `dvipng` or `convert` is a `FATAL_ERROR`. A bare
  MARTY build therefore fails on a machine without LaTeX, even if the user never draws a diagram.
- `src/grafed/gui/CMakeLists.txt:7-8`: `find_package(Qt5 ... REQUIRED)`. A missing Qt is also a hard failure when the GUI is
  enabled.
- `src/grafed/CMakeLists.txt:4`: the GUI subdirectory is only added when `MARTY_BUILD_GRAFED_GUI` is ON. The
  `src/grafed/core` library has no Qt dependency and is always built.
- `.github/workflows/python.yml:44-45`, `80-81`: the Python CI already passes `-DMARTY_BUILD_GRAFED_GUI=OFF` and
  `-DMARTY_REQUIRE_LATEX_TOOLS=OFF`. So the wheel build path is already tested without these tools.
- Python side: `python/pymarty/visualization/__init__.py:58-61` documents that `render_png` requires `latex` and `dvipng`
  and raises `MartyStateError`. `python/pymarty/visualization/grafed.py:38` raises when `grafed-gui` is not found.
  `latex()` returns a string and needs no tool.

Proposal:

1. Bare MARTY build: a missing tool or Qt only disables the matching feature and prints a status line. No hard failure.
   `MARTY_REQUIRE_LATEX_TOOLS` defaults to OFF. The GUI is built only when Qt 5 is found (`find_package` without
   `REQUIRED`, and the GUI subdirectory gated on `Qt5_FOUND`). The explicit `ON` setting keeps the strict behaviour for
   developers who want it. This is a change of default and is listed in F5.
2. Wheel: built with GUI OFF and no LaTeX requirement, as in the CI today. It does not bundle Qt, LaTeX or ImageMagick.
   Importing `pymarty`, running calculations, `latex()` and `save()` work without them. `render_png`, `show` and
   `grafed` raise `MartyStateError` or `BuildError` with the name of the missing tool.
3. Docker image: installs `texlive` (lualatex, latex), `dvipng`, ImageMagick (`convert`), and Qt 5, and builds
   `grafed-gui` with the GUI ON. Everything in the image works.
4. The Python API states this per function: which tool each visualisation function needs, and the exception raised when
   it is missing. Non-visualisation functions never need these tools.

Not verified: whether `texlive` in the image should be the full or a minimal package set (size trade-off, to decide in F5).
The C++ behaviour in step 1 needs a test with the tools absent.

| Question | Answer | Date |
|----------|--------|------|
| Q6d GRAFED dependencies | decided: GRAFED enabled by default; a missing tool or Qt 5 gives a warning and disables only that feature (no hard failure). `MARTY_REQUIRE_LATEX_TOOLS` defaults to OFF, `MARTY_BUILD_GRAFED_GUI` stays ON and is skipped with a warning when Qt 5 is absent. Docker image uses a minimal TeX Live set. | 2026-11-28 |
| Q1 restructure first | move-only PR first (recommended) | 2026-11-28 |
| Q2 built-in models folder | `builtin_models` (recommended) | 2026-11-28 |
| Q3 Python versions | 3.10 to 3.13 (recommended) | 2026-11-28 |
| Q4 first release scope | doctor, notebooks in CI, sdist, mutex (recommended) | 2026-11-28 |
| Q5 notebooks | all examples as notebooks | 2026-11-28 |
| Q6 wheel contents | wheels (Linux x86_64 only, no compiler, no ARM: untestable by the maintainer) plus Docker image with the toolchain. LoopTools: LGPL v3, bundle as shared libraries (both double and quad). | 2026-11-28 |
| Q7 gap order | Wilson API refactor first, then ports, then serialisation (recommended) | 2026-11-28 |
| D1 models import path (F1) | keep `pymarty.modeling.models`; build copies `builtin_models/python/` into the package (recommended) | 2026-11-28 |
| D2 C++ built-in models (F1) | move only definitions shared by several programs to `builtin_models/cpp/`; inventory is step 0 of F1 (recommended) | 2026-11-28 |
| Q6c runtime libraries | confirmed: Docker image bundles libgfortran, libquadmath, GSL and LoopTools; Linux wheel bundles the same runtime libraries and the two LoopTools shared objects, with GPL and LGPL texts; no compiler in either. | 2026-11-28 |
