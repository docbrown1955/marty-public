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

Decision needed (open): either (a) Python exposes the core's Casimir as-is and the example is
amended to `4/9` and `1`, which needs a new G1 review, or (b) the C++ API applies the standard
normalisation (for example by multiplying by N), which is a core change beyond phase 3 scope.
The Python layer must not rescale on its own (no algebra in Python).

## Other conventions to assert in tests

- Dynkin index: `T(fund of SU(N)) = 1/2`, `T(adj of SU(3)) = 1/2` as reported by `getIndex(r, 2)`.
- Products: `Irrep * Irrep` returns a sum with multiplicities, with dimensions summing to the product.
- The probe ran without exceptions for every group and product listed.

## Not covered by this spike

- U(1) charge arithmetic (`Fraction` labels) was not probed beyond construction.
- Casimir and index for SU(5), SO(10) and E6 were not probed. Phase 3 WP3.1 must decide their
  expected values before writing tests.
- Generators and structure constants (`src/marty/core/su_n_algebra.h`) were not probed.

## Consequences for phase 3

- Supported in the core for SU(N), SO(N), E6 and U(1). E7, E8, F4, G2 and Sp were not probed and
  should be assumed unsupported until checked, so their Python surface raises `NotImplementedError`.
- The timing budget for the performance guard can be based on E6 27 x 27bar (about 0.4 s).
- The usage example needs a decision before WP3.2 (see Decision needed above).
