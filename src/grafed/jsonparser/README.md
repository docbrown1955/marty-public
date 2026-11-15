# GRAFED JSON parser

Reads and writes the JSON format used to exchange diagrams between MARTY and GRAFED.

- `jsonObject.h`: generic JSON node representation, also used by the Boltzmann-solver plugin.
- `jsonLoader.*`: parses a JSON file into diagram objects and writes them back.

`MARTY`'s `Show()` writes this format to `/tmp/graphs.json` before launching `grafed-gui`. The
same parser is built as the object library `jsonparser`, and the Boltzmann-solver plugin
includes `grafed/jsonparser/jsonLoader.h` to export its data.
