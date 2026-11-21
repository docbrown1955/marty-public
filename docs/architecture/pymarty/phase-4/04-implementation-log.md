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
  - `mergeTerms`.

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

