# Remaining work inventory (after phase 7)

Status: investigation, 2026-11-28. Read-only. Each claim was checked in the repository on this date
unless marked "not verified". Commit reference: `87e0a6d0` (phase 7 remaining-work log).

Status legend: **absent** (nothing in the repo), **partial**, **done** (verified present), **stale** (the
document is out of date), **unverified** (cited but not re-checked).

## A. Layout (USER_FEEDBACK "Repository layout")

| Item | Current state (verified) | Target (USER_FEEDBACK) | Status |
|------|--------------------------|------------------------|--------|
| Python package | `python/pymarty/` (`__init__.py`, `config.py`, `diracology.py`, `group_theory.py`, `symbolic.py`, `symbolic_algorithms.py`, packages `calculation/`, `modeling/`, `simulation/`, `testing/`, `visualization/`) | `src/pymarty/` with a `<module>/` package and `_<module>_.py` each | absent |
| Mixed shapes | three modules are single files, five are packages (above). The target wants a package for every module. | one shape for all modules | absent |
| Python tests | `python/tests/` (subfolders per module, `system/`, `conftest.py`, `test_usage_examples.py`) | `tests/python/` | absent |
| C++ tests | `tests/` has `api`, `integration`, `system`, `units` directly | `tests/cpp/` | absent |
| Python usage examples | `python/examples/usage/` (7 files: `calculation`, `diracology`, `errors`, `group_theory`, `modeling`, `simulation`, `symbolic`) plus `python/examples/group_theory_basics.py` | `usage_examples/python/` | absent |
| Notebooks | `python/notebooks/gthdm_investigation.ipynb` (only notebook) | `usage_examples/python/` | absent |
| C++ usage examples | `examples/` (8 `.cpp` files, `examples/CMakeLists.txt`) | `usage_examples/cpp/` | absent |
| Python built-in models | `python/pymarty/modeling/models/` (`sm`, `qed`, `qcd`, `thdm`, `gthdm`, `sm_2idm`, `sm_triplet`, `gg_to_tt`, `zmumu`, `simple_spectrum`, `yukawas`, `_symbols`) | `builtin_models/python/` (name to decide) | absent |
| C++ built-in models | C++ models live in `examples/*.cpp` and `tests/system/src/*.cpp`; there is no single models folder (not verified in depth) | `builtin_models/cpp/` | absent |
| `dep/` rename | `dep/` (`dep/CMakeLists.txt`, `dep/LoopTools-2.16`); `CMakeLists.txt:111` `add_subdirectory(dep)`; `AGENTS.md:23` and `AGENTS.md:62` reference `dep/` | `third-party/` | absent |
| Docs that name paths | `AGENTS.md` (layout table, forbidden paths), `docs/architecture/pymarty/*.md` (`python/tests/...`, `python/examples/usage/...`, `src/csl/api`) | updated with the move | absent |

Consequence: every path cited in the pymarty plan documents changes with F1. The plan documents should be
updated in the same PR.

## B. Usage examples (USER_FEEDBACK "Usage examples")

| Item | Evidence | Status |
|------|----------|--------|
| Seven examples, one per module, run by `python/tests/test_usage_examples.py` | `python/tests/test_usage_examples.py:16` (`EXAMPLES` path), `:34` and `:43` (`usage_example` marker) | done |
| C++ examples are not run by any test | `examples/CMakeLists.txt` has no `add_test`; grep of `tests/**/CMakeLists.txt` for `examples` finds nothing | absent (contradicts USER_FEEDBACK "all usage examples ... run as part of some test suite") |
| Notebooks are not executed | no `nbclient`, `nbexec` or `ipynb` reference in `python/` or `tests/` tooling (grep) | absent |
| Fewer, richer examples; print more, assert less; latex and diagrams | not started; USER_FEEDBACK proposes 5 examples (symbolic, diracology, group_theory, gettingStarted, complex SM) | absent |
| Phase 8 criterion 11 (seven examples, linked from a getting-started page) | the getting-started page does not exist (see section D) | absent |

## C. API design (USER_FEEDBACK "API design")

| Item | Evidence | Status |
|------|----------|--------|
| Namespace classes that USER_FEEDBACK asks to remove | stub declares `class group_theory:` (`python/pymarty/_pymarty.pyi:362`), `class modeling:` (`:435`), `class calculation:` (`:703`), `class simulation:` (`:812`), `class diracology:` (`:846`) | absent (removal not done) |
| Stubs are the only public typing for the `_pymarty` extension | `python/pymarty/_pymarty.pyi`, `symbolic.pyi`, `diracology.pyi`, `group_theory.pyi` | partial |
| `mypy --strict` gate | phase 7 spec reports 74 pre-existing errors in files outside phase 7 (not re-run here) | partial, unverified count |
| Duplication, C++ facade vs Python layer | python-porting-rules say the layers are thin; no duplication audit found | absent |
| Error and output consistency | `python/pymarty/errors.py` exists; `Session` guard in `src/marty/api/common/session.h` (cited in phase 7 summary) | partial |
| Carried over from phase 4: the Python builders duplicate the C++ models, not reused | `docs/architecture/pymarty/phase-4/04-implementation-log.md:120` | partial |
| Known phase 4 open item: SIGSEGV in `csl::Index::getName()` (`test_parity.py` excluded) | `phase-4/04-implementation-log.md:221` | unverified (carried forward) |
| Known phase 4 difference: Z/A kinetic simplification not done | `phase-4/04-implementation-log.md:196` | unverified (carried forward) |

## D. Documentation (USER_FEEDBACK "Doc", phase 8 WP8.1)

| Item | Evidence | Status |
|------|----------|--------|
| User-facing documentation site | none found (only the planning docs under `docs/architecture/pymarty/`) | absent |
| Developer doc on remaining gaps | this folder, [01-inventory.md](./01-inventory.md) section E | partial (this work) |
| Stubs generated from pybind11 with CI diff check | `.github/workflows/python.yml` and `c-cpp.yml` exist; their content was not reviewed | unverified |
| Docstring examples executed | `python/tests/symbolic/test_docstrings.py` is referenced by `AGENTS.md`; coverage of the public API not measured | partial |
| Migration notes C++ to pymarty (name tables) | the name tables are in each `phase-N-*.md` (`port-table.md` in phases 1 to 3); no single reference | absent |
| Known limitations list | not written | absent |

## E. Remaining feature gaps (from phases 4 to 7)

### E1. Phase 7 remaining work (from `phase-7/03-spec.md`, "Remaining work")

| # | Item | Verified state | Status |
|---|------|----------------|--------|
| 1 | Ports not yet in `python/tests/system/`: `Bgenesis`, `BsMixingSM`, `BsMixingVLQ_5_6`, `_7_8`, `_9_10`, `_10_11`, `FItests`, `freeQuarkAmplitudes`, `gm2SM`, `wilsons_GTHDM` | `python/tests/system/` lists only `test_c7_sm`, `test_c8_sm`, `test_c9_sm`, `test_ewgauge`, `test_gg_to_tt`, `test_model_width`, `test_simple_spectrum`, `test_sm_2idm`, `test_sm_triplet`, `test_widths_sm`, `test_zmumu` | absent (10 ports) |
| 1b | `test_lha_reader` status "unverified" in the spec | `python/tests/simulation/test_lha_reader.py` exists, `read_lha` at `python/pymarty/simulation/lha.py:10` | done (spec is stale on this point) |
| 2 | Wilson coefficient API refactor | `docs/architecture/todo/wilson-coefficient-api.md`, status "not started" | absent |
| 3 | `keepOnlyFirstMassInLoop` as a global option: challenge its use and exposure | not started | absent (not verified) |
| 4 | Re-run the C9_SM slow test after the last edits | `python/tests/system/test_c9_sm.py` exists; no run recorded here | unverified |
| 5 | Mypy gate: 74 errors in untouched files | see section C | partial |
| 6 | Notebooks and C9 helpers: shared symbols must come from `python/pymarty/modeling/models/_symbols.py` | `_symbols.py` exists; compliance not checked | unverified |
| 7 | CKM matrix constant regenerated on each call | not checked | unverified |

### E2. Stale notes to correct

- [insertion-ordering.md](../../todo/insertion-ordering.md) says "implemented in the working tree, not
  committed" and "C9_SM is not yet fixed". Git history contains `96834ab1 Simplify insertion ordering and keep
  user fermion order` and `5e5df91b ... fix C9_SM port`. The note is **stale**. Update or delete it in F1.
- Phase 5 "Blocked on missing Python APIs" list
  ([phase-5-calculation.md](../phase-5-calculation.md), section "Findings and remaining work"):
  `simpleSpectrum` and `testModelWidth` are ported (`python/tests/system/test_simple_spectrum.py`,
  `test_model_width.py`). `FItests`, `BsMixing*`, `wilsons_GTHDM` and `gm2SM` are still open. The list is **stale**.
- Phase 7 summary says `test_lha_reader` has no port (spec item 1, "unverified"). It exists; see E1 row 1b.

### E3. Deferred from earlier phases

| Item | Source | Status |
|------|--------|--------|
| MSSM, PMSSM, NMFV Python models removed from phase 4 (partial ports) | `phase-4/04-implementation-log.md:203-209` | absent; blocked by model serialization (see E4) |
| `C7_PMSSM`, `C8_PMSSM`, `C9_MSSM`, `gm2PMSSM`, `test_fullMSSM` not ported | `phase-7/03-spec.md` "Postponed"; `phase-7/01-investigation-summary.md` section 4 | absent. `test_fullMSSM` has no reference comparison and is not in the CTest list (`tests/system/CMakeLists.txt:32-57`) |
| `FItests`: needs vector integrals and Feynman integral reduction API | `phase-5-calculation.md` "Blocked" list | absent |
| `gm2SM`: needs `isInLoop` predicate filter | `phase-5-calculation.md` "Blocked" list | absent |
| `add_amplitude` asserts non-empty; C++ skips empty amplitudes | `phase-5-calculation.md` "Open points" | unverified |
| C7 and C8 Wilson coefficients evaluate to `0` in Python (cause not established) | `phase-5-calculation.md` "Open points" | unverified (C7/C8 ports now exist; the cause is not recorded as closed) |
| Boltzmann-solver plugin | `phase-7/02-questions.md` Q2 (deferred, layering conflict with GRAFED `jsonLoader.h`) | absent, decision needed (see questions) |
| Quad precision in C++ CTest path | `phase-7/01-investigation-summary.md` section 5 | partial |
| Phase 6 open point: naming of explicit-parameter C functions (`c_<name>_explicit`) | `phase-6/03-spec.md:123` | unverified |

### E4. Blocked by design work

| Item | Source | Status |
|------|--------|--------|
| Model (de)serialisation (required for long models such as `pmssm_lem`) | [model-serialization.md](../../todo/model-serialization.md) | absent, not started |
| Wilson coefficient API refactor (`decomposeOnOperatorBasis`, `getOperatorCoefficient`) | [wilson-coefficient-api.md](../../todo/wilson-coefficient-api.md) | absent, not started |

## F. Phase 8 work packages (phase-8-polish-release.md) against the repo

| WP | Content | Status | Notes |
|----|---------|--------|-------|
| 8.1 | Stubs (CI regeneration check), documentation site, tutorials, migration notes | absent (stubs partial, see C) | the tutorial list (gettingStarted, QED, group theory, gamma, library evaluation) needs F2 examples first |
| 8.2 | Notebooks under `python/examples/notebooks/` (executed in CI); inline diagrams (`_repr_svg_` or `visualization.render`) | absent | `_repr_svg_` not found (grep). Notebooks go to `usage_examples/python/` after F1. `_repr_latex_` exists (`python/src/symbolic.cpp:535`, `python/src/diracology.cpp:171`) |
| 8.3 | `python -m pymarty doctor` with text and JSON output and exit codes | absent | no `doctor` string in `python/` (grep) |
| 8.4 | Wheels (cibuildwheel, manylinux_2_28, CPython matrix), `auditwheel`, licences and notices, PyPI name, sdist | absent | `python/pyproject.toml:9` `requires-python = ">=3.10"` (phase 8 proposes 3.9). `LICENSE` exists at repository root. No bundled-library manifest found |
| 8.5 | Concurrency (recursive mutex, GIL release, cancellation) | absent, optional | no `mutex`, `gil` or `call_guard` in `python/src` (grep) |
| 8.6 | `__version__` and deprecation helper | partial | `__version__` exists (`python/src/module.cpp:241`, `python/pymarty/__init__.py:22`). No `DeprecationWarning` helper (grep) |
| Test plan | wheel tests in clean venv, `doctor` tests, notebook execution, diagram golden files, concurrency tests | absent | |
| Security checklist review | `docs/architecture/pymarty/testing-strategy.md` section 7 | absent | |

## G. Delivery questions (USER_FEEDBACK "Software Delivery")

Open questions from USER_FEEDBACK: wheel or not, third-party inclusion (compiler), Docker, binaries, GitHub
release, what to test in CI, local development path (build folder, venv, typing), and a wrapper on the bare CMake
build. These are decisions. They are listed in [02-questions.md](./02-questions.md).

## H. Summary counts

- Layout items: 10 (all absent).
- Usage examples: 5 items (2 done, 3 absent).
- API design: 8 items (1 partial, 2 unverified carried-forward, the rest absent or partial).
- Phase 8 work packages: 6, plus test plan and security review. Only `__version__` is in place.
- Phase 7 remaining: 10 ports (absent), 2 design refactors (absent), 4 checks (unverified).
- Stale notes to fix: 2 (`insertion-ordering.md`, phase 5 blocked list), plus the phase 7 `test_lha_reader` note.
