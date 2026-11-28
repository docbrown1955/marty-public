Status: READY
Updated: 2026-11-21
Author: orchestrator (written from the findings of investigator "inv-core-model"; the subagent could not write files, so the orchestrator re-checked the lines marked [V])

# Investigation: core model API to wrap

## 1. Question

Which C++ core functions must the `pymarty.modeling` facade wrap (model, particles, Lagrangian, breaking, rotations, masses, Feynman rules), and what is their shape (overloads, variadics, templates, initializer lists)?

## 2. Facts

1. [V] `mty::Model` derives from `ModelBuilder`, has `inline static Model *current`, and its constructor is a variadic template `explicit Model(Args &&...args)` that sets `current = this`. `src/marty/core/model.h:63-75`.
2. [V] The destructor sets `Model::current = nullptr` only if it still points to this model. It does not restore a previously current model. `src/marty/core/model.h:80-84`.
3. [V] `ModelData::addGaugedGroup` has two overloads: with a dimension (`group::Type, name, int dim, coupling = nullptr`) and without it (`group::Type, name, coupling = nullptr`, for U(1) and exceptional algebras). `src/marty/core/modelData.h:383-410`.
4. [V] `ModelData::init(bool initGaugeTerms = true)`. `src/marty/core/modelData.h:181`.
5. [V] `ModelData::addFlavorGroup` (two overloads), `addParticle(Particle const&, bool initializeTerms = true)`, `getScalarCoupling(std::string_view) const`. `src/marty/core/modelData.h:422, 433, 471, 811`.
6. [V] `ModelData::addLagrangianTerm(csl::Expr const&, bool addHermitic = false)` and an overload taking `mty::Lagrangian::TermType`. `src/marty/core/modelData.h:1082, 1101`.
7. [V] Masses: `addBosonicMass(FieldType&&, std::string_view | csl::Expr const&)`, `addFermionicMass(...)` (four overloads, with and without a second field), `renameParticle(FieldType&&, std::string_view)`. These are templates on `FieldType`. `src/marty/core/modelData.h:1120-1235`.
8. [V] `ModelBuilder::replace` (several overloads on `csl::Expr`, `csl::Tensor`, and particles), `refresh()`, `breakGaugeSymmetry(std::string const&, ...)` (three overloads). `src/marty/core/modelBuilder.h:140-151, 307, 311-326`.
9. [V] `ModelBuilder::rotateFields` and `birotateFields` have many overloads: vectors of `mty::Particle`, vectors of `std::string`, and `std::initializer_list<std::string>`. Matrices are nested vectors of expressions. `src/marty/core/modelBuilder.h:157-256`. The `initializer_list` overloads must not be exposed; the facade uses the vector overloads (plan section 2 WP4.2).
10. Reported, not re-checked: the variadic `Model` constructor and the `initializer_list` overloads are the only variadic or template-free-violating parts of the public core API that the facade must avoid. Particle creation helpers (`scalarboson_s`, `weylfermion_s`, ...) and `Particle` methods (`setGroupRep`, `setMass`, `getFieldStrength`, `generateSimilar`, `getComplexConjugate`) are in the core headers; their exact lines were not checked by the orchestrator.

## 3. Existing tests and data

See [01-investigation-tests-examples-models.md](01-investigation-tests-examples-models.md) for the test and example inventory.

## 4. Options and trade-offs

- Wrap only the non-template, non-initializer-list overloads (vector of strings, vector of particles, explicit expression matrices). Cost: a few overloads hidden from Python, but the plan forbids variadic and initializer-list signatures in the public facade (plan section 5, criterion 3). **Opinion: recommended.**
- Wrap `addParticle` and friends through a single generic entry point that takes a `ParticleHandle`. Cost: the facade must know particle kinds; the plan already splits them by kind. **Opinion: keep the per-kind split.**

## 5. Questions for the user

Captured in [02-questions.md](02-questions.md).

## 6. Not verified

- Exact line numbers of the particle helper functions and of `Particle` methods (not checked).
- Whether `addTerm` is the core name, or only `addLagrangianTerm` exists (the reports use both names; the header shows `addLagrangianTerm`).
