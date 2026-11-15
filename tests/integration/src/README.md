# Integration test sources

The three Google Test programs of the integration suite:

- `test_flavor_groups.cpp`: flavour group handling.
- `test_specific_gauge_couplings.cpp`: gauge couplings in specific models.
- `gammaAPI.cpp`: checks of the gamma-matrix API from `src/marty/api/`.

Built and run through the parent folder's `CMakeLists.txt` (label `integration`).
