# Third-party and vendored code

`dep/` contains code MARTY needs but does not own.

- `CMakeLists.txt`: adds the LoopTools subdirectory to the build.
- `LoopTools-2.16/`: the LoopTools library (Fortran) for one-loop scalar integrals. It is
  used by MARTY's loop code (`src/marty/core/looptools_*.h`). See its own
  [README](LoopTools-2.16/README.md).

Do not modify this code. Update it by replacing the whole folder with a new upstream release.
