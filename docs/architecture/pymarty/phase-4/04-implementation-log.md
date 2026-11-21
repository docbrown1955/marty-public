# Phase 4 implementation log

## WP4.6: ports and examples

### Classification of the C++ example sources

| Source | Part | Status | Python counterpart |
|--------|------|--------|--------------------|
| `examples/gettingStarted.cpp` | Model: gauge groups, fermions, representations, breaking, renaming | Ported | `python/tests/modeling/test_ports.py` (`test_getting_started_*`) |
| `examples/gettingStarted.cpp` | Lagrangian terms, rotations, masses, `refresh`, amplitudes | Exempt | Not in WP4.6 scope (approved reduced scope) |
| `tests/system/src/simpleSpectrum.cpp` | Model part | Exempt | Needs flavour representations, generated indices and Dirac indices, not exposed by the facade yet |
| `examples/sampleSMFeynmanRulesLatex.cpp` | Standard Model Feynman rules as LaTeX | Ported | `test_sm_latex_document_has_one_entry_per_cpp_feynman_rule` |
| `python/examples/usage/modeling.py` | Usage example | Runs in `test_usage_examples.py` | n/a |

### Golden references

Generated from the C++ examples in the same build (`Release`, GCC 11):

- `gettingStarted` first half prints 12 particles: `B c_B u_R d_R W_1 W_2 W_3 c_W_1 c_W_2 c_W_3 u_L d_L`.
- `sampleSMFeynmanRulesLatex` writes 190 `Feynman vertex for` entries. The Python `latex_document()` also has 190.

The two LaTeX texts differ in size (159787 vs 159570 bytes). Generated index labels (`alpha_*`, `G_*`) differ between runs, so the test compares structure, not bytes.

### Known gaps found while porting

- `str(Model)` prints only particle names. The C++ `operator<<` also prints spin, mass, irreps and the Lagrangian. `Particle` does not expose spin or irreps in Python yet.
- `Particle` extras (`field_strength`, `similar`, `set_self_conjugate`, `set_mass`) are not bound. They are needed for the full `gettingStarted` and `simpleSpectrum` ports.

### Timed run

| Command | Result | Time |
|---------|--------|------|
| `pytest -m "not slow"` on `tests/modeling` and `tests/test_usage_examples.py` | 24 passed | 4.6 s |
| `pytest -m "not slow"` on `tests/modeling/test_ports.py` | 3 passed | 1.7 s |
| `sampleSMFeynmanRulesLatex.x` (C++ reference run) | 190 rules | about 1.7 s |
