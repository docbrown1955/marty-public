"""Group theory basics: dimensions of the supported groups and a few products.

Phase 3 exit artifact. For each constructible group family it prints the
dimension of the fundamental and adjoint irreps, and checks them against the
known values. It then shows the SU(5) and E6 products that the group theory
tests use as references. The full SU(3) walk-through (products, Casimir and
Dynkin index in MARTY's normalisation) is in ``usage/group_theory.py``.

Run it with ``python group_theory_basics.py``.
"""

from pymarty.group_theory import E6, E7, F4, G2, SU, Sp

# (name, group, fundamental dimension, adjoint dimension)
FAMILIES = [
    ("SU(2)", SU(2), 2, 3),
    ("SU(3)", SU(3), 3, 8),
    ("SU(5)", SU(5), 5, 24),
    ("Sp(4)", Sp(4), 4, 10),
    ("F4", F4(), 26, 52),
    ("E6", E6(), 27, 78),
    ("E7", E7(), 56, 133),
    ("G2", G2(), 7, 14),
]

print(f"{'group':<8}{'rank':>5}{'dim G':>7}{'fund':>7}{'adj':>7}")
for name, group, fund_dim, adj_dim in FAMILIES:
    fund = group.fundamental()
    adj = group.adjoint()
    print(f"{name:<8}{group.rank:>5}{group.dim:>7}{fund.dim:>7}{adj.dim:>7}")
    assert fund.dim == fund_dim, name
    assert adj.dim == adj_dim, name

# 5 x 5bar = 1 + 24 in SU(5)
su5 = SU(5)
fund5 = su5.fundamental()
product = fund5 * ~fund5
print("SU(5): 5 x 5bar =", {str(irrep): mult for irrep, mult in product.items()})
assert sum(mult * irrep.dim for irrep, mult in product.items()) == 25
assert su5.trivial() in product
assert su5.adjoint() in product

# 27 x 27bar of E6 contains the singlet and has dimension 27 * 27
e6 = E6()
fund27 = e6.fundamental()
product27 = fund27 * ~fund27
print(
    "E6: 27 x 27bar has",
    len(product27),
    "irreps, singlet present:",
    e6.trivial() in product27,
)
assert sum(mult * irrep.dim for irrep, mult in product27.items()) == 27 * 27
assert e6.trivial() in product27
