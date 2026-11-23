# Phase 5 spec: tree-level calculation and visualization export

Status: APPROVED (G1 accepted by user)
Updated: orchestrator draft, pending G1
Author: orchestrator

Inputs: [phase-5-calculation.md](../phase-5-calculation.md), [01-investigation-summary.md](01-investigation-summary.md),
[02-questions.md](02-questions.md), [usage example](../../../../../python/examples/usage/calculation.py).

## 1. Baseline (recorded before any change)

- Build in `/tmp/pymarty-build`: exit 0.
- `ctest -L 'units|integration' -j12`: 203/203 passed.
- `ctest -L '^(api|python)$' -j12`: 163 api and 3 python tests passed.
- `python/tests/test_usage_examples.py`: 6 passed, 1 skipped (`calculation`, module not implemented).

## 2. Decisions (from the Q&A)

| # | Question | Decision |
|---|----------|----------|
| Q1 | Wilson coefficients | Stay in phase 7 (7a, 7b). Not in phase 5. |
| Q2 | Calculation API location | `pymarty.calculation` functions taking the model explicitly. `modeling.Model` gets no new methods. `Model.finalize()` unchanged. |
| Q3 | Visualization | Full package in phase 5: JSON export, save, GRAFED locator and launcher (WP5.6 to WP5.8). |
| Q4 | Filters | Data filters converted to C++. Python diagram predicates applied to returned diagrams, do not change the expression. Lagrangian-level callbacks unsupported. |
| Q5 | FeynmanRules | Read-only view, plus vertices and propagators as plain data. |
| Q6 | Examples acceptance | QED Compton, e+e- to mu+mu-, sampleSQED, sampleQCDSextet (tree part), Nf flavour tree part, gg_to_tt tree part. One-loop parts go to phase 7. |

## 3. Python API

Package `pymarty.calculation` (new), `pymarty.visualization` (new).

| Name | Kind | Behaviour |
|------|------|-----------|
| `compute_amplitude(model, order, insertions, kinematics=None, options=None)` | function | Requires a finalized model (`MartyStateError` otherwise). `kinematics` is one distinct positive momentum index per insertion (core `Kinematics(insertions, indices)`); `None` keeps indices `1..N`. Explicit momentum tensors are deferred. Returns `Amplitude`. |
| `compute_squared_amplitude(model, amp)` / `compute_squared_amplitude(model, amp_left, amp_right)` | function | Returns the squared expression. |
| `compute_feynman_rules(model)` | function | Read-only `FeynmanRules` view: `len`, `str`, `latex(i)`, vertices and propagators as data. |
| `Order.TREE` | enum | `Order.ONE_LOOP` accepted by the enum, rejected with `MartyNotImplementedError` until phase 7. |
| `incoming(x)`, `outgoing(x)`, `anti(x)`, `left(x)`, `right(x)` | functions | Insertion objects. `x` is a name or `Particle`. Unknown name: `MartyLookupError` with close matches. |
| `FeynOptions(topology=None, filters=(), fermion_order=None, ...)` | class | Plain data. Converted to C++ `FeynOptions` at call time. Unknown topology: `MartyValueError`. |
| `force_particle(name)`, `forbid_particles(*names)`, `disable_particles(*names)` | functions | Data filters, converted to C++ `filter::...`. |
| `select_diagrams(amp, predicate)` | function | Python predicate applied to the diagrams of an `Amplitude`; returns a new list-like view. Expression unchanged. |
| `set_options(**flags)` | context manager | Sets `mty::option::*` flags (snake-case names), restores previous values on exit, also on exception. Unknown name: `MartyValueError` listing valid names. Value type checked. |
| `Amplitude.display()` | method | Returns `str`. |
| `Amplitude.diagrams` | property | Sequence of `Diagram`, read access. |
| `Diagram.particles` | property | Tuple of particle names in the diagram (read access, used by predicates). |
| `visualization.export_json(obj)` | function | Returns JSON `str` for an `Amplitude` or `FeynmanRules`. No GUI, no process. |
| `visualization.save(obj, path)` | function | Writes JSON to `path`, owner-only permissions. |
| `visualization.grafed.is_available()` | function | `bool`. |
| `visualization.grafed.show(obj, wait=False)` | function | Writes to a private temporary directory, launches GRAFED with an argument vector. Raises `MartyStateError` with install instructions if GRAFED is missing. |
| `visualization.show(obj)` | function | Uses `grafed.show` when available. |

Naming rule: every function is a verb (`compute_*`, `set_*`, `force_*`, `forbid_*`, `disable_*`,
`select_*`, `export_*`, `save`, `show`); insertion helpers are the bare nouns `incoming`, `outgoing`,
`anti`, `left`, `right`; classes and enum members are nouns. C++ names map one to one
(`computeAmplitude`, `computeSquaredAmplitude`, `computeFeynmanRules`).

Notes:
- Diagram selection by predicate is a Python callable; no C++ callback is bound.
- All public names have docstrings with `>>>` examples, executed by `test_docstrings.py`.
- Every name used in `python/examples/usage/calculation.py` is in the table above.

## 4. C++ API

New headers under `src/marty/api/`:

- `calculation/calculation.h`
  - `InsertionHandle` (name, direction, antiparticle, chirality).
  - `FeynOptionsData` (plain struct mirroring `FeynOptions`, validated), converted to `mty::FeynOptions`.
  - `AmplitudeHandle` (diagrams, expression, options used), `DiagramHandle` (read access).
  - `computeAmplitude(order, vector<InsertionHandle>, kinematics, FeynOptionsData)`; the variadic and
    initializer-list overloads of `Model::computeAmplitude` stay hidden behind this one function.
  - `computeSquaredAmplitude(amp)` and `computeSquaredAmplitude(ampL, ampR)`.
  - `displayText(amp)`, `abbreviationsText()`.
  - `Session` applies options and restores state, also when the computation throws.
- `calculation/feynmanRules.h` (extends the existing `src/marty/api/modeling/feynmanRules.h`)
  - Vertex and propagator data accessors.
- `calculation/options.h`
  - Table of `mty::option::*` flags from the X-macro in `src/marty/core/mrtOptions.h:91-132` and
    `src/csl/options.h:42-58`, with names and value kinds.
- `visualization/visualization.h`
  - `diagramsJson(AmplitudeHandle)`, `rulesJson(FeynmanRules)` returning `std::string`.
  - `saveDiagrams(obj, path)`, owner-only permissions.
  - `findGrafed(config)` (explicit path, `MARTY_GRAFED`, `PATH`, install prefix).
  - `launchGrafed(jsonPath, config)` using `posix_spawn` with an argument vector, non-blocking, returns
    handle with `pid`, `poll`, `wait`.
- `core/drawer.cpp:162-163`: `Show()` keeps its signature, reimplemented on the functions above. No fixed
  `/tmp/graphs.json`, no `system()`.

Layering: CSL does not include MARTY; the Python binding does not include C++ headers from `core/`
directly (only `api/`).

## 5. Bindings

- New binding sources `python/src/calculation.cpp` and `python/src/visualization.cpp`, registered in
  `python/CMakeLists.txt` and `python/src/module.cpp`.
- Python wrappers `python/pymarty/calculation/` and `python/pymarty/visualization/` with `.pyi` stubs
  (synced with the binding, per AGENTS.md "Stub sync").
- `python/pymarty/__init__.py`: add `calculation` and `visualization` to the listed modules.
- `python/tests/parity_exemptions.txt` lines 11-12 (Feynman rules not exposed): remove the exemption
  for the exposed FeynmanRules view and add parity ids for the new functions.

## 6. Work packages and verification

| WP | Content | Verification |
|----|---------|--------------|
| WP5.0 | Example `calculation.py` (done, lint clean; skipped until module exists) | `ruff check`, `ruff format --check`, `test_usage_examples.py` |
| WP5.1 | C++ calculation API (`api/calculation`), tests on insertions, options, amplitude, squared | `ctest -L units`, `ctest -L api`, `ctest -R calculation` |
| WP5.2 | Global flags table and `set_options(**flags)` | `ctest -R calculation`, python test `test_options.py` |
| WP5.3 | Data filters and `select_diagrams` | python tests `test_filters.py`; api test for filter conversion |
| WP5.3b | `feynman_rules` view and data | `ctest -L api`, python `test_feynman_rules.py` |
| WP5.4 | Kinematics symbols, squared facade | api tests, python `test_squared.py` |
| WP5.5 | Examples port: tree parts only (Q6 list) | `ctest -R` on the examples named in the test plan, one at a time (not `system` suite) |
| WP5.6 | Spike: LaTeX run at export? JSON schema | Written note in `phase-5/`, no code |
| WP5.7 | C++ visualization API; `Show()` reimplemented | `ctest -L api`, unit test that `Show` no longer calls `system` and writes no fixed path |
| WP5.8 | Python `pymarty.visualization` | `test_visualization.py`; `grafed.is_available()` false path tested without GUI |

Standing commands (from AGENTS.md and the process):
- Build: `cmake --build /tmp/pymarty-build --target _pymarty -j`.
- Python: `PYTHONPATH=/tmp/pymarty-build/python /tmp/pymarty-venv/bin/python -m pytest -c python/pyproject.toml --rootdir python python/tests -m "not slow" -q`.
- Gates: `ruff check .`, `ruff format --check .`, `mypy --strict pymarty tests tools` in `python/`; parity check.
- C++: `ctest -L units`, `ctest -L api`. Never `ctest -L system` or unfiltered `ctest`.

## 7. Acceptance criteria

1. `python/examples/usage/calculation.py` runs to the end (no skip), with all asserts passing.
2. Compton (QED, `Order.TREE`, 4 insertions) gives the same number of diagrams as the C++ `gettingStarted`/`sampleQED` run.
3. `modeling.Model` has no new attribute (checked by a test that lists its members before and after).
4. `set_options(...)` restores flags on normal exit and on exception.
5. `visualization.export_json` and `save` write no GUI process, no file in `/tmp/graphs.json`.
6. `Show()` in C++ no longer uses `system()` or a fixed temporary path.
7. Q6 example list (tree parts) runs; one-loop parts are absent and marked for phase 7.
8. All gates above green; parity exemptions updated.

## 8. Risks

- The C++ `Show()` and `saveMultiple` behaviour is partly verified by investigation (JSON only, no LaTeX run). WP5.6 must confirm before WP5.7.
- Filter conversion: the C++ `std::function` filter signatures must be checked when writing WP5.3.
- Examples in Q6 may be slow; run each one alone with `ctest -R`.
- Investigators could not write files (see `phase-5/01-investigation-summary.md`); the orchestrator consolidated them.

## 9. Changes to the plan

`phase-5-calculation.md` was updated to match this spec: `Model.amplitude`/`Model.squared` replaced by
`compute_amplitude`/`compute_squared_amplitude` functions (verb names, per user review), Wilson kept out,
filter callbacks and FeynmanRules data added.

## Implementation deviations

The deviations from this spec (JSON export and `save` for `Amplitude` only, index-only `kinematics=`,
the 0-based fermion order, `compute_squared_amplitude(..., other=None)`, the N_f=2 flavour example
interpretation, and `Zmumu` not ported) are recorded in
[phase-5-calculation.md](../../phase-5-calculation.md), section 8.
