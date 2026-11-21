Status: READY
Updated: 2026-11-21
Author: orchestrator (written from the findings of investigator "inv-tests-models"; the subagent could not write files, the orchestrator re-checked the lines marked [V])

# Investigation: tests, examples and built-in models

## 1. Question

Which existing examples and tests build models, built-in models or Feynman rules, and what would the Python ports need? Do the built-in model classes take constructor arguments?

## 2. Facts

1. [V] `examples/gettingStarted.cpp:31-65` builds a toy SU(2) x U(1) model, adds three Weyl fermions, sets group reps (`setGroupRep("Y", {1, 6})`), breaks U(1) and SU(2), renames particles and prints the model with `cout << toyModel`. The second half (Higgs mechanism, Feynman rules) starts after line 65.
2. [V by report, file read partially] `examples/sampleSMFeynmanRulesLatex.cpp:14-33` default-constructs `SM_Model`, computes Feynman rules and writes a LaTeX file. `examples/samplePMSSM.cpp:11-20` does the same with `PMSSM_Model`.
3. [V] `tests/integration/CMakeLists.txt:11-12` registers `test_physics_latex` and `test_feynman_rules_latex` through `ADD_INTEGRATION_TEST`. `test_specific_gauge_couplings` is also registered there (reported).
4. [V by report] `simpleSpectrum` is a `system` test (`tests/system/CMakeLists.txt:32`) comparing with `tests/system/data/output/simplespectrum.txt` (`tests/system/src/simpleSpectrum.cpp:128`). It must not be run by the agent (AGENTS.md).
5. [V] Built-in model classes: `QED_Model : public mty::Model` (`src/marty/models/qed.h:30`), `SM_Model : public mty::Model` (`sm.h:35`), `THDM_Model : public mty::SM_Model` (`thdm.h:67`), `MSSM_Model : public mty::Model, protected MSSM_Data` (`mssm.h:275`), `PMSSM_Model : public MSSM_Model` (`pmssm.h:30`). The plan's factory names (`models.QED()`, `models.SM()`, ...) map to these classes.
6. Reported, not re-checked: `gthdm.h`, `nmfv.h`, `qcd.h` also exist in `src/marty/models/`; the plan says to include them only if public and tested.
7. Reported, not re-checked: `THDM_Model` is a template on a type parameter (`thdm.h:48-55`), so the Python `THDM()` needs a fixed choice of type.

## 3. Consequences

- The model-building part of `gettingStarted` is a good first port: it covers `addGaugedGroup`, `init`, `renameParticle`, `weylfermion_s`, `setGroupRep` with Rationals, `breakGaugeSymmetry`, and printing.
- `sampleSMFeynmanRulesLatex` and `samplePMSSM` need the Feynman-rule facade and the LaTeX output.
- Built-in constructors take no arguments (reported for SM and PMSSM); QED is assumed the same and must be checked during WP4.3.

## 4. Options and trade-offs

- Port the built-in models through factory functions in C++ (`make_model(ModelKind)`, plan WP4.3), so the Python side only names them. **Recommended, as in the plan.**
- Expose `THDM` as one fixed type variant; a second variant would need its own name. Open question (Q6).

## 5. Questions for the user

Captured in [02-questions.md](02-questions.md).

## 6. Not verified

- Runtimes of the examples and the tests (no build run).
- The full contents of `tests/integration/src/test_feynman_rules_latex.cpp`, `test_physics_latex.cpp` and `test_specific_gauge_couplings.cpp` (only their registration was checked).
- Whether `QED_Model` has a no-argument constructor.
