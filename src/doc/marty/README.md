# Doxygen for MARTY

Doxygen configuration for the MARTY API documentation.

- `dox-config.in`: template configuration, filled by CMake. It reads `src/marty/` and uses
  `src/marty/README.md` as the main page.
- `CMakeLists.txt`: defines the `doc-marty` target, which runs Doxygen on the configured file.

Output goes to `docs/doc/marty/html` in the repository (committed HTML).
