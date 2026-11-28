# Simplify insertion ordering

Status: implemented in the working tree, not committed. C9_SM is not yet fixed
(the `C9_A` column matches the reference, `C9_BZ` and `C9_Tot` do not).

## Behaviour before this change

- `AmplitudeInitializer::orderInsertions`
  ([amplitudeInitializer.cpp](../../../src/marty/core/amplitudeInitializer.cpp))
  sorts the external insertions into a canonical order and applies the same
  permutation to the kinematics.
- It was controlled by the global `mty::option::applyInsertionOrdering`
  ([mrtOptions.h](../../../src/marty/core/mrtOptions.h)), copied into
  `FeynOptions::orderInsertions`.
- The fermion order was remapped to follow the sort, with the wrong formula
  (see below).
- `computeWilsonCoefficients_4Fermions` set `orderInsertions = false` on its
  own copy, so it saw the raw insertions. The Python dedicated-matching path
  received an amplitude whose insertions had already been sorted, so the two
  disagreed. This is the cause of the C9_SM zeros.
- `feynmanRule.cpp` turned the global off with `ScopedProperty` while it built
  Feynman rules.

## Implemented change

- The global `mty::option::applyInsertionOrdering` is removed.
- `FeynOptions::orderInsertions` defaults to `true`. `feynmanRule.cpp` sets it
  to `false` on its own options.
- `FeynOptions::keepFermionInsertionOrder` (default `false`) is set by
  `Model::computeAmplitude` when the user provided a fermion order. When it is
  set, `orderInsertions` sorts only the bosonic positions and leaves the
  fermionic positions as given. The kinematics follow the same permutation.
- "No user order" is represented by an empty fermion order, not by an identity
  permutation. The model fills in `defaultFermionOrder` after the flag is set,
  so the flag is what records that the user gave an order.
- Remap for the default path: `newFermionOrder[k] = newLabelOf[fermionOrder[k]]`,
  where `newLabelOf` maps an old label to its new position.

This differs from the original proposal (`std::optional` plus a per-options
flag only). The empty-vector-plus-flag form gives the same behaviour without
changing the public `getFermionOrder` type.

## Remap bug

With `fermions[k]` = old fermion label at new position `k` (the sort
permutation), the correct remap of the user order is
`newFermionOrder[k] = inv[fermionOrder[k]]`, where `inv` is the inverse of
`fermions`. The current code computes `fermionOrder[fermions[k]]`, which is
`order ∘ permutation` instead of `inverse ∘ order`.

For C9_SM (user order `{1, 0, 2, 3}`) the correct result is `{1, 2, 0, 3}`.
The current code gives `{2, 0, 1, 3}`, and the operator search then finds no
match, so the photon branch is zero.

## Design

1. Always sort the bosonic insertions, so the canonical output does not depend
   on the user's input order.
2. When the user has provided a fermion order, do not sort the fermionic
   insertions. The user's order then refers to the same fermions in the
   amplitude, the kinematics and the operator search.
3. "No user order" is an empty fermion order plus the flag, so an identity
   permutation is never mistaken for "unset".

Consequences:

- The remap bug cannot occur for user orders.
- The Python dedicated path receives insertions in the user's frame, the same
  as the C++ getter.
- Outputs without a user fermion order change only through the remap fix.

## Validation status

- ctest `units|integration|api`: 385/385 pass.
- Fast pymarty suite: 501 passed.
- C9_SM slow test: `C9_A` matches the reference. `C9_BZ` and `C9_Tot` still
  diverge, so the test still fails. This is the open item.
- Still to add: a unit test for a non-identity user order that checks fermion
  insertions keep their order.
