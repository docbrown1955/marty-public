Status: READY (amended after G1: new `Model(gauge=, flavor=)` interface, re-approval needed)
Updated: 2026-11-21
Author: orchestrator

# Phase 4 (`pymarty.modeling`): spec

Inputs: [02-questions.md](02-questions.md) (answered), the four investigation files, and the
usage example [modeling.py](../../../../python/examples/usage/modeling.py) (draft, to be approved at G1).

## 1. Baseline (before any change)

| Check | Command | Result |
|-------|---------|--------|
| Configure and build | `cmake .. -DMARTY_BUILD_PYTHON=ON -DPython_EXECUTABLE=/tmp/pymarty-venv/bin/python` then `cmake --build . -j` | OK (`/tmp/pymarty-phase4-build.log`) |
| C++ fast suites | `ctest -L 'units|integration'` | 197/197 passed (`/tmp/pymarty-phase4-ctest.log`) |
| Python suite | `PYTHONPATH=/tmp/pymarty-build/python /tmp/pymarty-venv/bin/python -m pytest -c python/pyproject.toml --rootdir python python/tests -m "not slow" -q` (from `/tmp`) | 380 passed in 154.9 s |

The first pytest attempt failed at collection because `PYTHONPATH` was not set to the build tree. It is
not a code issue.

## 2. Decisions (from the Q&A)

| Question | Decision |
|----------|----------|
| Q0 prerequisites | Phase 1 accepted. Phase 3 implemented, acceptance pending. Implementation waits for G1. |
| Q1 built-in models | **C**: `QED` and `SM` only. `THDM`, `MSSM`, `PMSSM` deferred (see [unported-features.md](../unported-features.md)). |
| Q2 THDM variant | Deferred with THDM ("see later"). |
| Q3 particle handles | **A**: shared handles, as in the core. A handle sees later changes made by the model. |
| Q4 spacetime | **A**: `pymarty.modeling.spacetime` is in scope. |
| Q5 construction (review of the G1 draft) | `Model(gauge=[...], flavor=[...])`. Groups come from `pymarty.group_theory` (`SU(2)`, `U1()`, ...). `GaugeGroup(group, name, boson_name=None, coupling=None)` and `FlavorGroup(name=, dim=)`, keyword style. The constructor declares the groups, calls `init`, and renames the bosons. No `add_gauge_group`, `init` or `rename_particle` in the public API for this step. |

Core ordering (`src/marty/core/modelData.cpp`): `addGaugedGroup` and `addFlavorGroup` assert `particles.empty()`;
`init()` runs once (`gaugeLocked`), creates the bosons and ghosts as particles, and attaches the flavor groups.
Bosons get default names (`A_<name>`), so `boson_name` is applied after `init`. The facade does all three steps in
the constructor, so user code can not get the order wrong.

## 3. Scope

In scope: `Model` (groups, `init`, `refresh`, printing), particle factories (scalar, Dirac, Majorana, Weyl,
vector), representations (Dynkin labels, rational charges), flavour groups, `add_particle`, `particle`,
`particles`, `rename_particle`, `add_term`, `replace`, `break_gauge_symmetry`, `rotate_fields`,
`birotate_fields`, `add_bosonic_mass`, `add_fermionic_mass`, `coupling`, Feynman rules (lazy, iterable, LaTeX),
`spacetime`, built-in `models.QED()` and `models.SM()`.

Out of scope: THDM, MSSM, PMSSM, GTHDM, NMFV, QCD (Q1); amplitudes and diagrams (phase 5); libraries (phase 6).

## 4. C++ facade (`src/marty/api/modeling/`)

The facade wraps the core and never exposes variadic, template or `initializer_list` signatures (plan
criterion 3). It uses the vector overloads of `rotateFields` and `birotateFields` and the
non-template overloads of `addGaugedGroup`.

| Header | Contents |
|--------|----------|
| `modelingTypes.h` | `enum class GroupKind {U1, SU, SO, Sp, ...}` (only kinds the core supports), `enum class ModelKind {QED, SM}`, `Chirality` reuse |
| `modelHandle.h` | `ModelHandle`: owns `shared_ptr<mty::Model>`, wraps every call in `mty::api::Session`. Constructor takes a list of gauge group descriptors and a list of flavor group descriptors, declares them, calls `init`, then renames the bosons. Exposes `refresh`, `add_particle`, `particle`, `add_term`, `replace`, `break_gauge_symmetry`, `rotate_fields`, `birotate_fields`, `add_bosonic_mass`, `add_fermionic_mass`, `coupling`. `rename_particle` stays internal (used for bosons, and exposed as a method for particles) |
| `particleHandle.h` | `ParticleHandle`: holds the `Particle` (`shared_ptr`) and its `ModelHandle` (keeps the model alive); `set_rep`, `set_mass`, `set_flavor_rep`, `set_self_conjugate`, `similar(name)`, `field_strength` |
| `spacetime.h` | Minkowski and Dirac index factories and `gamma`, `P_L`, `P_R` (built on the `csl` tensor API, Phase 1) |
| `feynmanRules.h` | `feynman_rules(model)`: immutable vector of rule handles, computed on first use under a `Session`; LaTeX output |
| `models.h` | `make_model(ModelKind)`: `QED_Model`, `SM_Model` construction, each in a `Session` |

Rules:
- Every API call runs under `mty::api::Session` (`src/marty/api/common/session.h`), which saves and restores
  `Model::current` and the option flags (investigation `01-investigation-model-current-audit.md`).
- The `Model` constructor (variadic template, `src/marty/core/model.h:63-75`) is only called inside a `Session`.
- Order checks (groups before `init`, `init` before particles, `break` after particles) raise `StateError`
  before the core is called.
- A particle from model A passed to model B raises `ValueError`.

## 5. Python package (`python/pymarty/modeling/`)

- `__init__.py` exports `Model`, `Particle`, `spacetime`, `feynman_rules`; `python/pymarty/models.py` exports
  `QED()` and `SM()`. Add `modeling` and `models` to `python/pymarty/__init__.py:14-17`.
- Binding: `python/src/modeling.cpp` (pybind11), registered from `python/src/module.cpp` and added to
  `python/CMakeLists.txt`. Exception translation reuses `module.cpp:296`. Cout redirection reuses `module.cpp:315`.
- Stubs `_pymarty.pyi` and `modeling.pyi` updated with the binding (must agree with the `.py` wrappers).
- `python/tests/parity_exemptions.txt`: C++ API tests with no Python counterpart are listed with reasons.
- Python names follow the table in [phase-4-modeling.md](../phase-4-modeling.md) section 3.

## 6. Work packages (in order)

| WP | Work | Verification |
|----|------|--------------|
| WP4.0 | Plan amendment and Q&A record (done in this step) | Docs review |
| WP4.1 | Spike: `Session` wrap for particle factories and `Model` ctor; confirm `Model::current` restored after each call | `ctest -R` on a new unit test in `tests/units/` (Model::current unchanged) |
| WP4.2 | C++ facade headers and `.cpp` for model, particle, spacetime, rules | `ctest -L 'units|integration'` plus new `tests/api/modeling/` targets |
| WP4.3 | `make_model(ModelKind)` for QED and SM | Scenario test: QED and SM rule counts and particle names equal the core (`tests/integration/src/test_feynman_rules_latex.cpp` cases reused) |
| WP4.4 | Feynman-rule facade and LaTeX output | `LatexMatchesExistingTests` against the core output |
| WP4.5 | Bindings, package, stubs, docstrings, parity exemptions | `ctest -L python` |
| WP4.6 | Ports: `python/examples/usage/modeling.py` (runs in `test_usage_examples.py`), L4 ports of `gettingStarted` (first half), `simpleSpectrum` (model part), `sampleSMFeynmanRulesLatex` | `pytest -m "not slow"` on the new tests |

WP4.2 to WP4.6 start only after G1 is approved.

## 7. Acceptance criteria (each mapped to a command)

| # | Criterion (from plan section 5) | Check |
|---|--------------------------------|-------|
| 1 | API and core printouts identical for getting-started, QED, SM | `pytest python/tests/modeling -k scenario` |
| 2 | Every method in section 1 has a documented Python spelling and a binding | Script listing C++ methods vs bindings (added in WP4.5) |
| 3 | No variadic, template or `initializer_list` in public facade headers | `grep -nE 'template|initializer_list|\.\.\.' src/marty/api/modeling/*.h` returns nothing |
| 4 | `Model::current` unchanged after each call; two models in any interleaving; particles outlive models | `ctest -R modeling_session` and `pytest -k lifetime` |
| 5 | Typed errors in C++ and Python, nothing printed to stderr | `pytest -k errors` |
| 6 | Ported examples match C++ output; test cases classified | Ported tests pass; classification table in `04-implementation-log.md` |
| 7 | Model-building examples within 1.5x C++ time plus 1 s (Release) | Timed run recorded in the log |
| 8 | `ctest -L 'units|integration'` green | Command run and recorded |
| 9 | Parity, stubs, doctests, sanitizers, quality gates pass | `ruff check`, `ruff format --check`, `mypy --strict pymarty tests tools`, `clang-format --dry-run --Werror`, parity script |
| 10 | `modeling.py` approved by a human before implementation (reviewer and date in PR) | G1 approval recorded below |

Never run `ctest -L system` or an unfiltered `ctest`. Use `pytest -m "not slow"`.

## 8. Risks

- `Model::current` is set by the constructor and not restored by the destructor (`src/marty/core/model.h:80-84`).
  Mitigation: `Session` around every call (WP4.1 test first).
- Core helpers use `Model::current` implicitly. A missed `Session` causes wrong-model behaviour. Mitigation: a
  test that builds two models in interleaved order.
- `initializer_list` and variadic core overloads must stay hidden (criterion 3).
- Particle handles share state with the core: a later `rotate_fields` is visible through earlier handles. The
  behaviour is documented and tested, not hidden.
- Some core operations need a specific order. The facade checks the order and raises `StateError`.
- Printing large models is slow. `repr` stays short, `__str__` is full.
- The usage example cannot run before implementation. It is approved at G1 and run in `test_usage_examples.py`
  once WP4.5 lands.

## 9. Gate G1

Approval needed on: section 2 decisions, section 3 scope, section 4 facade shape, and
`python/examples/usage/modeling.py`. No implementation starts before approval.

G1 approval: **approved** by the user, 2026-11-21 (no changes requested). Implementation (WP4.1 to WP4.6) starts now.
