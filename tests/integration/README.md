# Integration tests

Google Test programs that run complete MARTY computations and check the results, registered in
`CMakeLists.txt` with `ADD_INTEGRATION_TEST`. Each one links `marty` and carries the
`integration` label.

- `src/test_flavor_groups.cpp`: flavour group handling in models.
- `src/test_specific_gauge_couplings.cpp`: gauge couplings of specific models.
- `src/gammaAPI.cpp`: the gamma-matrix API (`src/marty/api/`).

Sources are in `src/`. The `CMakeLists.txt` in this folder lists the tests.
