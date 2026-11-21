# Phase 4: `pymarty.modeling`

Model building: gauge groups, particles, Lagrangian terms, symmetry breaking, field rotations, masses,
built-in models, and Feynman rules. The C++ source of truth is `mty::ModelData` / `ModelBuilder` /
`Model` (`src/marty/core/modelData.h`, `modelBuilder.h`, `model.h`), `mrtInterface.h` and the built-in
models in `src/marty/models/`.

Prerequisites: Phase 1 (`symbolic`) and Phase 3 (`group_theory`) accepted.

Usage example gate: before any C++ API or binding work starts, `python/examples/usage/modeling.py` is
written and approved by a human reviewer ([testing-strategy.md](testing-strategy.md) section 1.2). It fixes
the Python API of this phase (it should read like `examples/gettingStarted.cpp` up to the Feynman rules) and
is the first deliverable. It may be drafted earlier, in parallel with Phases 1 and 3.

## 1. Scope

In scope:
- `Model` creation with `gauge=` and `flavor=` lists (groups from `pymarty.group_theory`; the constructor runs
  the group declarations, `init` and the boson renames), `refresh`, `__str__` (what `cout << model` prints).
- Particle creation (scalar, Dirac, Majorana, Weyl with chirality, vector, ghost where the core allows),
  properties: mass, representation per group (Dynkin labels, rational charge for U(1)), self
  conjugation, flavour representation, naming, field strength, generating "similar" particles.
- `add_particle`, `get_particle`, `particles`, `rename_particle`.
- Lagrangian: `add_term(expr, add_hermitian_conjugate)`, `replace(old, new)`, `get_lagrangian` (read access
  and LaTeX/str).
- Symmetry breaking and rotations: `break_gauge_symmetry`, `rotate_fields`, `birotate_fields`,
  `add_bosonic_mass`, `add_fermionic_mass`, `get_scalar_coupling`.
- Spacetime objects needed to write terms: Minkowski and Dirac indices, `gamma`, `P_L`, `P_R`
  (`dirac4`), metrics. Exposed as `pymarty.modeling.spacetime`, built on `symbolic.Index/Tensor`.
- Built-in models (phase 4 scope, Q1=C): `models.QED()` and `models.SM()`. `THDM`, `MSSM` and `PMSSM`
  move to a later phase (listed in `unported-features.md`). Other models (GTHDM, NMFV) only if public and tested later.
- Feynman rules: computation, iteration, expressions, LaTeX output (`LatexFeynmanRules`).

Out of scope: amplitudes (Phase 5), diagrams display (Phase 5), libraries (Phase 6), LHA (Phase 7).

## 2. Work packages

- WP4.1 Spike: audit of `Model::current` consumers used during model building and particle creation
  (`scalarboson_s(name, model)` style helpers); define which API functions need a `Session`.
- WP4.2 C++ API `src/marty/api/modeling/`: `ModelHandle`, `ParticleHandle`, `GaugeGroupHandle`,
  `FeynmanRuleHandle`, with explicit non-template methods replacing variadics:
  - `ModelHandle(gauge_groups, flavor_groups)` declares the groups, calls `init`, and renames the bosons;
    `GaugeGroupHandle` / `FlavorGroupHandle` hold the group type, name, dimension and boson name
    (group type and fundamental dimension are read from the `pymarty.group_theory` object)
  - `add_particle(...)` split by kind (`add_scalar`, `add_dirac_fermion`, `add_majorana_fermion`,
    `add_weyl_fermion`, `add_vector_boson`), each returning a handle that holds its model
  - `set_mass`, `set_group_rep` (Rational charges and Dynkin labels), `set_flavor_rep`, `set_self_conjugate`
  - `rotate_fields(vector<string> old, vector<string> new, matrix)` with matrix of `ExprHandle`
    (accepts exact numbers and symbols), the vector overloads of the core mapped
  - `add_mass`, `replace`, `break_gauge_symmetry`, `rename_particle`, `refresh`
  - printing returns strings (`to_string(model)`, `lagrangian_to_latex`, `feynman_rules_to_latex`)
- WP4.3 Built-in models behind a factory `make_model(ModelKind)`.
- WP4.4 Feynman-rule facade: `feynman_rules(model)` returns an immutable vector of `FeynmanRuleHandle`
  (fields, expression, LaTeX), computed on first use (as the core does) under a `Session`.
- WP4.5 Bindings, Python package, examples.

## 3. Python spelling (names, not exhaustive)

| C++ | Python |
|-----|--------|
| `mty::Model m;` + `addGaugedGroup(group::Type::SU, "L", 2)`, `init()`, `renameParticle("A_L", "W")` | `Model(gauge=[GaugeGroup(SU(2), name="L", boson_name="W")], flavor=[FlavorGroup(name="Gen", dim=3)])` |
| `m.addGaugedGroup(group::Type::U1, "Y")` | `GaugeGroup(U(1), name="Y", boson_name="B")` |
| `weylfermion_s("Q_L", m, Chirality::Left)` | `m.weyl_fermion("Q_L", chirality="L")` (returns a `Particle`, not yet added) |
| `Q->setGroupRep("L", 1)`, `setGroupRep("Y", {1, 6})` | `Q.set_rep("L", 1)`, `Q.set_rep("Y", Fraction(1, 6))` |
| `m.addParticle(Q)` | `m.add_particle(Q)` |
| `m.breakGaugeSymmetry("Y")` | `m.break_gauge_symmetry("Y")` |
| `m.renameParticle("A_L", "W")` | `m.rename_particle("A_L", "W")` |
| `W_1->getFieldStrength()`, `W_1->generateSimilar("W")` | `W_1.field_strength`, `W_1.similar("W")` |
| `Index mu = MinkowskiIndex();` | `mu = spacetime.minkowski_index()` |
| `W(mu)`, `W({mu, nu})` | `W(mu)`, `W(mu, nu)` |
| `GetComplexConjugate(x)` | `x.conjugate()` |
| `m.replace(W_1, expr)` | `m.replace(W_1, expr)` |
| `m.rotateFields({"W_3","B"}, {"Z","A"}, {{cW,sW},{-sW,cW}})` | `m.rotate_fields(["W_3","B"], ["Z","A"], [[cW, sW], [-sW, cW]])` |
| `m.addBosonicMass("W", M_W)` | `m.add_bosonic_mass("W", M_W)` |
| `m.addFermionicMass("u_L", "u_R", m_u)` | `m.add_fermionic_mass("u_L", "u_R", m_u)` |
| `m.getScalarCoupling("g_Y")` | `m.coupling("g_Y")` |
| `m.getFeynmanRules()` | `m.feynman_rules` (lazy property, iterable, sized) |
| `LatexFeynmanRules(...)` | `m.feynman_rules.to_latex()` |
| `mty::SM_Model` etc. | `models.SM()`, `models.QED()` (THDM, MSSM, PMSSM in a later phase) |

Duck typing: group and particle arguments accept either handles or names; numbers (masses, charges,
matrix entries) accept anything `symbolic` accepts; rotation matrices are any nested iterable.

Lifetime: `Particle` keeps its `Model` alive (C++ handle holds the model). Using a particle after
`del model` must work. A particle from model A used in model B raises `ValueError`.

## 4. Test plan

L1 (`tests/api/modeling/`):
- Construction and building: `AddGaugeGroupsAndInit`, `UnknownGroupKindFails`, `DuplicateNamesFail`,
  `ParticleKindsCreated` (one per kind), `SetRepCharges`, `InvalidRepRaisesValueError`.
- Gettingstarted-model scenario (`GettingStartedModelBuildsAndPrints`): same steps as
  `examples/gettingStarted.cpp` up to `getFeynmanRules`. The printed model and the number and
  expression strings of the rules are compared to golden data produced by running the equivalent
  direct C++ core calls (the test constructs the same model through the core `Model` directly and
  through the API, and the two outputs must be identical; golden file as second guard).
- Rotations: `RotateFieldsMatchesCore`, `RotationMatrixSizeMismatchFails`, `BirotateFields`.
- Masses: `AddBosonicMass`, `AddFermionicMass`.
- Built-in models: `QedModelHasExpectedParticlesAndRules`, `SmModelParticleList` (rule counts and
  particle name lists from the core).
- Feynman rules: `RulesComputedOnce`, `RuleHandlesImmutable`, `LatexMatchesExistingTests` (reuse the
  cases of `tests/integration/src/test_feynman_rules_latex.cpp`).
- Physics latex and couplings: reuse cases of `test_physics_latex.cpp` and
  `test_specific_gauge_couplings.cpp` (these already test the core; the API tests prove the facade
  reaches the same results).
- Ownership/state: `ParticleOutlivesModelHandle`, `TwoModelsIndependent`,
  `SessionRestoresCurrentModel` (after every method), `NoOptionFlagChanged`.
- Errors: unknown particle/group names, mismatched dimensions, adding a particle twice, using a
  particle in a foreign model.

L2 (`python/tests/modeling/`): parity tests for all L1 cases plus
- duck typing: names versus handles, `Fraction`/int/symbol charges, nested tuple matrices
- `__str__` equals the C++ printing, `_repr_html_`/`_repr_latex_` for notebooks (optional)
- iteration and `len` of particles and rules, `in` tests by name
- lifetime: `del model; gc.collect()` then use particle; the live object counter returns to zero
- `clean_state` after each test
- legacy output redirect: model printing and `refresh` messages appear on `sys.stdout`

L4 examples (Python ports, compared against the C++ example outputs where deterministic):
- `gettingStarted` first half (model, rules), `simpleSpectrum` (model part), `sampleSMFeynmanRulesLatex`
  (the LaTeX file content equals the C++ one). `samplePMSSM` moves with PMSSM to the later phase.

L5: none.

## 5. Acceptance criteria

1. The scenario test in section 4 shows that API and direct core calls produce identical model
   printouts and Feynman-rule expressions (string equality) for: the getting-started model, QED, SM.
2. Every method of the list in section 1 exists in C++ with a documented Python spelling, and in Python
   as a call-through (checked by a script listing each C++ API method and its binding).
3. No variadic, template or `initializer_list` in the public C++ API headers of this module (compile
   test or grep in the review checklist).
4. Lifetime and state: `Model::current` after each API call is unchanged; two models can be built in
   one process in any interleaving (test); particles survive models.
5. Error matrix: every invalid operation in section 4 raises the typed exception with a clear
   message in C++ and in Python, none prints to stderr, none terminates the process.
6. The Python ports of `gettingStarted` (first half), `sampleSMFeynmanRulesLatex` and
   `simpleSpectrum` (model part) run in CI and match the C++ outputs. `test_feynman_rules_latex`,
   `test_physics_latex` and `test_specific_gauge_couplings` cases are each classified (ported or exempt
   with reason).
7. Time budget: the model-building examples run in no more than 1.5x the C++ time plus 1 s (Release).
8. `ctest -L 'units|integration'` green. This phase changes core code only if a spike shows it is needed;
   the PR then lists the `system` tests worth a manual run (informational, testing-strategy.md section 1.1).
9. Parity, stubs, doctests, sanitizers and quality gates pass.
10. `python/examples/usage/modeling.py` was approved by a human before implementation started (reviewer and
    date in the PR), is unchanged since or its changes were re-approved, and runs in
    `test_usage_examples.py` (L4b).

## 6. Exit artifacts

Stubs, docstrings with examples, ported examples, golden data for model printouts and rule listings,
record of the `Model::current` audit.

## 7. Risks and open questions

- Core uses `Model::current` implicitly in particle helpers: all API calls must go through a `Session`.
- Particles are `shared_ptr` objects; core calls such as `replace` or `refresh` mutate them, and
  other Python handles can then observe changes. The facade defines this contract precisely (handles
  refer to the same particle, expressions are immutable) and tests it.
- Printing (`operator<<`) of large models is slow or huge: Python `repr` stays short, `__str__` is
  full.
- Some core operations need a particular order (add groups, `init`, add particles, break). The API
  checks the order and raises `StateError` rather than letting the core assert.
- Built-in model construction time for the deferred PMSSM model may be long: tests marked slow once it is ported.
