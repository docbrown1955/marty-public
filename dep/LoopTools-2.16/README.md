# LoopTools 2.16

Vendored copy of LoopTools 2.16 (Fortran), the one-loop integral library by T. Hahn and M. Pérez-Victoria.

- `src/`: Fortran sources grouped by function (subfolders such as `A/` and `B/` for the
  one-point and two-point functions).
- `build/` and `build-quad/`: double and quadruple precision build trees.
- `configure`, `makefile.in`, `makefile`: the upstream build system. MARTY compiles it through `dep/CMakeLists.txt`.
- `manual/`: the LoopTools guide (`LT216Guide.pdf`, `LT216Guide.tex`).

Third-party code: keep it unchanged. MARTY wraps it through `src/marty/core/looptools_*.h`.
