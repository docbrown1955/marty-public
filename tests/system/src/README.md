# System test programs

`tests/system/src/` contains the physics programs run by the system test suite. Each `.cpp`
file is an executable built by `tests/system/CMakeLists.txt` and linked with `marty`.

Main groups:

- Conjugation and Majorana loops: `testConjugation.cpp`, `majoranaLoop.cpp`.
- Widths and cross sections: `testModelWidth.cpp`, `Zmumu.cpp`, `gg_to_tt.cpp`,
  `freeQuarkAmplitudes.cpp`, `NLepto.cpp`.
- Flavour: `BsMixingSM.cpp`, `BsMixingVLQ_*.cpp`, `C7_*.cpp`, `C8_*.cpp`, `C9_*.cpp`.
- Muon g-2: `gm2SM.cpp`, `gm2PMSSM.cpp`.
- Models and spectra: `EWGauge.cpp`, `SM_2IDM.cpp`, `SM_triplet.cpp`, `simpleSpectrum.cpp`,
  `testConjugation.cpp`, `Bgenesis.cpp`.
- Infrastructure: `FItests.cpp` (basic Feynman integrals), `comparedata.h` (numerical
  comparison with tolerance).

Reference outputs are in `../data/output/`, and the `libsrc/` folder holds generated-library
sources used by some tests.
