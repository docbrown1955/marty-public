# TODO: model (de)serialization

Status: not started. Blocks the pymarty ports of long-to-build models (MSSM, PMSSM, NMFV).
See the [phase-7 spec](../pymarty/phase-7/03-spec.md), section "Postponed".

## Problem

Models are currently defined only through C++ code (`tests/system/src/*.cpp`,
`src/marty/modeling/`). Building a complex model (for example `pmssm_lem`) means running all
model-building steps every time. Some of those models were then auto-generated through a
model-builder feature once the steps were done. That path is complex, fragile and not ideal.

## Goal

MARTY should save and load a fully built model without rerunning the model-building steps and
without C++ user code. Loading a file must give the same model state as building it.

## Scope to serialize

The state is spread across several objects, so the format is probably a set of files, not one:

- expressions (`csl::Expr` trees, including constants, symbols and their properties)
- tensors and their indices (`csl::Index`, index ranges, dimensions)
- particles and fields (`mty::Particle`, gauge and mass eigenstates, mixing matrices)
- Lagrangian terms and their generation (`mty::Lagrangian`, interaction terms)
- symmetry breaking and parameter definitions (`mty::Model` setup)
- model-level options and metadata needed to reproduce the calculation

## Open design questions

- Text format (JSON or a MARTY-specific text form) versus binary. Text is preferred for review.
- One file per object kind, or one file per model with sections. Decide after a first pass.
- Object identity: expressions and indices are shared, so references need IDs.
- Format versioning and the migration policy for older files.
- Whether the file is an input of the C++ library, the Python API, or both.

## Acceptance (to define when work starts)

- Round trip: build a small model (for example the SM subset in `python/tests/system`), save it,
  load it, and get identical Feynman rules and amplitudes.
- Round trip of one long model (for example `pmssm_lem`) without rerunning the build steps.
- Parse errors and version mismatches raise clear errors.

## Related

- Phase-4 known issues (`docs/architecture/pymarty/phase-4/04-implementation-log.md`): serialization
  must not hit the same dangling-pointer risk seen in `csl::Index`.
