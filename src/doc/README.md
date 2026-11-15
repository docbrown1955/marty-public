# Doxygen documentation sources

`src/doc/` configures the Doxygen API documentation for CSL and MARTY. The generated HTML is
committed under `docs/doc/csl/html` and `docs/doc/marty/html`.

- `CMakeLists.txt`: adds the `csl/` and `marty/` documentation targets.
- `csl/Doxyfile.in` and `marty/dox-config.in`: Doxygen configurations. Both use the
  `README.md` of their source folder as the main page (`USE_MDFILE_AS_MAINPAGE`).
- `common/`: the shared look (layout XML, header, footer, CSS, logo).

Build the documentation with the CMake targets `doc`, `doc-csl` or `doc-marty` (Doxygen must be
installed). `update-doc` is the target that copies the output into `docs/`.
