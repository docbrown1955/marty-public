# Doxygen for CSL

Doxygen configuration for the CSL API documentation.

- `Doxyfile.in`: template configuration, filled by CMake. It reads the sources in `src/csl/`
  and uses `src/csl/README.md` as the main page.
- `CMakeLists.txt`: defines the `doc-csl` target, which runs Doxygen on the configured file.

Output goes to `docs/doc/csl/html` in the repository (committed HTML).
