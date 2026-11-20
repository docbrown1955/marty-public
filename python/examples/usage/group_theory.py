"""Usage example: group theory (SU(N), U(1), irreps and tensor products).

``pymarty.group_theory`` gives access to the simple groups of MARTY's core
without any model. A group is built from its name, and its irreps are
labelled by Dynkin labels. Irreps are immutable and hashable. Tensor products
are decomposed into irreps with multiplicities. Casimirs and Dynkin indices
are exact ``Fraction`` objects, in MARTY's normalisation.

Every step prints what it shows and asserts the result, so the example fails
as soon as the library stops doing what is written here.

Run it with ``python group_theory.py``. It is also run by the test suite
(see ``docs/architecture/pymarty/testing-strategy.md``, section 1.2).
"""

from fractions import Fraction

from pymarty.errors import MartyValueError
from pymarty.group_theory import SU, U1

su3 = SU(3)
print("group:", su3, "rank", su3.rank, "dim", su3.dim)
assert su3.rank == 2
assert su3.dim == 8

# The fundamental representation and its conjugate
fund = su3.fundamental()
antifund = ~fund
print("fundamental:", fund, "dim", fund.dim)
print("conjugate:", antifund)
assert fund.dim == 3
assert fund.highest_weight == (1, 0)
assert antifund == su3.irrep(0, 1)
assert not fund.is_self_conjugate

# Casimir and Dynkin index, exact rationals in MARTY's normalisation. Both are
# 1/N of the standard SU(N) values (4/3 and 1/2 for N = 3).
print("C2(fundamental) =", fund.casimir, " T(fundamental) =", fund.dynkin_index)
assert fund.casimir == Fraction(4, 9)
assert fund.dynkin_index == Fraction(1, 6)

adj = su3.adjoint()
print("adjoint:", adj, "dim", adj.dim, "C2 =", adj.casimir)
assert adj.dim == 8
assert adj.casimir == 1

# Tensor product: 3 x 3bar = 1 + 8
prod = fund * antifund
print("3 x 3bar =", dict(prod))
assert prod == {su3.trivial(): 1, adj: 1}
assert sum(mult * irrep.dim for irrep, mult in prod.items()) == 9

# Singlet search: a trivial irrep in the product means the two irreps can
# couple to a singlet, i.e. an interaction is possible. 3 x 3 has none.
print("trivial in 3 x 3bar:", su3.trivial() in prod)
assert su3.trivial() in prod
no_singlet = fund * fund
print("3 x 3 =", dict(no_singlet))
assert no_singlet == {su3.irrep(0, 1): 1, su3.irrep(2, 0): 1}
print("trivial in 3 x 3:", su3.trivial() in no_singlet)
assert su3.trivial() not in no_singlet

# A larger case: 8 x 8 = 1 + 8 + 8 + 10 + 10bar + 27 (the adjoint appears twice)
bigger = adj * adj
print("8 x 8 =", dict(bigger))
assert bigger == {
    su3.trivial(): 1,
    adj: 2,
    su3.irrep(3, 0): 1,
    su3.irrep(0, 3): 1,
    su3.irrep(2, 2): 1,
}
assert sum(mult * irrep.dim for irrep, mult in bigger.items()) == 64
print("trivial in 8 x 8:", su3.trivial() in bigger)
assert su3.trivial() in bigger

# Invalid Dynkin labels are rejected with a MARTY error
try:
    su3.irrep(1, 0, 0)
except MartyValueError as error:
    print("invalid labels rejected:", error)
else:
    raise AssertionError("SU(3) accepted three Dynkin labels")

# U(1) charges are exact rationals
u1 = U1()
charge = u1.irrep(Fraction(1, 3))
print("U(1) charge:", charge, "dim", charge.dim)
assert charge.dim == 1
assert charge * charge == {u1.irrep(Fraction(2, 3)): 1}
