# Distribution and manylinux: deferred

Status: deferred by decision (Nov 17, 2026). Nothing in Phases 0 to 7 depends on a distributable wheel.
Development and CI use source builds (`MARTY_BUILD_PYTHON=ON`). This note records what is known so the topic
can be picked up later without redoing the investigation. Evidence: [spike-d-wheel-feasibility.md](spike-d-wheel-feasibility.md)
and [spike-e-generated-link.md](spike-e-generated-link.md).

## What works today

- A wheel builds on the host (GCC 12, scikit-build-core, about 6 min, 29.4 MB), installs in a clean venv, and
  the Python tests pass against it. It is tagged `linux_x86_64`, not manylinux.
- `auditwheel show` reports a `manylinux_2_34` compatibility only (glibc 2.34, libstdc++ from GCC 12).

## Known problems

1. No manylinux build: needs a container runtime (Docker Desktop WSL integration is off on the dev machine) or a
   CI job. The `wheel` job in `.github/workflows/python.yml` has never run.
2. Wheel content is too large: the top-level `install` rules put shared and static libraries, GoogleTest, about
   295 headers and pkg-config files in `pymarty/_data/`. The build also compiles every target (tests, examples).
3. `auditwheel repair` fails: `libmarty-*.so` have `NEEDED libmarty.so` and no `RPATH`.
4. LoopTools and the Fortran runtime (`libgfortran`, `libquadmath`): bundle with `auditwheel`, or link LoopTools
   statically. Undecided.
5. Compile-time paths: `MARTY_CXX` and `MARTY_INSTALL_PATH` are baked into the C++ library. `Config`
   ([phase-0-foundations.md](phase-0-foundations.md), WP0.6) is the planned run-time replacement.
6. Generated libraries (Phase 6) need a compiler on the user machine, compatible with the wheel's libstdc++ and
   libgfortran. Spike E showed the compile-and-link step works against a build tree only.
7. pybind11: the code must build with 2.11 and 3.x (a `py::make_tuple` ternary was rejected by 3.1).
8. Source trees for sdist or wheel builds: `docs/code/MARTY-*.zip` are in Git LFS, so exclude `docs/`.

## Questions to settle

- Supported Python versions (proposal 3.9 to 3.13) and platforms (Linux x86_64 only for now; aarch64 later).
- Image and compiler: `manylinux_2_28` with a `gcc-toolset`, or another baseline.
- Package name on PyPI and ownership; version policy (`pymarty.__version__` versus the MARTY version, currently
  the placeholder `0.1.0.dev0`).
- Whether the wheel ships headers and libraries for generated libraries, or Phase 6 requires a MARTY install.
- Licensing: GPL v3 text, notices and source pointers for bundled libraries (GSL, LoopTools, libgfortran).
- Shared versus static MARTY libraries inside the wheel.

## Suggested order when resumed

1. Install only a `python` component (`install.components` in `pyproject.toml`) and build only `_pymarty`; set
   `RPATH $ORIGIN` on shared libraries.
2. Decide static versus bundled LoopTools; make `auditwheel repair` pass.
3. Run the build in a manylinux container (local or CI); record wheel size and the tag.
4. Install in a clean venv without compiler or Fortran runtime; run `import pymarty` and the error round trip.
5. Re-run spike E against the installed wheel layout.
6. Then the Phase 8 items: wheel matrix, `doctor`, release process, license compliance.

## Where this touches the plan

- Phase 0 acceptance criterion 6 and spike D: moved here, no longer blocking Phase 0.
- Phase 6 (`simulation`): relies on `Config`, not on a wheel.
- Phase 8: wheel matrix, release and compliance items start from this note.
