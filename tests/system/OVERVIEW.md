# System tests

Physics regression tests. Each test is a program in `src/` that runs a full MARTY calculation,
writes its numbers to `libs/` or stdout, and compares them with a reference file in
`data/output/` (see `src/comparedata.h`).

The full description of each test (purpose, method and tolerance) is in
[README.md](README.md). Summary of the content:

- `src/`: test programs (for example `FItests.cpp`, `majoranaLoop.cpp`, `testConjugation.cpp`,
  `testModelWidth.cpp`, `Zmumu.cpp`, `EWGauge.cpp`, `gg_to_tt.cpp`, `NLepto.cpp`) and the
  B-physics, C7/C8/C9, g-2 (`gm2SM.cpp`, `gm2PMSSM.cpp`) and spectrum tests.
- `libsrc/`: sources of the generated libraries used by some tests (`ckm.*`, examples).
- `data/`: input files (`input/`, spectra) and reference outputs (`output/`).
- `CMakeLists.txt`: `ADD_SYSTEM_TEST(display-name file)` registers each test and a cleanup
  step. Tests run from this folder with `LD_LIBRARY_PATH` set to the build tree.

The `README.md` in this folder is the test catalogue and is kept as is. This `OVERVIEW.md`
complements it.
