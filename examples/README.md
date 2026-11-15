# Examples

Stand-alone programs showing how to use the MARTY API. Each file is built as `<name>.x` by
`CMakeLists.txt` and linked with `marty`.

| File | Shows |
|------|-------|
| `gettingStarted.cpp` | Full first example: build a toy model with `SU(2)` and `U(1)` groups, break the symmetry, add masses, compute Feynman rules, the tree-level amplitude and the squared amplitude, then generate a library. Companion of the website tutorial. |
| `sampleScalar.cpp` | A simple scalar model. |
| `sampleQED.cpp`, `sampleSQED.cpp` | QED and supersymmetric QED amplitudes. |
| `sampleQCDSextet.cpp` | Group representations and a cross section in QCD with a colour sextet. |
| `sample2HDM.cpp` | Two-Higgs-doublet model and Wilson coefficients. |
| `samplePMSSM.cpp` | The pMSSM model from `src/marty/models/`. |

Start with `gettingStarted.cpp`. The `.gitignore` excludes the `test*` outputs.
