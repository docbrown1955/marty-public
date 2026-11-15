# Unit tests for CSL

Google Test cases for the symbolic kernel:

- `test_numbers.cpp`: integer, rational and real number types and their arithmetic.
- `test_literals.cpp`: constants, variables and the imaginary unit.
- `test_equality.cpp`: equality and comparison of expressions.
- `test_init_sanitizer.cpp`: initialisation and sanitiser checks for expressions.
- `csltestdata.h`: common expressions and fixtures shared by the tests.

Registered by `CMakeLists.txt` in this folder with the `units` label.
