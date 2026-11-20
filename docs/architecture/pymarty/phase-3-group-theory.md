# Phase 3: `pymarty.group_theory`

Pure group theory, usable without any model. Wraps `SemiSimpleGroup`, `SemiSimpleAlgebra`, `Irrep`,
`SumIrrep` and related classes in `src/marty/core` (`group.h`, `semiSimpleAlgebra.h`,
`representation.h`, `su_n_algebra.h`, `groupIndices.h`, `generator.h`).

Prerequisites: Phase 0 accepted. Independent of Phases 1 and 2.

Usage example gate: before any C++ API or binding work starts, `python/examples/usage/group_theory.py` is
written and approved by a human reviewer ([testing-strategy.md](testing-strategy.md) section 1.2). It fixes
the Python API of this phase and is the first deliverable.

Gate G1 status: `python/examples/usage/group_theory.py` approved by @gregoire-uhlrich_SAGCP on 2026-11-20,
after one revision (singlet search in 3 x 3bar and 3 x 3, and the 8 x 8 product). Spike 3.0
(`spike-3-0-group-capabilities.md`) confirmed the decompositions and T(fundamental) = 1/2, but found the
core Casimir is 1/N of the standard value. The user chose the core normalisation, so the example now
asserts C2(fundamental) = 4/9 and C2(adjoint) = 1. This change needs a new G1 review. The example is
skipped by `test_usage_examples.py` until `pymarty.group_theory` exists.

## 1. Scope

In scope:
- Groups: `U1`, `SU(n)`, and the other simple groups that the core can construct from an
  algebra type (`algebra::Type`: A, B, C, D, E6, E7, E8, F4, G2). Spike 3.0 establishes exactly which are
  constructible and which operations work for each. Classes unavailable in the core are not
  invented: they raise `NotImplementedError`.
- Irreps from Dynkin labels (the convention already used by `setGroupRep("L", 1)` in the examples:
  label `1` of SU(2) is the doublet; U(1) uses a rational charge), fundamental, adjoint, trivial, from
  dimension where unambiguous.
- Irrep data: dimension, highest weight (Dynkin labels), conjugate, self-conjugacy, quadratic
  Casimir, Dynkin index, weights with multiplicities.
- Tensor products and their decomposition into irreps with multiplicities.
- Group data: rank, dimension of the group, number of generators, name, algebra type.
- Generators and structure constants where the core provides them (SU(N) in the fundamental,
  `su_n_algebra.h`: Pauli and Gell-Mann matrices, `f` and `d`). Spike 3.0 decides whether numeric
  generator matrices for arbitrary irreps are available; if not they are out of scope with
  `NotImplementedError`.

Out of scope: gauge fields, couplings, anything touching `Model`, `GaugedGroup`,
`getCorrespondingIndex` (it takes a `QuantumFieldParent`), colour traces (`colorSpace.h`,
`traceIdentities.h`, handled in modeling/calculation).

## 2. Work packages

- WP3.0 Spike: inventory of the core. For each group family, which of: construction, irreps,
  products, Casimir, index, conjugation, generators work, how long they take for realistic sizes
  (SU(5), SO(10), E6 27), and which depend on `csl::Expr` results. Output: capability matrix in a
  decision record.
- WP3.1 C++ API `src/marty/api/group_theory/`: facade classes `GroupHandle`, `IrrepHandle`,
  `Decomposition`, plus free functions. All rational outputs are `Rational` (conversion from the
  `csl::Expr` returned by `getQuadraticCasimir`/`getIndex`, failing with `ValueError` otherwise).
  U(1) charges are `Rational`.
- WP3.2 Python bindings and package module.
- WP3.3 Port of the group part of `tests/integration/src/test_flavor_groups.cpp`.

## 3. C++ API (sketch) and Python spelling

| C++ API | Python |
|---------|--------|
| `GroupHandle su(int n)`, `u1()`, `so(n)`, `sp(n)`, `exceptional(Type)` | `SU(3)`, `U1()`, `SO(10)`, `Sp(4)`, `E6()`, `G2()` ... |
| `IrrepHandle irrep(group, vector<Rational> dynkin)` | `g.irrep(1, 0)` (accepts any iterable via `*labels` or a sequence) |
| `fundamental`, `adjoint`, `trivial`, `irrep_from_dim(dim, conjugate)` | `g.fundamental()`, `g.adjoint()`, `g.trivial()`, `g.irrep_from_dim(3)`; ambiguity gives `ValueError` listing the candidates |
| `dim(irrep)`, `highest_weight`, `is_self_conjugate`, `conjugate` | `r.dim`, `r.highest_weight`, `r.is_self_conjugate`, `r.conjugate()` or `~r` |
| `casimir(irrep)`, `dynkin_index(irrep)` | `r.casimir`, `r.dynkin_index` returning `fractions.Fraction` |
| `weights(irrep)` | `r.weights()` returning tuples of labels with multiplicities |
| `tensor_product(r1, r2)` | `r1 * r2` or `g.tensor_product(r1, r2)` returning a `Decomposition` |
| `Decomposition` | `Mapping[Irrep, int]` view (irrep to multiplicity), `.dim`, ordered deterministically |
| `rank`, `dim`, `name`, `type` | `g.rank`, `g.dim`, `g.name`, `g.type` |
| `generators(group, irrep)`, `structure_constants(group)`, `d_symbol(group)` | `g.generators(r)`, `g.structure_constants()`, `g.d_symbol()` returning nested tuples of `complex`/`Fraction` as appropriate |

Normalisation conventions (generator normalisation, Casimir and index normalisation) are those of
MARTY. The C++ doc comment and Python docstring state them explicitly (for the fundamental of SU(N)
the Dynkin index convention is verified in spike 3.0 and written down; tests assert MARTY's convention
and the cross-checks below that hold for any normalisation).

Python details: `Irrep` is hashable and comparable (`==` within one group, `TypeError` across
groups), immutable, with `__repr__` such as `SU(3)[1,0]`. Dynkin labels are accepted as `int`, or
`Fraction` for U(1) charges.

## 4. Test plan

L1 (`tests/api/group_theory/`):
- Construction: `SU2Dimensions` (labels 0..4 give 1,2,3,4,5), `SU3Dimensions` (3, 6, 8, 10, 15, 15', 27),
  `U1Charges`, `InvalidLabelsRaiseValueError`, `NonPositiveGroupSizeFails`.
- Known decompositions written as literals (references: any standard group theory text, listed in the
  test comments):
  - SU(2): `2x2 = 1+3`, `2x3 = 2+4`, `3x3 = 1+3+5`
  - SU(3): `3x3bar = 1+8`, `3x3 = 3bar+6`, `3x8 = 3+6bar+15`, `8x8 = 1+8+8+10+10bar+27`
  - SU(5): `5x5bar = 1+24`, `5x5 = 10+15`
  - SO(10) (if constructible): `16x16 = 10+120+126`
- Property tests for several groups and irreps (generated from a table):
  - `dim(r1 x r2) = sum(multiplicity * dim)` over the decomposition
  - conjugation is an involution, `dim(conj r) = dim r`, `conj` of `conj` is the same irrep
  - the trivial irrep is a unit: `r x 1 = r`
  - product is commutative (as multisets) and associative on a sample
  - Dynkin index sum rule: `T(r1 x r2) = dim(r1) T(r2) + dim(r2) T(r1)`
  - `T(r) * dim(G) = C2(r) * dim(r)`
  - adjoint: `C2(adjoint)` equals the dual Coxeter number in the chosen normalisation
- Rationals: `CasimirIsExactRational` (no float rounding), `NonRationalResultIsError`.
- Generators and structure constants: commutation relations `[T^a, T^b] = i f^{abc} T^c` checked
  numerically; Jacobi identity for `f`; hermiticity of generators; `tr(T^a T^b) = T(r) delta^{ab}`.
- State and lifetime: `IrrepOutlivesGroupHandleCopy`, no use of `Model::current`.
- Performance guard: a time-limited test of the biggest supported product (limit chosen from spike 3.0,
  the test documents the limit).

L2 (`python/tests/group_theory/`): parity tests for all L1 cases, plus
- duck typing of Dynkin labels (`int`, `Fraction`, objects with `__index__`, any iterable), irreps used
  as dict keys and set members, decompositions compared to plain dicts
- operators: `r1 * r2`, `~r`, comparisons across groups raise `TypeError`
- `Fraction` types returned (never `float`) and `Decomposition` is a `Mapping`
- generators returned as nested tuples whose elements are numbers (no pybind internals)
- independence from other modules (subprocess import test)
- doctests of the docstring examples

L5-style cross-check: `python/tests/system/test_flavor_groups_group_part.py` reproduces the group part
of `test_flavor_groups.cpp` (dimension queries and structure of flavour irreps).

## 5. Acceptance criteria

1. The capability matrix of spike 3.0 is committed, and the public API raises `NotImplementedError` (with
   the group family and operation in the message) for every unsupported combination (tested).
2. All literal decompositions above that apply to constructible groups pass, and all property tests
   pass for at least: SU(2), SU(3), SU(4), SU(5), U(1), and SO(10) and E6 if constructible.
3. For every irrep the sum rule and the `T dim(G) = C2 dim(r)` relation hold exactly with `Fraction`
   arithmetic.
4. Generator tests: commutation relations and trace normalisation hold to 1e-12 for SU(2) and SU(3)
   fundamentals (and any other case the core supports).
5. The Python layer contains no group theory: bindings only convert types. Review checklist signed in
   the PR.
6. Independence test passes (no import of `modeling`, `calculation`, `simulation`, `diracology`, and
   not even `symbolic`: the module returns exact `Fraction`/`complex`, not expressions).
7. Parity checker green. Port of the group part of `test_flavor_groups.cpp` passes.
8. `ctest -L 'units|integration'` green. Quality gates pass.
9. `python/examples/usage/group_theory.py` was approved by a human before implementation started (reviewer
   and date in the PR), is unchanged since or its changes were re-approved, and runs in
   `test_usage_examples.py` (L4b).

## 6. Exit artifacts

Docstrings with the normalisation conventions, an example script
`python/examples/group_theory_basics.py` (products and Casimirs of SU(3)), golden data of
decompositions, the capability matrix.

## 7. Risks and open questions

- `getCorrespondingIndex` and colour-space code tie groups to fields; they are deliberately not exposed.
- Large irreps can be slow (weight system enumeration). Mitigation: documented limits, test guards.
- Numeric generator matrices for arbitrary irreps may be missing in the core. If so, this is reported
  rather than silently approximated.
- Conjugation conventions of `Irrep` (labels order, self-conjugate U(1) rep) need to be pinned by tests
  that read MARTY's outputs once and then assert literal values.
