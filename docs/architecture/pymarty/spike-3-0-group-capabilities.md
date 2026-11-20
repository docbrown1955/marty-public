# Spike 3.0: group theory capabilities of the MARTY core

Status: done. Probe run on the build in `/tmp/pymarty-build` (not committed). Phase 3 WP3.0.

## Question

Which group families and operations does the core support, what do they cost, and which
conventions must the `pymarty.group_theory` tests assert?

## Method

A standalone C++ probe (not in the repo) linked against `libmarty.so`. It constructs the concrete
groups in `src/marty/core/group.h` (`SU`, `SO`, `E6`, `U1`), builds irreps with
`SemiSimpleAlgebra::highestWeightRep`, and multiplies them with `Irrep::operator*`. It prints
Casimir and index values from `getQuadraticCasimir` and `getIndex`.

## Results

| Group | Construction | Irreps | Products | Casimir | Index T(r) | Time |
|-------|--------------|--------|----------|---------|------------|------|
| SU(3) | yes (`SU("SU3", 3)`) | fund, adj | 3 x 3bar = 1 + 8; 8 x 8 = 1 + 8 + 8 + 10 + 10 + 27 | fund 4/9, adj 1 | 1/2 both | ms |
| SU(5) | yes | fund | 5 x 5bar = 1 + 24 | not probed | not probed | 3 ms |
| SO(10) | yes (`SO("SO10", 10)`) | spinor 16 | 16 x 16 = 10 + 120 + 126 | not probed | not probed | 29 ms |
| E6 | yes (`E6("E6")`) | 27 | 27 x 27bar = 27 + 351 + 1728 (dim 2106) | not probed | not probed | 394 ms |
| U(1) | yes (`U1("U1")`) | n/a | not probed | n/a | n/a | <0.01 ms |

All decompositions have dimensions that add up (9, 64, 25, 256, 2106).

## Convention finding (blocking for the usage example)

The core's SU(3) Casimir is `C2(3) = 4/9` and `C2(8) = 1`. The standard normalisation with
`T(3) = 1/2` gives `C2(3) = 4/3` and `C2(8) = 3`. The core value is the standard one divided by N = 3.
The cause is in `SemiSimpleAlgebra::getQuadraticCasimir` (`src/marty/core/semiSimpleAlgebra.cpp:82`),
which uses `getEta()/2 * (a+2) . G . a` and the per-class `getEta()` overrides in
`semiSimpleAlgebra.h` (class `A` returns `CSL_1`, the default is `CSL_2`). The Dynkin index
`T(3) = 1/2` already matches the standard value. No repository test asserts a Casimir value.

The approved usage example `python/examples/usage/group_theory.py` asserts
`fund.casimir == Fraction(4, 3)` (line 38) and `adj.casimir == 3` (line 45). These will fail against
the core as it stands.

Decision (user, option "core"): Python exposes the core's Casimir as-is. The usage example now
asserts `4/9` and `1`, and needs a new G1 review. The core is not changed. The Python layer must
not rescale on its own (no algebra in Python).

## Other conventions to assert in tests

- Dynkin index: `T(fund of SU(N)) = 1/2`, `T(adj of SU(3)) = 1/2` as reported by `getIndex(r, 2)`.
- Products: `Irrep * Irrep` returns a sum with multiplicities, with dimensions summing to the product.
- The probe ran without exceptions for every group and product listed.

## Second probe round (core Casimir, index and remaining groups)

Core values, using `getQuadraticCasimir(highestWeight)` and `getIndex(r, 2)`:

| Group | Irrep | dim | C2 (core) | T (core, `getIndex(r,2)`) |
|-------|-------|-----|-----------|---------------------------|
| SU(5) | fund | 5 | 12/25 | 1/2 |
| SU(5) | adj | 24 | 1 | 1/2 |
| SO(10) | vector | 10 | 9/8 | 1 |
| SO(10) | adj | 45 | 2 | 1 |
| SO(10) | spinor | 16 | 45/32 | 1 |
| Sp(4) | fund | 4 | 11/15 | 1 |
| E6 | 27 | 27 | 13/9 | 1 |
| E6 | adj | 78 | 2 | 1 |

- Constructs without error: SU(5), SO(4), SO(5), SO(10), Sp(4), E6, E7, E8, F4, G2.
- `getDim()` on the group object returns 1 for E6, E7, E8, F4, G2. Do not use it for the group
  dimension; use the algebra dimension instead.
- SO(3) (B1) aborts. The core prints a `ValueError` and the process then segfaults. The API must
  validate SO dimension before construction.
- U(1): the charge of `highestWeightRep({1,3})` is `1/3`, and `q*q` returns a single irrep with dim 1.
  `getQuadraticCasimir` segfaults, and `getIndex` throws a core `NotImplementedError` that is not
  caught. The API must map these to typed errors and must not call the Casimir on U(1).
- Index conventions are unresolved. `getIndex(adj of SU(3), 2) = 1/2` and `getIndex(adj of SO(10), 2) = 1`
  do not match the standard Dynkin values (SU(3) adjoint 3, SO(10) adjoint 8). Do not assert these
  until the meaning of the second argument is checked in `semiSimpleAlgebra.cpp`.
- Sp(4) fund index 1 and C2 11/15 are not checked against a reference. Do not assert them yet.

## Not covered by this spike

- Generators and structure constants (`src/marty/core/su_n_algebra.h`) were not probed.
- Sp(2n) beyond Sp(4), and index conventions for groups other than SU(N) fundamental.

## Consequences for phase 3

- Supported in the core for SU(N), SO(N) with N ≥ 4, Sp, E6, E7, E8, F4, G2 construction and U(1)
  charges. The Python surface must not call Casimir or index on U(1).
- Index and Casimir for non-fundamental irreps are the main risk. Assert only the values listed in
  the Casimir column (core convention) and the Dynkin index for SU(N) fundamental.

## Decision: Dynkin index convention (after the probes above)

The index used by the binding is `T(r) = C2(r) dim(r) / dim(G)`, with the core Casimir. This gives
T(fund of SU(3)) = 1/6, T(adj of SU(3)) = 1, T(vector of SO(10)) = 1/4, T(spinor of SO(10)) = 1/2, and
T(27 of E6) = 1/2. The `getIndex(r, 2)` values in the tables above are not used, because that core
function returns a constant that cancels in the normalisation.
- The timing budget for the performance guard can be based on E6 27 x 27bar (about 0.4 s).
- The usage example needs a decision before WP3.2 (see Decision needed above).
