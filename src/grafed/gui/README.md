# GRAFED GUI

The Qt 5 interface of GRAFED.

Main classes:

- `MainWindow` (`mainwindow.*`, `mainwindow.ui`): menus, toolbar, open / save / PDF export, and
  the diagram or session modes.
- `DiagramWidget` and `DiagramRenderer` (`diagramwidget.*`, `diagramrenderer.*`): the
  `QGraphicsView` that displays and edits a diagram.
- `Node`, `Edge`, `Diagram`, `Grid`, `Border`, `Curvature`: diagram items and drawing helpers.
- `NodeForm`, `EdgeForm` (`*.ui`): property editors for nodes and edges.
- `DialogLaTeX`, `PDFSetupDialog`, `HtmlConverter`: label editing and PDF export options.
- `jsonLoader`: loads diagrams (the parser itself lives in `../jsonparser/`).
- `resourcehandler.h`, `app.qrc`: embedded icons and images (see `ressources/`).

Build: the GUI is built by `CMakeLists.txt` as `grafed-gui`, linking Qt 5 and the GRAFED core.
`CMakeLists.txt.user` is a Qt Creator project file.
