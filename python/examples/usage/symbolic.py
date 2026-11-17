"""Usage example: symbolic expressions.

``pymarty.symbolic`` gives access to CSL expressions (``Expr``), with
arithmetic, mathematical functions, transformations, numeric evaluation,
and the index and tensor objects used by later phases. Every object is
immutable: a method returns a new expression and never changes its input.

Every step prints what it shows and asserts the result, so the example fails
as soon as the library stops doing what is written here.

Run it with ``python symbolic.py``. It is also run by the test suite
(see ``docs/architecture/pymarty/testing-strategy.md``, section 1.2).
"""

import gc
from fractions import Fraction

from pymarty.symbolic import (
    Expr,
    Minkowski,
    Space,
    constant,
    delta,
    epsilon,
    metric,
    relabel,
    sqrt,
    tensor,
    variable,
)

from pymarty import symbolic
from pymarty.errors import MartyError, MartyValueError, MartyZeroDivisionError

# 1. Numbers and arithmetic. Python numbers mix with Expr on either side, and
#    like terms are combined directly.
x = variable("x")
y = variable("y")
total = 2 * x + 3 * x
print("2x + 3x =", total)
assert total == 5 * x

mixed = 2 * x + 3 * x - x / 2
print("2x + 3x - x/2 =", mixed)
assert mixed == Fraction(9, 2) * x

# 2. Functions take and return Expr. Perfect powers and special values are
#    exact.
print("sqrt(4) =", sqrt(4))
assert sqrt(4) == 2
print("exp(0) =", symbolic.exp(0))
assert symbolic.exp(0) == 1
print("log(1) =", symbolic.log(1))
assert symbolic.log(1) == 0
print("sin(0) =", symbolic.sin(0))
assert symbolic.sin(0) == 0

# 3. Transformations return a new expression. The input is not changed.
binomial = (x + y) ** 2
expanded = binomial.expand()
print("(x + y)^2 expanded:", expanded)
assert expanded == x**2 + 2 * x * y + y**2
print("original still:", binomial)
assert binomial == (x + y) ** 2
assert binomial != expanded

# 3b. Factoring and collecting. Both return a new expression.
a = constant("a")
b = constant("b")
factored = (a * x + a * y).factor()
print("a x + a y factored:", factored)
assert factored == a * (x + y)
collected = (a * x + b * x + y).collect([x])
print("a x + b x + y collected in x:", collected)
assert collected == (a + b) * x + y

# 4. Derivatives and substitution. subs hands all the replacements to CSL
#    Replace, which makes a single bottom-up pass: a symbol that was just
#    substituted is not visited again.
f = x**3 + 2 * x * y
derivative = f.diff(x)
print("df/dx =", derivative)
assert derivative == 3 * x**2 + 2 * y
assert f.diff(y) == 2 * x

# Swapping two symbols works with this single pass.
swapped = (x + 2 * y).subs({x: y, y: x})
print("swap x and y in x + 2y:", swapped)
assert swapped == y + 2 * x

shifted = f.subs({x: y + 1})
print("subs x -> y + 1:", shifted)
assert (shifted - ((y + 1) ** 3 + 2 * (y + 1) * y)).expand() == 0

# 5. Numeric values. evaluate(**values) returns a new expression. complex()
#    and float() need an expression without free symbols.
value = (x**2 + 1).evaluate(x=3)
print("(x^2 + 1) at x = 3:", complex(value))
assert complex(value) == 10
assert float(Expr(Fraction(1, 4))) == 0.25
print("free symbols of f:", sorted(s.name for s in f.free_symbols))
assert f.free_symbols == {x, y}
assert f.depends_on(x)
assert not Expr(3).depends_on(x)

# Free symbols are left in place by evaluate. Only complex() and float() need
# a complete set of values.
partial = f.evaluate(x=2)
print("f at x = 2:", partial)
assert partial == 8 + 4 * y
assert partial.free_symbols == {y}

# Complex parts. Symbols are real by default.
assert x.conjugate() == x
assert x.real_part() == x
assert x.imag_part() == 0
assert symbolic.I.conjugate() == -symbolic.I
assert Expr(1 + 2j).conjugate() == Expr(1 - 2j)

# Symbols and numbers are hashable. A sum, product or function is not: use
# structural_key() for those.
assert {x: 1}[x] == 1
assert hash(Expr(2)) == hash(Expr(2))
try:
    hash(x + y)
    raise AssertionError("a sum must not be hashable")
except TypeError:
    pass
assert (x + y).structural_key() == (y + x).structural_key()

# 6. Printing. str() is CSL text, to_latex() is the LaTeX form used by the
#    C++ output.
latex = (x / y).to_latex()
print("LaTeX of x/y:", latex)
assert latex == r"\frac{x}{y}"
assert str(x) == "x"

# 7. Errors. Everything pymarty raises derives from MartyError. Asking for a
#    number while a symbol is still free is a MartyValueError, which is also
#    a ValueError.
try:
    complex(x)
    raise AssertionError("complex(x) must fail while x is free")
except MartyError as error:
    print(f"caught {type(error).__name__}: {error}")
    assert isinstance(error, MartyValueError)
    assert "x" in str(error)

try:
    complex(x)
    raise AssertionError("complex(x) must fail while x is free")
except ValueError:
    print("also caught as a plain ValueError")

assert issubclass(MartyValueError, ValueError)

# 8. Powers with a zero base and a negative exponent raise instead of stopping
#    the process, and so does an infinite base. The library works afterwards.
try:
    _ = Expr(0) ** -1
    raise AssertionError("0 ** -1 must fail")
except MartyZeroDivisionError as error:
    print(f"caught {type(error).__name__}: {error}")
    assert isinstance(error, (MartyError, ZeroDivisionError))

try:
    _ = symbolic.oo**2
    raise AssertionError("oo ** 2 must fail")
except MartyValueError as error:
    print(f"caught {type(error).__name__}: {error}")
assert Expr(2) ** 3 == 8

# A division by zero is not an error in CSL: it gives infinity (and CSL writes
# a warning on stderr).
assert Expr(5) / 0 == symbolic.oo

# 9. Equality is CSL identity. Two calls to constant("a") make two different
#    symbols, even though they print the same.
a1 = constant("a")
a2 = constant("a")
print("same object:", a1 == a1)
assert a1 == a1
print("two constants named a:", a1 == a2)
assert a1 != a2
assert str(a1) == str(a2)


# 10. Spaces and indices. An Index keeps its Space alive, so it can outlive
#     the scope that created the space.
def make_index():
    space = Space("E4", 4)
    return space.index("mu")


lifetime_index = make_index()
gc.collect()
print("index still usable after its scope:", lifetime_index.name)
assert lifetime_index.name == "mu"
assert lifetime_index.space.dimension == 4

space = Space("S", 4, index_names=("alpha", "beta"))
mu = space.index("mu")
nu = space.index("nu")

# A list of indices: five indices on a space with two default names are five
# distinct indices, and they can be used by position in expressions.
indices = space.indices(5)
assert len(indices) == 5
assert len(set(indices)) == 5
assert indices[4].space is space

# A fixed index is a component number, checked against the dimension.
assert space.fixed_index(1).space is space
try:
    space.fixed_index(4)
    raise AssertionError("component 4 does not exist in dimension 4")
except MartyValueError as error:
    print(f"caught {type(error).__name__}: {error}")

# 11. Tensors. A repeated index contracts in a product, and relabel gives the
#     canonical names of the space.
tx = tensor("x", [space])
ty = tensor("y", [space])
tz = tensor("z", [space])
contracted = tx(mu) * ty(mu)
print("x_mu y_mu =", relabel(contracted))
assert relabel(contracted).to_latex() == "x_{alpha}y_{alpha}"
assert contracted.free_indices == ()
assert len(contracted.dummy_indices) == 1
assert tx(mu).free_indices[0].name == "mu"
assert relabel(tx(mu) + ty(mu)).to_latex() == "x_{alpha} + y_{alpha}"

# Free indices take the first names, dummy indices the following ones.
mixed_names = tx(mu) * ty(mu) * tz(nu)
assert relabel(mixed_names).to_latex() == "x_{beta}y_{beta}z_{alpha}"

# Fixed indices are not renamed, and relabel does not change its input.
assert relabel(tx(space.fixed_index(1))).to_latex() == "x_{1}"
assert contracted.dummy_indices[0].name == "mu"

# Symmetries: full, or by pair of index positions.
sym = tensor("S", [space, space], symmetric=True)
anti = tensor("A", [space, space], antisymmetric=True)
assert sym(mu, nu) == sym(nu, mu)
assert anti(mu, nu) == -anti(nu, mu)
try:
    tensor("B", [space, space], symmetric=True, antisymmetric=True)
    raise AssertionError("a tensor cannot be both symmetric and antisymmetric")
except MartyValueError as error:
    print(f"caught {type(error).__name__}: {error}")

# Kronecker delta, metric and epsilon take their space from the indices.
latex_space = Space("L", 4, index_names=(r"alpha;\alpha", r"beta;\beta"))
la = latex_space.index("alpha")
lb = latex_space.index("beta")
assert delta(la, lb).to_latex() == r"\delta_{\alpha\beta}"

mink_mu = Minkowski.index(r"\mu")
mink_nu = Minkowski.index(r"\nu")
assert metric(mink_mu, mink_nu).to_latex() == r"g^{\mu\nu}"

# In a signed space a contraction needs one index of each sign.
tv = tensor("v", [Minkowski])
tw = tensor("w", [Minkowski])
try:
    _ = tv(mink_mu) * tw(mink_mu)
    raise AssertionError("same-sign contraction must fail in Minkowski")
except MartyValueError as error:
    print(f"caught {type(error).__name__}: {error}")
vw = tv(mink_mu) * tw(mink_mu.flipped())
assert vw.free_indices == ()

a4, b4, c4, d4 = space.indices(4)
eps = epsilon(a4, b4, c4, d4)
assert eps != 0
assert epsilon(b4, a4, c4, d4) == -eps
assert epsilon(a4, a4, c4, d4) == 0

# 12. Immutability. The handle is never mutated, even when a transformation
#     is applied to a shared sub-expression.
shared = x + y
result = (shared * shared).expand()
print("shared sub-expression unchanged:", shared)
assert shared == x + y
assert result == x**2 + 2 * x * y + y**2
assert result is not shared
