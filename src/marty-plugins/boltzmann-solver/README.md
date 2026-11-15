# Boltzmann-solver plugin

`src/marty-plugins/boltzmann-solver/` is a MARTY plugin: a separate library that depends on
MARTY, and is never depended upon by it. It builds `marty-boltzmann-solver` (shared) and
`marty-boltzmann-solver-static`.

Main features:

- `quantumnumberdata.*`: `QuantumNumberData` holds the quantum numbers (conserved charges) of
  particles and checks them for a set of insertions.
- `processdata.*`: `ProcessData` is a list of processes. Each process has a name, incoming and
  outgoing `mty::Insertion`s, and quantum numbers. It can generate JSON for the particle lists
  and quantum-number lists.
- `libjsondata.*`: a JSON exporter that writes the process data to a file for the external
  solver.

Tests: `tests/units/marty-plugins/boltzmann-solver/test_quantumnumbers.cpp`.

Plugin rules (from [../README.md](../README.md)): own directory, static and shared libraries,
depends on MARTY only, and automatically tested.
