# Gamma-matrix API (`mty::gamma_api`)

A small, free-function interface to the Dirac algebra of SGL, meant for use from other languages
or scripts:

- Single objects: `g`, `eps`, `gamma`, `gamma5`, `P_L`, `P_R`, `C`, `DMinko`, `imaginaryUnit`.
- Chains and traces: `current`, `trace`.
- Simplification: `ordered`, `simplified`, `project`, and Fierz helpers such as
  `applySingleFierz` (basis choice via `FierzBasis`).
- `keepSymbolic4D` controls whether 4D identities are used.

Expressions are `mty::gamma_api::Expr` (an alias for `sgl::GExpr`). Error types are in
`gamma_api_error.h`. The integration test `tests/integration/src/gammaAPI.cpp` covers this
API.
