# Marty plugins

`src/marty-plugins/` contains extensions that are built as separate libraries on top of MARTY.
Rules (see the `README.md` in this folder): each plugin has its own directory, builds static and
shared libraries, may depend on MARTY (never the reverse), and must be tested automatically.

Content:

- `boltzmann-solver/`: exports processes and quantum numbers from MARTY models to JSON for a
  Boltzmann-equation solver. See its [README.md](boltzmann-solver/README.md).
- `CMakeLists.txt`: adds each plugin subdirectory.

The `README.md` in this folder is the plugin guideline and is kept as is. This `OVERVIEW.md`
complements it with the current content.
