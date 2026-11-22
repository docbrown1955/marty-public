# Phase 4 implementation log

## WP4.6: ports and examples

### Classification of the C++ example sources

| Source | Part | Status | Python counterpart |
|--------|------|--------|--------------------|
| `examples/gettingStarted.cpp` | Model: gauge groups, fermions, representations, breaking, renaming | Ported | `test_getting_started_model_particles_match_cpp` |
| `examples/gettingStarted.cpp` | Rotation to `Z`/`A`, amplitude (6 diagrams), squared amplitude | Ported | `test_getting_started_tree_amplitude_matches_cpp_diagram_count` |
| `examples/gettingStarted.cpp` | `Display`/`Show` (GRAFED), `mty::Library`, `make` builds | Exempt | Not Python-facing: GUI and library generation |
| `tests/system/src/simpleSpectrum.cpp` | Model part: flavour groups, breaking, promotion to Goldstone | Ported | `test_simple_spectrum_model_particles_match_cpp` |
| `tests/system/src/simpleSpectrum.cpp` | Spectrum generation and comparison with `tests/system/data/output/simplespectrum.txt` | Exempt | Not run here: system tests are maintainer-run only |
| `examples/sampleSMFeynmanRulesLatex.cpp` | Standard Model Feynman rules as LaTeX | Ported | `test_sm_latex_document_has_one_entry_per_cpp_feynman_rule` |
| `python/examples/usage/modeling.py` | Usage example | Runs in `test_usage_examples.py` | n/a |

### Golden references

Generated from the C++ examples in the same build (`Release`, GCC 11):

- `gettingStarted` first half prints 12 particles: `B c_B u_R d_R W_1 W_2 W_3 c_W_1 c_W_2 c_W_3 u_L d_L`.
- `sampleSMFeynmanRulesLatex` writes 190 `Feynman vertex for` entries. The Python `latex_document()` also has 190.

The two LaTeX texts differ in size (159787 vs 159570 bytes). Generated index labels (`alpha_*`, `G_*`) differ between runs, so the test compares structure, not bytes.

### Known gaps found while porting

- Resolved: `str(Model)` now uses the C++ `operator<<` (particles with spin, mass, irrep, and the Lagrangian). `Particle.spin` and `Particle.irrep(group)` are exposed.
- Resolved: `Model.rotate_fields` accepts names (C++ string overload, delegating to the core `rotateFields`), so the port no longer needs `Particle.similar` as a workaround.
- Resolved: gauge groups are read from the core model, not kept as facade copies. `Particle.irrep(group)` and `isU1` therefore work on `standardModel()` and `qed()`. Each core gauge group maps to one cached `group_theory.Group` handle (`Group::wrap`, which keeps the model alive), so irreps of one model compare with each other. Irreps compare by algebra (`SemiSimpleAlgebra::operator==`: same type and rank) and highest weight, as in core MARTY. Irreps from a model therefore equal `Group.su(2)` irreps of the same highest weight, and cross-group comparisons return `False` instead of raising. Spin and weak-isospin doublets (`SU(2)_s`, `SU(2)_L`) compare equal, which the core previously did not do.
- The C++ final particle list of `simpleSpectrum` was not captured (the reference run stops on a missing `libooptools.so`). The Python test checks the list that the Python port produces, following the same steps as the C++ file.
- `gettingStarted` library generation fails in this environment (missing `clooptools.h`). It is outside the port scope.

### Full ports (WP4.6, second step)

Facade additions for the ports: `Particle.expr`, `Particle.__call__(*indices)`, `field_strength`, `similar`, `set_self_conjugate`, `set_fundamental_flavor_rep`; `Model.generate_index`, `promote_to_goldstone`, `compute_amplitude`, `compute_squared_amplitude`; `Order`, `Leg`, `Amplitude`; `dirac_index`, `renamed_indices`.

Checks: `pytest -m "not slow"` on `tests/modeling` and `test_usage_examples.py` passes. `ctest -L 'units|integration|api'` passes (361 tests). The C++ `RelabelIndices` test was updated to the approved cycling behaviour: 3 indices over a 2-name pool give IDs `{0, 1}`.

### Irrep comparison, index pool and model recreations

- Irrep equality reuses `SemiSimpleAlgebra::operator==` (type and rank), so `SU(2)_s` equals `SU(2)_L`. Core tests: `tests/units/marty/test_irrep_equality.cpp`.
- `csl::SpaceIndexPool` (`src/csl/space_index_pool.h`) owns the name cycling and the ID counter that `csl::Space` previously held globally. A plain `RelabelIndices(expr)` uses a fresh pool per call, so it starts at name 0 with ID 0. A renaming shared across expressions keeps its pools and keeps cycling. Decision to review: there is no persistent pool on `Space` itself.
- `QCD_Model` triplets used `SU3_c`, which no longer exists after the `GaugeGroupSpec` constructor; they now use the declared group name `C` (`src/marty/models/qcd.cpp`). Without this the C++ QCD model throws at runtime.
- Model recreations live in `python/tests/modeling/cpp_models/` (one `build() -> Model` per model), checked by `test_cpp_*.py`:
  - QED and QCD: complete. Particle names and rule counts match the C++ models (QCD: 8 particles, 5 rules).
  - SM, MSSM, THDM, GTHDM, NMFV, PMSSM: partial. Strict xfails record the missing counts. The C++ references are SM 42 particles / 190 rules, MSSM_HEM 29 / 148, THDM 45 / 349, GTHDM 45 / 413, PMSSM 87 / 3984.
  - `pmssm_lem` is excluded, as agreed.
### Gap closure, round 2

- Flavour symmetry breaking with explicit renaming. Core gains `ModelBuilder::breakFlavorSymmetry(flavor, map)` (`src/marty/core/modelBuilder.cpp`), mirroring the gauge map overload. Unlisted particles keep the default `name_j` names. Python: `Model.break_flavor_symmetry(flavor, renaming="default")`, sharing `parseRenaming` with `break_gauge_symmetry` (`python/src/modeling.cpp`). The SM builder now uses this in place of the C++ `renameParticle` follow-up.
- Ghost bosons: `Particle.ghost_boson()`, `Particle.set_conjugated_ghost_boson()` and `Model.ghost_boson(vector_boson, name, conjugated=False)` wrap the core calls. The default ghosts of gauge bosons already exist in the model (`c_W`, `c_B`).
- Decision: the new Python surface stays a thin facade over core methods. The C++ `mrtInterface.h` free functions were not copied one by one. No interface change was needed in core for this round.
- Decision: the MSSM rule-count check is a strict xfail rather than a pin on the current value.
- Remaining SM ghost steps (`c_Wp`/`c_Wm` on the broken `W`) depend on electroweak breaking, which is not ported yet.
- Bindings still missing for full ports:
  - Model flavour space (`getVectorSpace`) and tensor couplings, so Yukawa and soft-mass tensors cannot be written from Python.
  - Hermitian tensor properties (`addHermitianProperty`).
  - Gauge fixing (`xi`).
  - Mass diagonalisation (`DiagonalizeMassMatrices`, `gatherMass`) and Majorana promotion.
  - `Particle.setMass` and `getEpsilon()`.

Exposed: `Model.covariant_derivative(group, field, mu, indices)` and
`Model.collect_terms()` (C++ `mergeTerms`, renamed only in Python). Calling
`collect_terms` after the GTHDM adjust steps does not split generations 2 and 3.

### Timed run

| Command | Result | Time |
|---------|--------|------|
| `pytest -m "not slow"` on `tests/modeling` and `tests/test_usage_examples.py` | 24 passed | 4.6 s |
| `pytest -m "not slow"` on `tests/modeling/test_ports.py` | 3 passed | 1.7 s |
| `sampleSMFeynmanRulesLatex.x` (C++ reference run) | 190 rules | about 1.7 s |

### One-call model builders

- `Model.add_weyl_fermion`, `add_dirac_fermion`, `add_majorana_fermion` and `add_scalar_boson` create the field and attach it in one call (`addField` in `python/src/modeling.cpp`). Arguments: `gauge_rep` (dict), `flavor_rep` (one flavour-group name), `mass` (`Expr`) and `self_conjugate`. Only the name is required.
- Defaults: scalars are self-conjugate (real) by default, as requested. Dirac and Weyl default to not self-conjugate. Majorana is always self-conjugate, so it has no `self_conjugate` argument.
- Decision: Weyl fermions take no `mass` argument. Use `add_fermionic_mass`.
- Decision: the unattached factories (`weyl_fermion`, `dirac_fermion`, `majorana_fermion`, `scalar_boson`) are retired from the Python surface, because no recreation or test uses them. The C++ facade methods stay, since the builders call them. The `Particle` setters (`set_rep`, `set_self_conjugate`, `set_fundamental_flavor_rep`) stay for unattached particles from `Particle.create_similar_particle`.
- Decision: charged or complex scalars in the recreations pass `self_conjugate=False` explicitly (SM `H`, the MSSM and PMSSM sfermions and Higgs doublets, NMFV `sL_L`, `sE_R`, `sQ_L`, `sU_R`, `sD_R`, `Hu`, `Hd`, and THDM/GTHDM doublets). The `True` default would make these invalid, for example `H` fails the gauge-invariance check.
- `Particle.create_similar_particle` stays unattached. `Model.create_similar_particle(name, reference, *, self_conjugate=None, mass=None)` attaches the result.
- Done: all recreations (`qed`, `qcd`, `sm`, `mssm`, `nmfv`, `pmssm`, `thdm`, `gthdm`), `test_ports.py`, `test_modeling_api.py` and the example use the builders.
- Done: `Particle.is_self_conjugate()` getter (`Particle::isSelfConjugate` in the facade). `test_is_self_conjugate_reflects_builder_flags` checks it.


### Finalization and freezing

- Decision: `Model.finalize()` calls `refresh()` once and then freezes the model. It is idempotent. `Model.is_finalized()` is a method, so Phase 5 can assert it.
- Decision: builders and mutators raise `MartyStateError` on a finalized model (`checkOpen` in `src/marty/api/modeling/modeling.cpp`). Reads stay open: `particle`, `particles`, `feynman_rules`, `compute_amplitude`, `describe`, `ghost_boson` and `generate_index`.
- Decision: the freeze is a facade-level flag (`detail::ModelState::finalized`). The C++ core is unchanged.
- Decision: `standard_model()` and `qed()` return finalized models.
- Decision: `Model.add_tensor_coupling(tensor)` takes a `pymarty.symbolic` tensor directly. No facade tensor wrapper is added.
- Decision: `Model.vector_space(flavor)` exposes the index space of a flavour group, for building Yukawa tensors. `SpaceHandle`'s constructor from a `Space` is now public in `src/csl/api/tensor.h`.
- Not exposed: `gatherMasses`, which `refresh()` already calls.
- Open: the SM, THDM, GTHDM, MSSM, PMSSM and NMFV ports are not done. They need facade primitives for mass-matrix diagonalisation, `set_mass`, scalar-coupling lookup, CKM construction, and Goldstone promotion with gauge fixing.

### Gap closure: primitives, renames and SM port

- Added `Model.diagonalize_mass_matrices()` (facade `diagonalizeMassMatrices`) and `Model.build_ckm(flavor)` (facade `buildCkm`). `build_ckm` returns a `TensorHandle` with spaces `{space, space}`.
- Added `pymarty.symbolic.diagonal_tensor(name, space, entries)`, backed by the C++ helper `diagonalTensor` in `src/csl/api/tensor.cpp`. Decision: diagonal mass matrices use numeric entries, not delta sums. Delta-sum diagonals trigger the "mixings in mass terms" warning in `gatherMass` (`src/marty/core/modelBuilder.cpp`).
- Decision: `new_coupling` is renamed `create_coupling`, and `ghost_boson` is renamed `create_ghost_boson(name, vector_boson, *, conjugated=False)`. Both match the builder naming, with the name first.
- Decision: `Model.refresh()` is removed from the Python surface. `finalize()` runs the refresh internally, so the recreations and tests call `finalize()` at the end of their builders. The C++ facade method stays.
- Decision: `standard_model()`, `qed()` and `renamed_indices()` are module-level functions, not Model static methods.
- Decision: `compute_amplitude`, `compute_squared_amplitude` and `build_ckm` stay on Model, since Phase 5 needs them.
- SM port (`tests/modeling/cpp_models/sm.py`): particle names match `models.SM()`. The Feynman-rule test is still an xfail: 176 rules against 190. The missing parts are W/Z mass setting and gauge fixing, from `src/marty/models/sm.cpp` lines 52-53. This attribution is not confirmed.

### Model API cleanup (supersedes the naming and removal decisions above)

- Removed from Python: `standard_model()`, `qed()`, `renamed_indices()`, `dirac_index()`, `build_ckm`, `compute_amplitude`, `compute_squared_amplitude`, `feynman_rules`, and the `Order`, `Leg`, `Amplitude`, `FeynmanRules` classes. The `pymarty.models` module is deleted. Calculation is Phase 5 scope. The C++ facade keeps these functions for Phase 5. Consumers adapt: `expr.relabel_indices()` replaces `renamed_indices`, and `model.generate_dirac_index()` replaces `dirac_index`.
- Decision: the built-in SM and QED models are gone from Python. Parity tests in `python/tests/modeling/test_cpp_*.py` compare against reference particle names hard-coded from the C++ output (`CPP_SM_PARTICLES`, `CPP_QED_PARTICLES`) instead.
- Naming: `add_*` modifies a model (`add_particle`, `add_similar_particle`, `add_ghost_boson`, `add_coupling`, `add_tensor_coupling`, `add_term`). `generate_*` only creates an object without attaching it (`generate_index`, `generate_dirac_index`, and on `Particle`: `generate_field_strength`, `generate_similar_particle`, `generate_ghost_boson`).
- `coupling(name)` looks up an existing coupling and raises `LookupError` if it is absent. `add_coupling(name)` creates one. They are not duplicates.
- `refresh()` is internal. `finalize()` is the public step and `is_finalized()` checks it.
- Added `pymarty.symbolic.unitary_tensor(name, space, rows)`, backed by `csl::Unitary` through `unitaryTensor` in `src/csl/api/tensor.cpp`. The SM CKM matrix uses it.
- Known deviation: the Python SM CKM (`cpp_models/sm.py`, `_ckm_matrix()`) uses symbolic constants `V_ud`, `V_us`, ... instead of the numerical values `sm_input::V_*` used by the core `buildCKM`.
- `python/tools/check_modeling_bindings.py` maps facade names per class (`ALIASES`). Facade-only methods (`refresh`, `standardModel`, `qed`, `feynmanRules`, `computeAmplitude`, `computeSquaredAmplitude`, `Particle::raw`) are marked internal.

### Model builders as a package, static Minkowski index

- Model builders moved from `tests/modeling/cpp_models/` to `python/pymarty/modeling/models/` (`qed`, `qcd`, `sm`, `mssm`, `pmssm`, `nmfv`, `thdm`, `gthdm`). Each module exposes one class `X(Model)` with `__init__(self, *, finalize: bool = True)`. The body is a module-level `_add_content(model)`, so subclasses can reuse it with `finalize=False`. The tests import `from pymarty.modeling.models.X import X`.
- Decision: the C++ model wrappers are not reused by Python. The Python builders are duplicates of the C++ models, and the comparison with the C++ output is deferred to Phase 6 or later.
- Decision: `python/CMakeLists.txt` copies and installs the `pymarty` package recursively, so the `models/` subpackage is included.
- Decision: the particle accessors are `field_strength()` and `ghost_boson()`, without a `generate_` prefix, since they return existing objects. `generate_similar_particle` keeps its prefix, because it creates an unattached particle.
- Decision: `spacetime.minkowski_index()` is replaced by the static method `Model.generate_minkowski_index(name="mu")`, next to `generate_dirac_index`. It is backed by the new facade function `minkowskiIndex` in `src/marty/api/modeling/modeling.cpp`, which calls `csl::Minkowski.generateIndex`. The `spacetime` object is removed.

### Yukawa terms for THDM and GTHDM

- Decision: the THDM Yukawa terms are ported in `add_yukawas(model)` and `replace_yukawas(model, yukawas)`, in the shared module `python/pymarty/modeling/models/yukawas.py`. The six complex tensors `Y1u, Y1d, Y1e, Y2u, Y2d, Y2e` are indexed on `model.vector_space("SM_flavor")`, and are added with `add_tensor_coupling` in the C++ order. GTHDM reuses `add_yukawas`, since the C++ `GTHDM_Model` inherits `THDM_Model<GTHDM_type>`. The replacement is a model-building script, not part of `modeling.Model`, so GTHDM supplies its own version of it.
- Correction: the earlier "no SM_flavor space" gap was stale. `vector_space` and `add_tensor_coupling` were already bound.
- Done for THDM: `replaceYukawas` (Z2 coefficients, mass matrices, CKM rotation), the W ghosts `c_Wp`/`c_Wm` with the `c_W_1`/`c_W_2` replacements, the W_3/B diagonalisation with the A/Z renames (as in `SM_Model::diagonalizeSMMassMatrices`), and the SM flavour splitting (`break_sm_flavor`). The particle-name set now equals the C++ THDM set, and its strict xfail is lifted.
- Core change: `Model::replace(csl::Tensor, csl::Expr)` (`src/marty/api/modeling/modeling.cpp`) replaces a bare tensor, as C++ `Replaced(*this, Ye2, ...)` does. Replacing the indexed `Y(fi,fj)` by an expression without free indices (such as `0`) raises a `ValueError`, so the bare-tensor overload is needed.
- GTHDM (partial): `replaceYukawas` with the xi parametrisation is ported in `gthdm.py` (`replace_gthdm_yukawas`), in the C++ order, and reuses the shared THDM content (`add_thdm_content`). It needs the `matrix_tensor` facade (`matrixTensor` in `src/csl/api/tensor.cpp`) for the zero-first-row xi matrices.
- Open for GTHDM: the mass eigenstates of generations 2 and 3 (`c`, `t`, `s`, `b`, `mu`, `tau`) are not created. Generation 1 (`u`, `e`) splits in `finalize()`, so the Dirac mass term is diagonal there only. The xi cross terms apparently do not cancel in the generation 2 and 3 mass term. Cause not yet identified. `test_particle_names_match_cpp_gthdm` stays a strict xfail with this reason.

### Tensor involution properties and MSSM soft masses

The tensor facade now exposes the three CSL involution properties:
`add_complex_property`, `add_hermitian_property(space, init, result)` and
`add_transposed_property(space, init, result)`. The hermitian and transposed
setters need a space with exactly two indices; the core raises otherwise.

The MSSM soft masses (`M2_s_L`, `M2_s_E`, `M2_s_Q`, `M2_s_U`, `M2_s_D`) are
ported in `mssm.py`. Each is a complex hermitian flavour matrix on
`model.vector_space("SM_flavor")`, added as a tensor coupling, then used in
the Lagrangian term for its scalar. This mirrors `initSLeptons` and
`initSQuarks` in `src/marty/models/mssm.cpp`. The Feynman-rule count still
differs from C++, because the gaugino Yukawas and D-terms remain blocked.

### Known issues

- **Segfault in the full pymarty pytest run (open).** Under ctest, `pymarty.pytest` crashes with SIGSEGV in `csl::Index::getName()` (`src/csl/index.cpp`, around line 55), reached from `printLaTeX` in the golden test `test_golden_expressions_printing_matches_cpp` (`python/tests/symbolic/test_parity.py`, around line 245). It needs `tests/modeling/test_cpp_pmssm.py` to run earlier in the same process. Subset runs and standalone scripts do not reproduce it.
  - Ruled out: a stack overflow. `ulimit -s unlimited` avoids the crash, but gdb shows a shallow backtrace.
  - Leading hypothesis (unproven): a dangling `Space` pointer. `csl::Index` stores a raw `const Space *` (`src/csl/index.h`, line 84). Local spaces in the parity helpers (`V`, `S`, `U` in `test_parity.py`) can be destroyed while expressions that use them are still alive. `ExprHandle` holds no owner (`src/csl/api/expr.h`, line 184).
  - Workaround (temporary): `python/CMakeLists.txt` excludes `test_parity.py` from `pymarty.pytest` and runs it as `pymarty.pytest.symbolic_parity`.
  - Next steps: print the faulting `space->name` under gdb; run valgrind or an ASan build of `_pymarty` on the full suite (detached, long timeout); if confirmed, keep the owning space alive in core and add a regression test; remove the workaround; check `ulimit -c` with a writable `core_pattern`; decide whether the `finalize=True` default for QED, QCD and PMSSM stays.

