# WP5.6 spike: LaTeX at export and JSON schema

## Question

Does exporting diagrams (`Amplitude.json()`, `Drawer::launchViewer`) run LaTeX,
and what is the JSON schema?

## Findings

- `LatexLinker::saveMultiple` (`src/grafed/core/latexLink.cpp`) writes the
  diagram file only. It does not run LaTeX: the diagram text is produced from
  the stored expressions and the file is written directly.
- The file has the form `"root" : { "diagrams" : [ ... ] }` without outer
  braces. It is read by GRAFED but is not strict JSON. The Python API keeps
  this format and documents it as "Diagram file ... in the format read by
  GRAFED" (`python/src/calculation.cpp`).
- Consequence: the Python `Amplitude.json()` and the C++ viewer need no LaTeX
  installation, and a missing LaTeX toolchain does not affect export.

## Decision

- Keep the GRAFED format unchanged in phase 5. A strict JSON schema would
  change the GRAFED reader and is out of scope.
- The viewer (WP5.7) writes into a private `mkdtemp` directory and launches
  `grafed-gui` with `posix_spawnp` and an argument vector, with no shell.
