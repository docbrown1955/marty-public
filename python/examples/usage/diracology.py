"""Usage example: Dirac algebra (gamma matrices).

``pymarty.diracology`` gives access to the Dirac / gamma-matrix algebra of
SGL. Gamma matrices, the metric and epsilon tensors are built from integer
Lorentz indices. Expressions are immutable: a method returns a new expression
and never changes its input. Coefficients are ints, floats, ``Fraction``
objects or symbolic objects.

Every step prints what it shows and asserts the result, so the example fails
as soon as the library stops doing what is written here.

Run it with ``python diracology.py``. It is also run by the test suite
(see ``docs/architecture/pymarty/testing-strategy.md``, section 1.2).
"""

from fractions import Fraction

from pymarty.diracology import (
    P_L,
    P_R,
    C,
    D,
    DiracExpr,
    FierzBasis,
    I,
    current,
    double_fierz,
    eps,
    g,
    gamma,
    gamma5,
    single_fierz,
    symbolic_dimension,
    trace,
)
from pymarty.errors import MartyValueError

# 1. Building blocks. Indices are integers: they label the Lorentz (or Dirac)
#    slot of a tensor. Repeated indices are summed, as in the Feynman rules.
metric = g(0, 1)
print("g(0, 1) =", metric)
assert isinstance(metric, DiracExpr)
assert g(0, 1) == g(1, 0)

print("gamma(0) =", gamma(0))
print("gamma5() =", gamma5())
print("P_L() + P_R() =", P_L() + P_R())
print("C() =", C(), " I =", I, " D =", D)

# 2. Arithmetic. Python numbers mix with DiracExpr on either side, and like
#    terms are combined directly.
combined = 2 * g(0, 1) + 3 * g(0, 1) - g(0, 1) / 2
print("2 g + 3 g - g/2 =", combined)
assert combined == Fraction(9, 2) * g(0, 1)
assert -g(0, 1) + g(0, 1) == 0

# 3. Traces. A trace of an odd number of gamma matrices vanishes.
tr_two = trace([gamma(0), gamma(1)])
print("Tr(gamma^0 gamma^1) =", tr_two)
assert tr_two == 4 * g(0, 1)
assert trace([gamma(0), gamma(1), gamma(2)]) == 0

# The trace of four gamma matrices, contracted with a metric, gives D times
# a metric.
tr_four = trace([gamma(0), gamma(1), gamma(2), gamma(3)])
print("Tr(gamma^0 gamma^1 gamma^2 gamma^3) =", tr_four)
contracted_trace = (g(0, 1) * tr_four).simplify()
print("g(0, 1) Tr(...) simplified =", contracted_trace)
assert contracted_trace == 4 * D * g(2, 3)

# 4. Epsilon contraction: two epsilons with one index in common reduce to
#    metrics. This holds in 4 dimensions, so it is evaluated with
#    keep=False and simplified before comparison.
with symbolic_dimension(keep=False):
    contracted = (eps(0, 1, 2, 3) * eps(0, 1, 2, 4)).simplify()
    print("eps(0,1,2,3) eps(0,1,2,4) =", contracted)
    assert contracted == -6 * g(3, 4)

# 5. Currents and simplification. current() builds a chain of gamma matrices
#    between two Dirac indices. Dirac and Lorentz indices are distinct, so
#    they never collide. simplify() and order() return new expressions.
chain = current([gamma(0), gamma(1), gamma(2)], 5, 6)
print("current =", chain)
print("simplified =", chain.simplify())
print("ordered =", chain.order())
assert chain.order().order() == chain.order()

# Anticommutation: gamma^0 gamma^1 + gamma^1 gamma^0 = 2 g^{01}, so the sum
# reduces to a metric times the identity current.
anticommutator = current([gamma(0), gamma(1)], 0, 1) + current(
    [gamma(1), gamma(0)], 0, 1
)
print("gamma^0 gamma^1 + gamma^1 gamma^0 ordered =", anticommutator.order())
assert anticommutator.order() == 2 * g(0, 1) * current([], 0, 1)

# Chains containing gamma5 are ordered and projected in the same way.
with_gamma5 = current([gamma(0), gamma5(), gamma(1), gamma(2)], 0, 1)
ordered_gamma5 = with_gamma5.order()
print("ordered with gamma5 =", ordered_gamma5)
assert ordered_gamma5.order() == ordered_gamma5

# 6. Projection on a basis of Dirac structures.
projected = chain.project(FierzBasis.CHIRAL)
print("projected on the chiral basis =", projected)
print(
    "gamma5 chain projected on the chiral basis =",
    ordered_gamma5.project(FierzBasis.CHIRAL),
)

# 7. Fierz identities. single_fierz and double_fierz take two currents and
#    return a named result: ``rhs`` is the rearranged form, ``lhs`` the
#    product of the inputs before rearrangement.
left = current([gamma(0)], 1, 2)
right = current([gamma(0)], 3, 4)
once = single_fierz(left, right, basis=FierzBasis.STANDARD)
print("single Fierz, rhs =", once.rhs)
print("single Fierz, lhs =", once.lhs)
assert once.lhs == left * right

twice = double_fierz(left, right)
print("double Fierz, rhs =", twice.rhs)
assert twice.lhs == left * right

# Double Fierz identities are checked in 4 dimensions, where the
# expressions reduce to a short form.
with symbolic_dimension(keep=False):
    projectors = double_fierz(current([P_L()], 1, 2), current([P_R()], 3, 4))
    assert projectors.rhs.simplify() == projectors.lhs

    # Well-known identity: gamma^a gamma^b gamma^c P_L x gamma_c gamma_b gamma_a P_R
    # equals 16 gamma^a P_L x gamma_a P_R.
    rearranged = double_fierz(
        current([gamma(0), gamma(1), gamma(2), P_L()], 1, 2),
        current([gamma(2), gamma(1), gamma(0), P_R()], 3, 4),
    )
    expected = 16 * current([gamma(0), P_L()], 1, 2) * current([gamma(0), P_R()], 3, 4)
    print("well-known double Fierz, 4D rhs =", rearranged.rhs.simplify())
    assert rearranged.rhs.simplify() == expected.simplify()

# 8. Dimension. By default the Dirac algebra keeps the dimension symbolic.
#    ``symbolic_dimension`` sets it for a block and restores it on exit.
print("g(0, 0) symbolic =", g(0, 0))
print("g(0, 0) with keep=False =", g(0, 0))
assert g(0, 0) == D  # the metric trace stays D; only epsilon contractions use 4D

# 9. Output. Expressions print as text and have a LaTeX form for notebooks.
print("LaTeX of the trace:", tr_two.to_latex())

# 10. Errors. Invalid input raises a MartyValueError from pymarty.errors.
try:
    gamma(-1)
except MartyValueError as error:
    print("gamma(-1) raises:", error)
else:
    raise AssertionError("a negative index must be rejected")
