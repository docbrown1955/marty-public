"""Group theory: simple gauge groups, irreps and tensor products.

Groups are built with ``SU(n)``, ``SO(n)``, ``E6()`` and ``U1()``. Irreps are
labelled by Dynkin labels, or by an exact charge for U(1), and are immutable
and hashable. ``r1 * r2`` is the tensor product, returned as a
``Decomposition``: a read-only mapping from irreps to multiplicities.

Casimirs and Dynkin indices are ``fractions.Fraction`` objects in MARTY's core
normalisation. The Dynkin index is ``T(r) = C2(r) dim(r) / dim(G)``, so that
``T(fundamental of SU(3)) = 1/6``. Irreps of different groups cannot be
compared, and comparing them raises ``TypeError``.

>>> from pymarty.group_theory import SU
>>> su3 = SU(3)
>>> su3.fundamental() * ~su3.fundamental() == {su3.trivial(): 1, su3.adjoint(): 1}
True
>>> su3.fundamental().dynkin_index
Fraction(1, 6)
"""

from collections.abc import Mapping

from pymarty._pymarty import group_theory as _core

Group = _core.Group
Irrep = _core.Irrep
Decomposition = _core.Decomposition

# A Decomposition is a read-only mapping from irreps to multiplicities.
Mapping.register(Decomposition)


def SU(n: int) -> Group:
    """Special unitary group SU(n), with n >= 2."""
    return _core.Group.su(n)


def SO(n: int) -> Group:
    """Special orthogonal group SO(n), with n >= 4."""
    return _core.Group.so(n)


def E6() -> Group:
    """Exceptional group E6."""
    return _core.Group.e6()


def U1() -> Group:
    """Abelian group U(1). Its irreps are labelled by a rational charge."""
    return _core.Group.u1()
