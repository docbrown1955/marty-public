# GRAFED core

Non-GUI helpers of GRAFED.

- `latexLink.*`: builds LaTeX snippets for particle and vertex labels and calls `latex` /
  `lualatex` and `dvipng` to produce images.
- `planargraph.*`: geometric primitives (`drawer::Point`) and the planar graph used to lay out
  diagrams.
- `adjacencyMatrix.h`: adjacency structure for diagram graphs.

The matching Qt-facing code is in `../gui/`.
