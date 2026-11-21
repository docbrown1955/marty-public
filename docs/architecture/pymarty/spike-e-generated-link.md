# Spike E: compile and link a generated-style program against the build tree

Status: DONE for the compile-and-link question on this machine. Not tested: a wheel layout, an installed prefix, and
MARTY's actual generated library (no model was generated; the probe is a hand-written program).

## Question

Can a compiler on the user machine compile a small program against the headers and libraries of the build, and link it
(ABI, `libstdc++` version, rpath)? The loader question is deferred to Phase 6.

## Experiment

Probe: `/tmp/spike-e/gen_check.cpp`. It includes `marty/core/mrtError.h`, `marty/api/common/error.h` and `csl/api/error.h`,
calls `CallHEPError(mty::error::ValueError, ...)`, and catches `mty::error::Exception`.

Build and link (flags copied from `examples/CMakeFiles/gettingStarted.x.dir`, reduced):

```
g++-12 -std=gnu++23 -O2 -DMARTY_QUADRUPLE_ENABLED=1 -I<repo>/src gen_check.cpp \
  -L<build>/src/marty -L<build> -Wl,-rpath,<build>/src/marty:<build> \
  -lmarty-static -lgsl -lgslcblas -looptools -lgfortran
```

The same with `g++-10 -std=gnu++2a`.

## Result

- GCC 12: compiles with `-Wall -Wextra -Wpedantic` without warnings, links, runs, exit 0. The error is caught as
  `mty::error::Exception` with the expected message.
- GCC 10: compiles, links and runs, exit 0. A GCC 12-built static archive links into a GCC 10 program here.
- `ldd`/`readelf`: `NEEDED` lists only `libstdc++`, `libgcc_s`, `libc`. `RUNPATH` points into `/tmp/pymarty-build`
  (a build-tree path, not relocatable). `libmarty-static.a` is in `src/marty/`, not the build root, so `-L` must include it.
- `-looptools` was requested but is not in `NEEDED` (`--as-needed`); the probe uses no LoopTools symbol.

## Decision

- Go for compile-and-link with the flags above. The user compiler needs C++23 (`-std=gnu++23`, or `gnu++2a` on GCC 10).
- Required flags for generated libraries: `-I<prefix>/include`, `-L<prefix>/lib` (or the static archive), `-lgsl -lgslcblas`,
  `-looptools` (and `-lgfortran` when the Fortran object is pulled in), `-DMARTY_QUADRUPLE_ENABLED=1` if built with it.
- Phase 6 must replace the build-tree `RUNPATH` with `$ORIGIN`-relative paths or static linking for wheels (spike A).
