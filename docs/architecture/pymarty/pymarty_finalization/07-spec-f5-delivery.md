Status: DRAFT (not frozen; revised after the F4 gate)
Updated: 2026-11-28
Author: main agent (orchestrator)
Depends on: F1, F4

# F5 spec: delivery (wheels, Docker image, doctor, licences, GRAFED optional)

Scope: distribution of pymarty and of MARTY, with the decisions of Q4, Q6, Q6b, Q6c and Q6d.
Source: [phase-8-polish-release.md](../phase-8-polish-release.md) WP8.3, WP8.4, WP8.6,
[USER_FEEDBACK.md](../USER_FEEDBACK.md) "Software Delivery", [02-questions.md](./02-questions.md).

## 1. Decisions already recorded

- **Q4:** first release includes `doctor` (text and JSON), notebooks executed in CI, sdist built and tested in CI,
  and the process-wide mutex. Diagrams and GIL release and cancellation are deferred.
- **Q6:** Linux x86_64 wheels, CPython 3.10 to 3.13 (Q3). No ARM, because the maintainer cannot test it. No
  compiler in the wheel. A Docker image holds the compilers.
- **Q6b (LoopTools, LGPL v3):** ship the two shared objects, `libooptools.so` and `libooptools-quad.so`, with the
  LGPL text. Static builds are not shipped in the wheel.
- **Q6c:** the Docker image and the wheel bundle libgfortran, libquadmath, GSL (`gsl`, `gslcblas`) and the two
  LoopTools shared objects. The wheel and the image carry the GPL v3 and LGPL v3 texts, and a notices file. No
  compiler is bundled in either. libgfortran and libquadmath are under GPL v3+ with the GCC Runtime Library
  Exception, which allows bundling. GSL is GPL v3, compatible with MARTY's GPL-3.0-or-later (`LICENSE`).
- **Q6d (GRAFED):** GRAFED is enabled by default, but optional. A missing LaTeX tool or Qt 5 gives a CMake warning
  and disables only that feature. `MARTY_REQUIRE_LATEX_TOOLS` defaults to OFF
  (`CMakeLists.txt:15-18`). `src/grafed/gui/CMakeLists.txt:7-9` changes from REQUIRED to optional.
  The Docker image uses a minimal TeX Live set.

## 2. Open decisions (to record before G1 of F5)

- The minimal TeX Live package set for lualatex output. To be checked by building one diagram in the image.
- Whether `auditwheel repair` accepts the bundled libgfortran and GSL under manylinux_2_28, and which exclusions
  (if any) are needed.
- Docker base image and registry, and whether the image is built in CI or by hand.
- Deprecation policy length (phase 8 WP8.6 proposes a removal version in each warning).

## 3. Steps

1. Change the GRAFED defaults: `CMakeLists.txt:15-18` (`MARTY_REQUIRE_LATEX_TOOLS` OFF) and
   `src/grafed/gui/CMakeLists.txt:7-9` (Qt 5 and GSL optional with a warning). Test with CMake configured
   without Qt and without LaTeX: the configure step succeeds and the bare build works.
2. Make `doctor` (`python -m pymarty doctor`): reports pymarty and MARTY versions, Python, compiler (path, version,
   builds a trivial library), `make`, LoopTools (double and quad), quad precision, GRAFED found, LaTeX tools,
   writable cache dir. Exit code is non-zero only when a requested feature is missing. Text and JSON output.
3. Mutex: a process-wide recursive mutex in the bindings (`src/pymarty/cpp_binding/`), with a test of two threads
   building different models.
4. Wheel build: cibuildwheel matrix (manylinux_2_28, CPython 3.10 to 3.13), `auditwheel repair`, bundled libraries
   listed in a manifest inside the wheel. The root `pyproject.toml` (F1) is the build entry point.
5. Licence and notices: GPL v3 text, LGPL v3 text, and a notices file listing GSL, LoopTools, libgfortran and
   libquadmath with their versions. A CI script compares the bundled libraries with the manifest (criterion 7).
6. Docker image: minimal base with gfortran, gcc, GSL development headers, quad precision support and a minimal
   TeX Live set. It installs the Qt 5 GRAFED package optionally. The image runs the `doctor` command and the
   toolchain test job (phase 8 test plan).
7. Test jobs: "no toolchain" (wheel only, features needing a compiler raise `BuildError`) and "toolchain" (wheel plus
   a library build and evaluation). Both run the fast subset (`-m "not slow"`).
8. sdist: builds with CMake on a user machine, tested in CI for one job. Documented in the installation page of F4.
9. Versioning and deprecation: `pymarty.__version__` equals the MARTY version. A deprecation helper emits
   `DeprecationWarning` with the removal version (WP8.6).
10. Release rehearsal: publish to the test index, install from it in a clean environment, record the log.

## 4. Verification

- Configure and build with the GRAFED options off, and with the defaults, in a clean build directory.
- `doctor` tests: all tools present; `PATH` without a compiler; `MARTY_GRAFED` pointing to a fake script; exit codes.
- Wheel tests in a clean virtual environment, both job types.
- `auditwheel show` and the manifest CI script.
- The full manual suite (L5) is run by the maintainer on the release candidate, and reported, not a gate.
- Never run the `system` CTest suite in CI (AGENTS.md).

## 5. Acceptance

Phase 8 criteria 1, 5, 7, 8, and 9 (security review is a separate PR, see testing-strategy section 7), plus:

- The GRAFED defaults in section 1 are in place and tested.
- The image and the wheel carry the notices and the licence texts.

## 6. Out of scope

- Inline diagrams and GIL release or cooperative cancellation (deferred by Q4).
- ARM builds (no test machine).
- Boltzmann-solver plugin packaging (phase 7 Q2, separate decision).

## Amendments

(none)
