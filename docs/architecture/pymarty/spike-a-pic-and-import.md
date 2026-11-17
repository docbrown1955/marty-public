# Spike A: position-independent code and extension import

Status: PARTIAL. The PIC audit and the import in the development build are done. The relocatable wheel build is not (see spike D).

## Question

Can the core static libraries be linked into a Python extension module, and which shared libraries does the
extension need at run time?

## Experiment

1. Static audit (grep of every `add_library` and `set_target_properties` in `src/`).
2. Configure `/tmp/pymarty-build` (Release, g++-12, gfortran-10) with `-DMARTY_BUILD_PYTHON=ON`; build target `_pymarty`.
3. `ldd` and `readelf -d` on `pymarty/_pymarty.cpython-310-x86_64-linux-gnu.so`.
4. Import under the venv (`/tmp/pymarty-venv`, Python 3.10.12) with `PYTHONPATH` pointing at the build tree.

## Result

- PIC audit: every object library sets `POSITION_INDEPENDENT_CODE` (csl-core, grafed-core, jsonparser,
  marty-lha-obj, marty-core, sgl, marty-models, marty-ufo, marty-gamma-api, marty-boltzmann-solver-core).
  Static targets are built from these PIC objects.
- Build: `_pymarty` compiled and linked with 0 `error:` lines. pybind11 v2.11.2 (system 2.9.1 is too old, so the
  `find_package(pybind11 2.11)` path was not taken).
- `ldd`: depends only on libstdc++, libgcc_s, libc, libm. `libooptools.so` is not in the `NEEDED` list, even though
  `marty-static` links it (the linker dropped it because no symbol from it is referenced).
- `readelf`: `RUNPATH` is `/tmp/pymarty-build`, i.e. the build tree. This must be fixed before a wheel is shipped.
- `import pymarty`: works, `__version__` is `0.1.0.dev0`.

## Fix list before a wheel

1. Replace the build-tree `RUNPATH` with `$ORIGIN` (or statically link everything used) and make the wheel
   self-contained.
2. Confirm which LoopTools symbols are referenced; if any, bundle `libooptools.so` or link it statically.
3. Verify the same on a manylinux image (blocked: see spike D).
