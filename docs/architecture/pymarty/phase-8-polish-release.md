# Phase 8: polish and release

Makes the package ready for users: complete stubs and documentation, notebooks, inline diagrams,
wheel matrix, license compliance, and the release process. Several items can start earlier (stubs,
docs, notebooks follow each module); they are collected here for acceptance.

Prerequisites: Phases 0 to 7 accepted (or the unaccepted parts listed as known limitations).

## 1. Scope

- Type stubs and documentation for all modules.
- Notebook experience: `_repr_latex_`, diagram display in Jupyter.
- `python -m pymarty doctor`: environment diagnostics.
- Wheel matrix and release process, license and source compliance.
- Concurrency and interruption policy.
- Versioning and deprecation policy.

## 2. Work packages

### WP8.1 Stubs and docs
- Stubs generated from the pybind11 module with the stub generator, then curated; `py.typed` shipped.
  CI regenerates them and fails on a diff.
- Documentation site: one page per module generated from docstrings plus tutorials (ports of
  `gettingStarted`, QED amplitude, group theory tour, gamma algebra, library evaluation). Every code
  block is executed in CI (doctest or notebook execution).
- Migration notes "from the C++ API to pymarty": the name tables of the phase documents collected into
  one reference.

### WP8.2 Notebooks and diagrams
- Notebook demos under `python/examples/notebooks/`, executed in CI.
- Inline diagram rendering. Spike: (a) run GRAFED headless (Qt `offscreen` platform) to export
  images, or (b) a pure-Python SVG renderer reading the JSON exported by `visualization.export_json`.
  Decision criteria: no hard dependency on Qt in the wheel, deterministic output, acceptable layout
  quality. Result: `Amplitude._repr_svg_()` or `visualization.render(obj)`; GRAFED stays the
  interactive editor.

### WP8.3 `doctor` command
- `python -m pymarty doctor` reports: pymarty version and MARTY version, Python version, compiler found
  (path, version, can build a trivial library), `make`, LoopTools library present, quad precision
  availability, GRAFED found, LaTeX tools found, writable cache dir. Exit code non-zero when something
  required for a requested feature is missing. Output is plain text and a JSON option.

### WP8.4 Wheels and release
- cibuildwheel matrix: CPython 3.9 to 3.13 (proposal; confirm against the project policy), Linux x86_64
  (manylinux_2_28). aarch64 is evaluated separately, not promised.
- `auditwheel repair` result checked: bundled libraries listed in a manifest file in the wheel.
- License compliance: GPL v3 text in the wheel, notices for bundled third-party libraries (GSL,
  LoopTools, libgfortran runtime), a pointer to the corresponding source (tagged release and the build
  recipe). The wheel build is reproducible from the tag.
- Check name availability on PyPI and the publication workflow (trusted publishing, test index first).
- Source distribution (`sdist`) builds with CMake and compilers on a user machine (documented, tested in CI
  for one job).

### WP8.5 Concurrency and interruption (decision and implementation if accepted)
- Policy: the API is single threaded. A process-wide recursive mutex in the bindings makes concurrent
  calls from several Python threads safe (they serialise).
- Optionally release the GIL during long C++ calls (amplitudes, builds), safe because no Python
  callbacks exist and the mutex serialises access.
- Cooperative cancellation for long computations (periodic check of a flag set by a signal handler that
  turns `KeyboardInterrupt` into an `Interrupted` error and leaves state consistent through the
  `Session` guard). Accepted only if tests show state stays consistent after interruption.

### WP8.6 Versioning and deprecation
- `pymarty.__version__` equals the MARTY version, with a build metadata suffix for pymarty-only patches
  if needed. Semantic-versioning policy for the Python API; a deprecation helper that emits
  `DeprecationWarning` with the removal version.

## 3. Test plan

- L6 wheel tests: for every Python version in the matrix, install in a clean virtual environment (no
  compiler for the "no toolchain" job, with compiler for the "toolchain" job), run the fast test subset
  and the doctests. The toolchain job also builds and evaluates a library (Phase 6 test).
- `doctor` tests: output with all tools present, with `PATH` stripped of the compiler, with
  `MARTY_GRAFED` pointing to a fake script; exit codes.
- Notebook execution of all examples (marker `example`), compared with stored outputs where
  deterministic.
- Diagram rendering tests: for a set of golden diagrams the SVG is byte-stable (or compared by a
  structural check) and contains the expected labels.
- Concurrency tests (if WP8.5 accepted): two threads building different models; interleaved calls;
  `KeyboardInterrupt` simulated with a signal during a long computation, followed by a normal
  computation giving the right result.
- Stub check, type check, documentation build, link check.
- Full nightly suite (L5) on the release candidate wheel (not on a source build).

## 4. Acceptance criteria

1. Wheels build for all matrix entries, pass `auditwheel show`, and the wheel test jobs pass in both
   configurations (with and without a compiler: features needing a compiler fail with a clear
   `BuildError`, everything else works).
2. The nightly suite (L5) passes on the release-candidate wheel for all ports accepted in Phase 7.
3. Stubs: `mypy --strict` clean on the package and on a sample user script exercising every module;
   no `Any` leaks in public signatures other than documented duck-typed protocols.
4. Documentation: every public function and class has a docstring with an executed example (doctest
   coverage report 100% of public API, exceptions listed with reasons); the documentation builds
   without warnings; links valid.
5. `doctor` reports correctly in the four scenarios of the test plan.
6. Inline diagram rendering decision made and recorded; the implemented path passes the golden-diagram tests,
   or the feature is explicitly listed as not included in this release.
7. License and notice files present and verified by a CI script that compares bundled libraries
   against the manifest.
8. Release rehearsal: publication to the test index and install from there in a clean environment.
9. Security checklist of [testing-strategy.md](testing-strategy.md) section 7 reviewed for the whole
   package (a dedicated review PR), with no open findings.
10. Known limitations are listed in the documentation (single threaded, in-process loading risk, GRAFED
    optional, platforms).

## 5. Exit artifacts

Release notes, documentation site, wheels, release rehearsal log, port coverage table showing which
C++ examples and tests have Python equivalents (target: the full list of the README phase table),
and the plan for the next iterations (remaining ports, extra platforms, nanobind evaluation, numpy
optional extra).

## 6. Risks and open questions

- Python version matrix versus binary size and CI time.
- Whether GRAFED can render offscreen in CI, or the pure-Python renderer is worth the maintenance.
- Cooperative cancellation touches core algorithms (checks inside long loops): only done if it
  stays small and safe.
- PyPI name availability and project ownership.
