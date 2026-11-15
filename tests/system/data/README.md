# System test data

- `input/`: input files used by the tests, such as `test.lha` (LHA parameter file) and
  `spectra/`, which holds about 100 spectrum files (`spectrum_N.txt`) for scan-style tests.
- `output/`: reference results that the system tests compare against, for example
  `BsMixingSM.txt`, `C7_SM.txt`, `ee_mumu_tot.txt`, `gm2_pmssm.txt`, `widths.txt` and
  `reinterpretation.txt`.

Changing a reference file changes what counts as a passing test, so update it only when a
physics change is intended and explained.
