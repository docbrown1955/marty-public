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
- Limitation: `standardModel()` and `qed()` do not populate group objects, so `Particle.irrep` raises `LookupError` on them.
- The C++ final particle list of `simpleSpectrum` was not captured (the reference run stops on a missing `libooptools.so`). The Python test checks the list that the Python port produces, following the same steps as the C++ file.
- `gettingStarted` library generation fails in this environment (missing `clooptools.h`). It is outside the port scope.

### Full ports (WP4.6, second step)

Facade additions for the ports: `Particle.expr`, `Particle.__call__(*indices)`, `field_strength`, `similar`, `set_self_conjugate`, `set_fundamental_flavor_rep`; `Model.generate_index`, `promote_to_goldstone`, `compute_amplitude`, `compute_squared_amplitude`; `Order`, `Leg`, `Amplitude`; `dirac_index`, `renamed_indices`.

Checks: `pytest -m "not slow"` on `tests/modeling` and `test_usage_examples.py` passes. `ctest -L 'units|integration|api'` passes (361 tests). The C++ `RelabelIndices` test was updated to the approved cycling behaviour: 3 indices over a 2-name pool give IDs `{0, 1}`.

### Timed run

| Command | Result | Time |
|---------|--------|------|
| `pytest -m "not slow"` on `tests/modeling` and `tests/test_usage_examples.py` | 24 passed | 4.6 s |
| `pytest -m "not slow"` on `tests/modeling/test_ports.py` | 3 passed | 1.7 s |
| `sampleSMFeynmanRulesLatex.x` (C++ reference run) | 190 rules | about 1.7 s |
