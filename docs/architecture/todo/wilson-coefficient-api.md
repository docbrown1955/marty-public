# TODO: Wilson coefficient API refactor

Status: not started. Deliberately deferred until the C9_SM divergence is fixed, so that no
existing behaviour changes while C9 is being debugged.

## Problem

- `Model::getWilsonCoefficient` (the name suggests a single coefficient) actually decomposes an
  amplitude onto the operator basis. Its name is misleading.
- `Model::computeWilsonCoefficients` takes raw insertions and recomputes the amplitude, even when
  the caller already holds an `Amplitude` object. The dedicated path of the facade
  (`src/marty/api/calculation/calculation.cpp`, `wilsonCoefficients`) does this and reuses the
  amplitude's insertions, which may already be reordered by `fermion_order`.
- Reading a single coefficient requires the caller to know the internal operator tensors.

## Planned changes

1. Add `decomposeOnOperatorBasis(...)` in C++ to replace `getWilsonCoefficient`, and
   `getWilsonCoefficients` / `computeWilsonCoefficients` (new names and signatures), keeping the old
   ones as deprecated wrappers.
2. Make the amplitude-based decomposition take the `Amplitude` object without recomputing it.
3. Add `getOperatorCoefficient(wilsonSet, operators...)` returning the coefficient of each
   requested operator. Keep helpers such as `getDimension6Operators(...)`.
4. Adapt every in-source C++ example and test to the new API, with deprecation notes on the old
   functions.
5. Correct the pymarty layer (`python/src/calculation.cpp`, `python/pymarty/calculation/__init__.py`,
   stubs and tests) to match.

## Notes

- Items 1-2 change the behaviour of the dedicated Wilson path. Do them after the C9_SM mirror
  comparison has identified the cause of the zero photon coefficient.
