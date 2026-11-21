# Testing strategy and global acceptance rules

Principle: **C++ API tests are the specification. Python tests prove the binding is faithful and
usable.** The Python layer carries no logic, so Python tests are not allowed to be the only place a
behaviour is tested.

## 1. Test levels

| Level | Where | Tool | CTest label / marker | What it proves | Runs |
|-------|-------|------|----------------------|----------------|------|
| L0 Existing suites | `tests/units`, `tests/integration`, `tests/system` | gtest, system programs | `units`, `integration`, `system` | No regression of MARTY itself | every PR (units, integration). `system`: manual, informational, never gating, see section 1.1 |
| L1 C++ API tests | `tests/api/<module>/` (flat `tests/api/src/` in Phase 0, which has no module) | gtest, new `ADD_API_TEST` macro (same pattern as `ADD_INTEGRATION_TEST`) | `api` | The C++ API contract, errors, state neutrality, lifetime | every PR |
| L2 Python unit tests | `python/tests/<module>/` | pytest | `python` (ctest) / marker `unit` | Binding fidelity: coercion, duck typing, operators, iteration, repr, exceptions, lifetime | every PR |
| L3 Parity | `python/tools/check_parity.py` | script | part of `python` | Every L1 test has an L2 counterpart (or a documented exemption) and conversely | every PR |
| L4 Examples | `python/examples/*.py` | pytest runs them | marker `example` | Python reproduces `examples/*.cpp` (output compared with the C++ example where deterministic) | every PR (fast ones); slow ones manual only |
| L4b Usage examples | `python/examples/usage/<module>.py` | pytest runs them (`python/tests/test_usage_examples.py`) | marker `usage_example` | One short, human-readable example per module (`errors`, `symbolic`, `group_theory`, `diracology`, `modeling`, `calculation`, `simulation`) that fixes the agreed Python API; they must run, they are not about coverage | every PR |
| L5 System ports | `python/tests/system/` | pytest | markers `slow`, `loops`, `needs_compiler`, `quad` | Python reproduces `tests/system/src/*.cpp` and matches the reference files in `tests/system/data/output/` | manual only, see section 1.1 |
| L6 Wheel tests | CI job | cibuildwheel test command | marker `wheel` | Wheel installs in a clean environment and works | every PR touching packaging, release |
| L7 Quality gates | CI | compilers, sanitizers, mypy, ruff, coverage | n/a | See section 5 | every PR |

Every phase document lists exactly which tests belong to which level.

### 1.1 System tests are manual and never an acceptance criterion

The C++ `system` suite (`ctest -L system`) and the Python L5 ports (marker `slow`) take a long time.
They are run **only by the maintainer, by hand, from time to time**, and **they are never part of a
phase's acceptance criteria**. The acceptance gate for regressions is `ctest -L 'units|integration'`
(plus `api` and `python` for the new code). No phase, agent, scheduled job or PR workflow runs the
system tests automatically. In practice:

- Agents and contributors never run `ctest -L system` (nor an unfiltered `ctest`, nor `pytest` without
  `-m "not slow"`).
- When a change could affect a system test (core change, generated-library change, `drawer.cpp`), the PR
  description lists the exact test names (`ctest -R <name>`) as information for the maintainer, who runs
  them when convenient and reports. A failure found that way is a bug report, not a failed acceptance.
- A single named system test may be run by an author for debugging (`ctest -R <name>`), never the label.
- Python ports of system tests that are slow are marked `slow`, developed against small or reduced
  variants where possible, and tracked in the port coverage table with their status. Fast ports (not
  `slow`) run in the PR like any other Python test.
- The GitHub job `manual-system` in `.github/workflows/python.yml` is `workflow_dispatch` only (manual
  trigger), not scheduled.

### 1.2 Usage examples are reviewed before the implementation

Each module has one usage example, `python/examples/usage/<module>.py`, written for a human reader:
short, linear, commented, printing what it shows, using the real public API and nothing private.
It is the first deliverable of the module's phase, not the last:

1. The example is written first, together with the module's Python name table.
2. A human reviews it and agrees on the Python API it shows (gate G1 of [process/README.md](process/README.md)).
   Implementation (C++ API and bindings)
   starts only after that approval, recorded in the PR or issue (reviewer and date).
3. The example is then made to run. A change of the example after approval needs a new review.
4. `test_usage_examples.py` runs every file of `python/examples/usage/` and fails when a module present
   in the package has no example. The test is about keeping the example true, not about coverage:
   assertions in the example are there for the reader.
5. An approved example whose module is not in the package yet is skipped by the test, with a clear
   reason, until the implementation reaches it.
6. Examples are shipped in the source distribution; they are not part of the wheel.

## 2. Parity rule

- Each C++ API test `TEST(<Module>Api, <Behaviour>)` has a Python test marked
  `@pytest.mark.parity("<Module>Api.<Behaviour>")`.
- Legacy tests that serve as API tests (for example `tests/integration/src/gammaAPI.cpp`) are listed in
  `python/tests/parity_sources.txt` so they are scanned too.
- A C++ test that deliberately has no Python counterpart (for example C++ pointer semantics) is listed
  in `python/tests/parity_exemptions.txt` with a reason. Exemptions are reviewed like code.
- `check_parity.py` fails when: a C++ API test has no counterpart and no exemption; a parity marker
  references a C++ test that does not exist; an exemption references a test that does not exist.
- A Python test may be richer than its C++ counterpart (duck-typing variants, parametrised inputs). It
  must not contradict it.
- Rollout: the checker may land at the end of Phase 0 or with Phase 1, but is mandatory as soon as two
  modules have tests. Until then, parity is checked by review.

## 3. Shared data (golden files)

- Location: `tests/api/data/<module>/*.json` (or `.txt` for plain tables).
- Produced by the C++ side: a C++ API test writes the file when `MARTY_UPDATE_GOLDEN=1` is set, and
  compares otherwise. Python tests only read them.
- Golden data is never regenerated automatically. A change needs a justification in the commit
  message (physics or intended behaviour change), like the reference data in
  `tests/system/data/output/` (rule from `AGENTS.md`).
- Physics numbers that come from literature or textbook identities (for example SU(3) products) are
  written in the tests as literal values with a reference comment; they are not derived by running
  the code under test.
- Values quoted in the phase documents as "taken from C++" must be read from the existing C++ tests
  or output, never invented.

## 4. Markers, fixtures and environment

Markers (registered in `pyproject.toml`, `--strict-markers`): `unit`, `parity(id)`, `example`, `usage_example`,
`slow`, `loops` (needs LoopTools), `needs_compiler`, `quad`, `needs_grafed`, `wheel`.

Default local run: `pytest -m "not slow and not needs_compiler"`. CI per PR: that plus
`needs_compiler` for fast generated-library tests. Everything, including `slow`: maintainer, manually.

Fixtures:
- `clean_state` (autouse): opens a `Session`-like check before each test and asserts after the test
  that `Model::current`, all option flags and the symbolic-4D flag are back to their initial values.
  A test that leaks state fails.
- `workdir`: each test that writes files runs in its own temporary directory (`tmp_path`).
- `built_library` (session scoped, cached by content hash): builds a generated library once per run
  and shares it between tests, to keep compile time down.
- `fake_grafed`: a script on `MARTY_GRAFED` that records its arguments, used instead of the real GUI.
- `skip_if_missing`: skips with a clear message when a compiler, LoopTools or GRAFED is absent, but the
  manual full runs and the wheel CI job fail instead of skipping (a skipped required test is a failure there).

Speed: tests print their wall time; manual full runs record the timings and flag a test that becomes more
than 2x slower than its recorded baseline. Baselines for C++ system tests are read from CTest's
`build/Testing/Temporary/LastTest.log`.

Per `AGENTS.md`, system-sized tests are slow (very slow in Debug). Python CI uses a Release build.

## 5. Quality gates (checked in CI for every phase)

| Gate | Rule |
|------|------|
| Warnings | New C++ compiles with `-Wall -Wextra -Wpedantic` without warnings, on all three CI toolchains (GCC 14, GCC 11, Clang 18) |
| Format | `clang-format --dry-run --Werror` on changed C++ files; `ruff format --check` and `ruff check` on Python |
| Types | `mypy --strict` on `python/src/pymarty` and the stubs; stubs are regenerated in CI and must not differ from the committed ones |
| Sanitizers | `ctest -L api` passes under ASan + UBSan (separate CI job, Debug) |
| Coverage | C++ API: line coverage >= 85% of `src/*/api/` through the existing coverage targets extended with the `api` label. Python package: >= 90% (bindings are thin). Thresholds can be revised by decision, not silently |
| Lifetime | Lifetime and leak tests of the phase pass (section 4 of [api-conventions.md](api-conventions.md)) |
| State neutrality | `clean_state` fixture and a C++ equivalent (`Session` tests) pass |
| Doctests | Every docstring example runs (`pytest --doctest-modules`) |
| Independence | `group_theory` and `diracology` import test passes |
| Security review | Checklist in section 7 reviewed in the PR |
| Docs | Every new public function has its C++ doc comment (with Python spelling) and Python docstring |

## 6. Definition of done (all phases)

A phase is accepted when all hold:

1. Every item in the phase's "Acceptance criteria" list is satisfied, with the evidence recorded in the
   PR description (test names, CI run, numbers).
2. L0: `ctest -L 'units|integration'` is green, with no change in reference files. The `system` suite is
   not a criterion (section 1.1); the PR may list system tests worth a manual run.
3. L1, L2, L3 are green for the phase's module and for all earlier modules.
4. The phase's fast examples and ports (L4, and L5 ports not marked `slow`) listed in its document reproduce
   the C++ results within the tolerance used by the C++ test (read from that test, never loosened).
   `slow` ports are informational (section 1.1).
5. All quality gates in section 5 pass.
6. The "Exit artifacts" of the phase exist (tests, stubs, docstrings, notebook or example, the
   decision records of spikes).
7. No deferred "TODO" in the module's public API without an issue reference and a listed
   `NotImplementedError` with a clear message.

## 7. Security checklist (reviewed per PR)

- No `system()`/`popen()` with composed strings; process launches use argument vectors.
- No predictable temporary file names; use `mkstemp`/`tempfile` with mode 0600 in a private directory.
- Names that become file names, identifiers in generated code or Makefile fragments are validated.
- Generated shared libraries are only loaded from paths built by pymarty in this session or passed
  explicitly by the user; the documentation states that loading executes code.
- No `eval`/`exec` of strings from users; expressions are never parsed from strings by the Python layer.
- Environment variables read by `Config` (`MARTY_*`, `CXX`) are documented, and are only used as
  arguments (never interpolated into a shell command).
- No network access.

## 8. How to run (target commands, to be created in Phase 0)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DMARTY_BUILD_PYTHON=ON
cmake --build build -j
ctest --test-dir build -L api              # C++ API tests (L1)
ctest --test-dir build -L python           # Python tests and parity check (L2, L3)
ctest --test-dir build -L 'units|integration'   # existing fast suites (L0)
pytest python/tests -m "not slow"          # direct pytest use
```
