# MARTY built-in models

Ready-made `mty::Model` subclasses, one per theory.

| File | Theory |
|------|--------|
| `sm.*` | Standard Model |
| `qed.*` | QED |
| `qcd.*` | QCD |
| `thdm.*` | Two-Higgs-doublet model |
| `gthdm.*` | General two-Higgs-doublet model |
| `nmfv.*` | Non-minimal flavour violation |
| `mssm.*`, `mssmData.h` | Minimal supersymmetric SM (fields and parameters) |
| `pmssm.*` | Phenomenological MSSM |
| `pmssm_lem.h`, `pmssm_lem/` | pMSSM with the split implementation (interaction, kinetic, mass and content parts) |

Each model class derives from `mty::Model` and defines its own content (gauge groups, particles,
Lagrangian, symmetry breaking). Calculations (amplitudes, widths, Wilson coefficients) are
inherited from `mty::Model`. To add a model, copy `qed.*` or `sm.*`. The `examples/` folder and the system
tests use these models (for example `samplePMSSM.cpp` and the `C7`, `C8`, `C9` tests).
