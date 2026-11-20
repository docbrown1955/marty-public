"""Dirac algebra: gamma matrices, traces, projections and Fierz identities.

Build the building blocks with ``g()``, ``eps()``, ``gamma()``, ``gamma5()``,
``P_L()``, ``P_R()`` and ``C()``, then combine them with the operators.
``current()`` and ``trace()`` take a chain of gamma matrices, and
``single_fierz()`` and ``double_fierz()`` rearrange two currents.

Indices are integers and accept any object with ``__index__``. Coefficients
are ``int``, ``float``, ``fractions.Fraction`` or a ``pymarty.symbolic``
expression. ``D`` is the symbolic dimension of the Minkowski metric and ``I``
the imaginary unit.

Simplification works on expressions inside a current: a bare gamma matrix,
``gamma5()`` or projector that is not part of ``current([...])`` cannot be
converted and raises ``RuntimeError``. ``order()`` is applied repeatedly until
it reaches a fixed point, so ``e.order().order() == e.order()``.

``DiracExpr`` compares canonically with ``==``, so it is unhashable.

>>> from pymarty.diracology import g, gamma5, trace
>>> gamma5() * gamma5() == 1
True
>>> trace([]) == 4
True
"""

from typing import NamedTuple

from pymarty._pymarty import diracology as _core

DiracExpr = _core.DiracExpr
FierzBasis = _core.FierzBasis
D = _core.D
I = _core.I  # noqa: E741  (the imaginary unit, named as in the C++ API)
g = _core.g
eps = _core.eps
gamma = _core.gamma
gamma5 = _core.gamma5
P_L = _core.P_L
P_R = _core.P_R
C = _core.C
current = _core.current
trace = _core.trace


class FierzResult(NamedTuple):
    """Result of a Fierz rearrangement of two currents.

    ``rhs`` is the rearranged product and ``lhs`` the product of the two
    currents before rearrangement.
    """

    rhs: DiracExpr
    lhs: DiracExpr


def single_fierz(
    chain1: DiracExpr,
    chain2: DiracExpr,
    basis: FierzBasis = FierzBasis.CHIRAL,
) -> FierzResult:
    """Single Fierz rearrangement of two currents.

    >>> from pymarty.diracology import FierzBasis, current, gamma, single_fierz
    >>> left = current([gamma(0)], 1, 2)
    >>> right = current([gamma(0)], 3, 4)
    >>> single_fierz(left, right, FierzBasis.STANDARD).lhs == left * right
    True
    """
    return FierzResult(*_core.single_fierz(chain1, chain2, basis))


def double_fierz(
    chain1: DiracExpr,
    chain2: DiracExpr,
    basis: FierzBasis = FierzBasis.CHIRAL,
) -> FierzResult:
    """Double Fierz rearrangement of two currents, with the same result as
    :func:`single_fierz`.

    >>> from pymarty.diracology import current, double_fierz, gamma
    >>> left = current([gamma(0)], 1, 2)
    >>> right = current([gamma(0)], 3, 4)
    >>> double_fierz(left, right).lhs == left * right
    True
    """
    return FierzResult(*_core.double_fierz(chain1, chain2, basis))


def symbolic_dimension(*, keep: bool) -> _core.SymbolicDimension:
    """Context manager setting the dimension convention for a block.

    ``keep=True`` keeps evanescent operators symbolic; ``keep=False`` evaluates
    epsilon contractions in 4 dimensions. The metric trace ``g(mu, mu)`` stays ``D``.
    The previous convention is restored on exit, also on exception.

    >>> from pymarty.diracology import g, symbolic_dimension
    >>> with symbolic_dimension(keep=False):
    ...     g(0, 0) == 4
    True
    """
    return _core.SymbolicDimension(keep)


def set_symbolic_dimension(*, keep_symbolic: bool) -> None:
    """Sets the dimension convention until it is changed again.

    Prefer :func:`symbolic_dimension`, which restores the previous convention.
    """
    _core.set_symbolic_dimension(keep_symbolic)


__all__ = [
    "C",
    "D",
    "DiracExpr",
    "FierzBasis",
    "FierzResult",
    "I",
    "P_L",
    "P_R",
    "current",
    "double_fierz",
    "eps",
    "g",
    "gamma",
    "gamma5",
    "set_symbolic_dimension",
    "single_fierz",
    "symbolic_dimension",
    "trace",
]
