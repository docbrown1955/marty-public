# Unit tests

Fast Google Test checks, each registered with `SETUP_GTEST_TARGET(... units)`.

- `csl/`: CSL tests. `test_numbers.cpp` (number types), `test_literals.cpp` (constants,
  variables, imaginary unit), `test_equality.cpp` (expression comparison), and
  `test_init_sanitizer.cpp` (initialisation checks). `csltestdata.h` holds shared test data.
- `marty/`: `test_smparam_definition.cpp`, which checks the definition of Standard Model
  parameters.
- `marty-plugins/boltzmann-solver/`: `test_quantumnumbers.cpp`, which checks the quantum-number
  handling of the Boltzmann-solver plugin.

Add a new unit test next to the code it covers, in the matching subfolder, and register it in
that folder's `CMakeLists.txt`.
