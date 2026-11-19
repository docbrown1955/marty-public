"""Multi-step scenarios that combine several parts of the symbolic interface.

Each test reads like a short paper exercise: build an expression, transform it
with several methods, then check the result against a hand computation.
"""

import pytest

from pymarty import symbolic
from pymarty.symbolic import (
    ComplexProperty,
    ExprType,
    I,
    Minkowski,
    PrimaryType,
    cos,
    exp,
    pi,
    sin,
    tensor,
    tensor_field,
)
from pymarty.symbolic_algorithms import (
    all_of_leafs,
    distribute_if,
    find_if_leaf,
    find_if_node,
)


@pytest.mark.unit
def test_second_derivative_of_x_sin_x() -> None:
    # (x sin x)'' = 2 cos x - x sin x
    x = symbolic.variable("x")
    second = (x * sin(x)).diff(x).diff(x)
    assert second.expand() == (2 * cos(x) - x * sin(x)).expand()


@pytest.mark.unit
def test_taylor_coefficients_of_x_exp_x_at_zero() -> None:
    # f = x e^x: f(0) = 0, f'(0) = 1, f''(0) = 2
    x = symbolic.variable("x")
    f = x * exp(x)
    assert f.evaluate({x: 0}) == 0
    assert f.diff(x).evaluate({x: 0}) == 1
    assert f.diff(x).diff(x).evaluate({x: 0}) == 2


@pytest.mark.unit
def test_binomial_cube_expands_to_pascal_row_and_matches_values() -> None:
    # (x + y)^3 = x^3 + 3 x^2 y + 3 x y^2 + y^3
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    expanded = ((x + y) ** 3).expand()
    assert expanded == x**3 + 3 * x**2 * y + 3 * x * y**2 + y**3
    assert float(expanded.evaluate({x: 1, y: 1})) == 8.0
    assert float(expanded.evaluate({x: 2, y: -1})) == 1.0


@pytest.mark.unit
def test_shift_polynomial_variable_with_replace_then_expand() -> None:
    # p(x) = x^3 - 2x + 1 with x = y + 1 gives y^3 + 3y^2 + y
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    p = x**3 - 2 * x + 1
    shifted = p.replace({x: y + 1}).expand()
    assert shifted == (y**3 + 3 * y**2 + y).expand()
    assert shifted.free_symbols == frozenset({y})


@pytest.mark.unit
def test_complex_number_algebra_with_real_symbols() -> None:
    # z = x + i y with real x, y: |z|^2 = z z* = x^2 + y^2
    x = symbolic.variable("x", complex_property=ComplexProperty.Real)
    y = symbolic.variable("y", complex_property=ComplexProperty.Real)
    z = x + I * y
    assert x.is_real and y.is_real
    assert (I * I) == -1
    assert (z * z.conjugate()).expand() == x**2 + y**2
    assert z.real_part().expand() == x
    assert z.imag_part().expand() == y
    assert complex((1 + I) ** 2) == pytest.approx(2j)
    assert float(sin(pi / 2).evaluate()) == pytest.approx(1.0)


@pytest.mark.unit
def test_tensor_contraction_free_and_dummy_indices() -> None:
    # A_{mu nu} B^{mu} has free index nu and dummy index mu
    A = tensor("A", [Minkowski, Minkowski])
    B = tensor("B", [Minkowski])
    mu, nu, _ = Minkowski.indices(3)
    contraction = A(mu, nu) * B(mu.flipped())
    assert [i.name for i in contraction.free_indices] == [nu.name]
    assert [i.name for i in contraction.dummy_indices] == [mu.name]


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="relabel_indices renames a dummy index to an index with id 0 "
    "(C++ RelabelIndices); see the gap report",
)
def test_relabelling_a_dummy_index_gives_the_renamed_contraction() -> None:
    A = tensor("A", [Minkowski, Minkowski])
    B = tensor("B", [Minkowski])
    mu, nu, rho = Minkowski.indices(3)
    contraction = A(mu, nu) * B(mu.flipped())
    renamed = contraction.relabel_indices({mu: rho, mu.flipped(): rho.flipped()})
    assert renamed == A(rho.flipped(), nu) * B(rho)


@pytest.mark.unit
def test_tensor_field_parents_are_shared_across_index_values() -> None:
    phi = tensor_field("phi", Minkowski, [Minkowski])
    X = tensor("X", [Minkowski])
    A = tensor("A", [Minkowski])
    mu, nu = Minkowski.indices(2)
    first = phi(mu, X)
    second = phi(nu, X)
    assert first.primary_type == PrimaryType.Field
    assert first.parent_id is not None
    assert first.parent_id == second.parent_id
    assert first.parent_id != A(mu).parent_id
    assert [i.name for i in first.free_indices] == [mu.name]


@pytest.mark.unit
def test_abbreviation_stands_for_its_definition_in_evaluation() -> None:
    x = symbolic.variable("x")
    K = symbolic.make_abbreviation("K", x**2 + 1)
    expr = K * K + 2 * K
    # K = x^2 + 1, so expr = (x^2 + 1)^2 + 2 (x^2 + 1)
    assert expr.evaluate({x: 1}) == 8
    assert expr.evaluate({x: 0}) == 3


@pytest.mark.unit
def test_distribute_factor_over_sums_and_locate_terms() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    expr = a * (x + y) * x
    result = distribute_if(expr, lambda f: f == a)
    assert result.type == ExprType.Prod
    assert find_if_node(result, lambda node: node.type == ExprType.Sum) is not None
    assert result.expand() == expr.expand()
    assert all_of_leafs(result, lambda leaf: leaf.type == ExprType.Variable)
    assert find_if_leaf(expr + b, lambda leaf: leaf == b) == b
    assert find_if_node(result, lambda node: node.type == ExprType.Sum) is not None


@pytest.mark.unit
def test_structural_hash_ignores_copies_but_not_content() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    z = symbolic.variable("z")
    product = x * y
    keys = {
        product.structural_key(),
        (y * x).structural_key(),
        product.copy().structural_key(),
        product.refresh(deep=True).structural_key(),
    }
    assert len(keys) == 1
    assert (x * z).structural_key() != product.structural_key()
    assert product.copy(deep=True) is not product
