# GRAFED

GRAFED (`src/grafed/`) is the Qt 5 application `grafed-gui` used to draw, edit and display
Feynman diagrams. MARTY starts it through `Show(...)`, which writes the diagrams to a JSON
file and runs the program on it.

Sub-folders:

- `core/`: LaTeX label handling (`latexLink`) and diagram geometry (`planargraph`,
  `adjacencyMatrix.h`). No Qt widgets here.
- `gui/`: the Qt user interface (main window, drawing widget, nodes, edges, forms, dialogs).
- `jsonparser/`: reads and writes the JSON diagram format.

Features: draw vertices and propagators on a grid, choose particle styles (scalar, fermion,
vector, ghost, gaugino, gluon, and so on, see `gui/ressources/images`), set labels with LaTeX
rendering, and export to PDF.

Build: `src/grafed/CMakeLists.txt` builds `grafed-gui`. It needs Qt 5, and at run time
`latex`, `lualatex`, `dvipng` and ImageMagick's `convert`.

Entry points: `gui/main.cpp`, `gui/mainwindow.h`, `gui/diagramwidget.h`.
