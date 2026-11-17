# Spike D: manylinux wheel feasibility

Status: BLOCKED (environment). No wheel was built.

Update: deferred by decision. Open points and the suggested order of work are in
[distribution-deferred.md](distribution-deferred.md); this record stays as evidence.

## Question

Can the manylinux image build LoopTools (Fortran), link GSL, bundle `libgfortran` with
`auditwheel`, and does the installed wheel import in a clean venv without compilers?

## Experiment

1. Docker availability: the Docker CLI exists only as a Windows binary
   (`/mnt/c/Program Files/Docker/Docker/resources/bin/docker`). Invoking it from WSL returns
   "docker could not be found in this WSL 2 distro"; WSL integration in Docker Desktop is not
   enabled. No manylinux image could be pulled or run.
2. `auditwheel` and `cibuildwheel` are not installed on the host (`which` returns nothing).

## Result

- No manylinux build, no `auditwheel show`, no wheel size.
- Host toolchain only: GCC 10/11/12, gfortran 10, Python 3.10.12, pybind11 2.9.1 (system).
  pybind11 ≥ 2.11 is fetched by CMake (see `python/CMakeLists.txt`).
- Likely requirement found while reading `src/marty/CMakeLists.txt`: `marty-static` links the
  shared `${PROJECT_BINARY_DIR}/libooptools.so`. A wheel must bundle it (or statically link
  LoopTools) plus `libgfortran`; `auditwheel repair` only handles this if the library is in the
  wheel's `RPATH` search path. This is unverified.

## Decision

Not closed. Next steps, owner to be agreed:

1. Enable Docker Desktop WSL integration (or use a Linux host) and run a `quay.io/pypa/manylinux_2_28`
   build of the `python/` target.
2. Decide between static LoopTools (`libooptools.a`) and bundling `libooptools.so` plus
   `libgfortran` via `auditwheel repair`.
3. Record wheel size and the `auditwheel show` tag then.

Mitigation agreed for the phase: keep `MARTY_BUILD_PYTHON` default OFF, and treat wheel packaging
as a Phase 0 exit blocker (AC6) until this record is updated.

## Addendum: host (non-manylinux) wheel attempt

Status: PARTIAL. A wheel was built on the host, installs and imports in a clean venv. It is not
a manylinux wheel, so AC6 stays open.

Commands (all under `/tmp`, nothing written in the repository):

```
python3 -m venv /tmp/pymarty-wheelvenv   # auditwheel 6.8.2, patchelf; build deps (scikit-build-core 1.1.2, pybind11 3.1.0) fetched by pip build isolation
CC=gcc-12 CXX=g++-12 FC=gfortran-12 pip wheel python/ -w /tmp/p0-wheelhouse
python3 -m venv /tmp/p0-freshvenv && /tmp/p0-freshvenv/bin/pip install /tmp/p0-wheelhouse/*.whl
```

Results:

| Item | Result |
|------|--------|
| Build | 284 targets, 5 min 51 s wall (49 min CPU), GCC 12.3.0, Ninja |
| Wheel | `pymarty-0.1.0.dev0-cp310-cp310-linux_x86_64.whl`, 29 447 826 bytes, 330 files |
| Fresh venv | `pip install` OK; `import pymarty` prints `0.1.0.dev0` |
| Tests | the 37 Python tests pass against the installed wheel (cwd `/tmp`, no `PYTHONPATH`) |
| `auditwheel show` | consistent with `manylinux_2_34_x86_64` (glibc 2.34, GLIBCXX_3.4.29 from GCC 12 libstdc++) |
| `auditwheel repair` | fails: `required library "libmarty.so" could not be located` |

Findings:

1. `_pymarty*.so` (5.5 MB) links the static libraries. `ldd` shows only libstdc++, libm, libgcc_s,
   libc. It needs no bundling today.
2. The scikit-build install step runs the top-level `install` rules, so the wheel also contains
   `pymarty/_data/` with the shared and static libraries, GoogleTest, headers and a pkg-config
   tree (295 headers). `libmarty-*.so` have `NEEDED libmarty.so` and no `RPATH`, which is why
   `auditwheel repair` cannot resolve them. Phase 6 (generated libraries) needs `_data/{lib,include}`
   but not gtest or the static archives. To do: install only the needed components (a dedicated
   `install(... COMPONENT python)` plus `install.components` in `pyproject.toml`), and set
   `RPATH $ORIGIN` on the shared libraries.
3. The build compiles all targets (tests, examples) because the top-level install depends on
   them. `build.targets = ["_pymarty"]` or a component install would cut the build time.
4. pybind11 3.1.0 rejected a `py::make_tuple` ternary in `defineError`; fixed in
   `python/src/module.cpp` (it compiles with 2.11+ and 3.x). Pin or test both in CI.
5. The default `gcc` on the host is 10.5, so `CC`, `CXX`, `FC` must be set explicitly.
6. LoopTools: `libooptools.so` and `libgfortran`/`libquadmath` (referenced by the shipped
   `libmarty.so`) would have to be bundled by `auditwheel repair`, or LoopTools linked statically.
   This is still undecided.

Still blocked: manylinux build in a container (no container runtime), wheel size on manylinux,
`cibuildwheel` run. The `wheel` job in `.github/workflows/python.yml` is untested.
