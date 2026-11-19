"""Multi-step scenarios that combine several parts of the symbolic interface.

Each test reads like a paper exercise: build an expression, transform it with
several methods in sequence, then check the outcome against a hand computation
or a numerical reference.
"""

import math

import pytest

from pymarty import symbolic
from pymarty.errors import MartyValueError
from pymarty.symbolic import (
    ComplexProperty,
    Expr,
    ExprType,
    I,
    Minkowski,
    PrimaryType,
    exp,
    sin,
    tensor,
    tensor_field,
)
from pymarty.symbolic_algorithms import (
    distribute_if,
    find_if_node,
)


@pytest.mark.unit
def test_chain_rule_through_replace_matches_direct_derivative() -> None:
    # h(u) = u sin u, u = x^2 + 1. Chain rule: d/dx h(x^2+1) = 2x h'(x^2+1)
    # with h'(u) = sin u + u cos u. Build it by differentiating in u, then
    # substituting, and compare with a direct differentiation in x.
    x = symbolic.variable("x")
    u = symbolic.variable("u")
    dh_du = (u * sin(u)).diff(u)
    via_chain = dh_du.replace({u: x**2 + 1}) * 2 * x
    direct = ((x**2 + 1) * sin(x**2 + 1)).diff(x)
    for point in (0.0, 0.5, 1.3):
        assert float(via_chain.evaluate({x: point})) == pytest.approx(
            float(direct.evaluate({x: point})), abs=1e-12
        )
    assert via_chain.free_symbols == frozenset({x})


@pytest.mark.unit
def test_taylor_polynomial_of_sin_times_exp_matches_function_near_zero() -> None:
    # f = e^x sin x. Derivatives at 0: f=0, f'=1, f''=2, f'''=2.
    # Build them with chained diff, assemble the cubic Taylor polynomial,
    # then check it against f at a small point and its derivative values.
    x = symbolic.variable("x")
    f = exp(x) * sin(x)
    d1 = f.diff(x)
    d2 = d1.diff(x)
    d3 = d2.diff(x)
    coefficients = [
        f.evaluate({x: 0}),
        d1.evaluate({x: 0}),
        d2.evaluate({x: 0}),
        d3.evaluate({x: 0}),
    ]
    assert [float(c) for c in coefficients] == pytest.approx([0, 1, 2, 2])
    taylor = x * 0
    for k, c in enumerate(coefficients):
        taylor = taylor + (float(c) / math.factorial(k)) * x**k
    point = 0.1
    assert float(taylor.evaluate({x: point})) == pytest.approx(
        float(f.evaluate({x: point})), abs=1e-5
    )


@pytest.mark.unit
def test_distribute_then_expand_agrees_with_full_expansion_numerically() -> None:
    # (a + b) (x + y) x = a x^2 + a x y + b x^2 + b x y. Only one sum factor
    # other than the matched one, so distribute_if must give the exact result.
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    expr = (a + b) * (x + y) * x
    partly = distribute_if(expr, lambda factor: factor == a + b)
    full = expr.expand()
    assert partly.expand() == full
    assert full == a * x**2 + a * x * y + b * x**2 + b * x * y
    assert full.type == ExprType.Sum
    values: dict[object, object] = {a: 2, b: -1, x: 3, y: 0.5}
    assert float(full.evaluate(values)) == pytest.approx(
        float(expr.evaluate(values)), abs=1e-12
    )


@pytest.mark.unit
def test_distribute_with_two_other_sums_keeps_the_value() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    expr = (a + b) * (x + y) * (x - y)
    partly = distribute_if(expr, lambda factor: factor == a + b)
    assert partly.expand() == expr.expand()


@pytest.mark.unit
def test_abbreviation_survives_copy_refresh_and_evaluation() -> None:
    # K = x^2 + 1 is reused in a rational-looking expression. Copying and
    # refreshing (deep and shallow) must not change its value or its symbols.
    x = symbolic.variable("x")
    K = symbolic.make_abbreviation("K", x**2 + 1)
    expr = K * K + 2 * K
    snapshots = [
        expr.copy(),
        expr.copy(deep=True),
        expr.refresh(),
        expr.refresh(deep=True),
    ]
    for point in (0, 1, -2):
        expected = (point**2 + 1) ** 2 + 2 * (point**2 + 1)
        assert float(expr.evaluate({x: point})) == pytest.approx(expected)
        for snapshot in snapshots:
            assert float(snapshot.evaluate({x: point})) == pytest.approx(expected)
    assert expr.free_symbols == frozenset({K})
    assert all(s.free_symbols == expr.free_symbols for s in snapshots)


@pytest.mark.unit
def test_complex_square_real_and_imaginary_parts_and_modulus() -> None:
    # z = x + i y, x and y real. Then z^2 = (x^2 - y^2) + i 2xy and
    # |z|^4 = (z z*)^2 = (x^2 + y^2)^2.
    x = symbolic.variable("x", complex_property=ComplexProperty.Real)
    y = symbolic.variable("y", complex_property=ComplexProperty.Real)
    z = x + I * y
    square = (z**2).expand()
    assert square.real_part().expand() == x**2 - y**2
    assert square.imag_part().expand() == 2 * x * y
    modulus_squared = (z * z.conjugate()).expand()
    assert modulus_squared == x**2 + y**2
    assert (modulus_squared**2).expand() == ((x**2 + y**2) ** 2).expand()
    assert complex((z**2).evaluate({x: 1, y: 1})) == pytest.approx(2j)
    assert complex((1 + I) ** 2) == pytest.approx(2j)


@pytest.mark.unit
def test_three_index_chain_has_no_free_indices_and_keeps_dummies() -> None:
    # T = A_{mu nu} B^{nu}_{rho} C^{rho mu}: a closed chain, so every index is
    # dummy. Opening it by relabelling one free index must keep the contraction.
    A = tensor("A", [Minkowski, Minkowski])
    B = tensor("B", [Minkowski, Minkowski])
    C = tensor("C", [Minkowski, Minkowski])
    mu, nu, rho, sigma = Minkowski.indices(4)
    closed = A(mu, nu) * B(~nu, rho) * C(~rho, ~mu)
    assert closed.free_indices == ()
    assert {i.name for i in closed.dummy_indices} == {mu.name, nu.name, rho.name}
    open_chain = A(mu, sigma) * B(~sigma, rho) * C(~rho, ~mu)
    assert [i.name for i in open_chain.free_indices] == []
    partial = A(mu, nu) * B(~nu, rho)
    assert {i.name for i in partial.free_indices} == {mu.name, rho.name}
    relabelled = partial.relabel_indices({rho: sigma})
    assert {i.name for i in relabelled.free_indices} == {mu.name, sigma.name}


@pytest.mark.unit
def test_index_sign_toggle_is_a_contraction_convention() -> None:
    # A_mu B^mu is the same contraction as A^mu B_mu, but A_mu B_mu is not a
    # contraction: the two indices must carry opposite signs.
    A = tensor("A", [Minkowski])
    B = tensor("B", [Minkowski])
    mu = Minkowski.indices(1)[0]
    assert A(mu) * B(~mu) == A(~mu) * B(mu)
    with pytest.raises(MartyValueError):
        A(mu) * B(mu)
    assert (~(~mu)) == mu
    assert [i.name for i in (A(mu) * B(~mu)).dummy_indices] == [mu.name]


@pytest.mark.unit
def test_tensor_field_parent_identity_survives_copies_and_products() -> None:
    # phi(mu, X) and phi(nu, X) share a parent (the field phi at point X).
    # A copy of a factor keeps it, and a product with a tensor keeps the
    # field's parent identity distinct from the tensor's.
    phi = tensor_field("phi", Minkowski, [Minkowski])
    X = tensor("X", [Minkowski])
    A = tensor("A", [Minkowski])
    mu, nu = Minkowski.indices(2)
    field_mu = phi(mu, X)
    field_nu = phi(nu, X)
    assert field_mu.primary_type == PrimaryType.Field
    assert field_mu.parent_id == field_nu.parent_id
    assert field_mu.copy(deep=True).parent_id == field_mu.parent_id
    assert A(mu).parent_id != field_mu.parent_id
    product = field_mu * A(~mu)
    assert product.free_indices == ()
    assert [i.name for i in product.dummy_indices] == [mu.name]
    assert product.refresh(deep=True) == product


@pytest.mark.unit
def test_pipeline_of_queries_locates_power_bases_after_expansion() -> None:
    # p = (x + y)^2 (x - 1) + sin(x y). Before expanding, a Pow with a Sum
    # base exists. After expanding the polynomial part, no such node remains,
    # while sin(x y) is still there as a leaf-level function of variables.
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    p = (x + y) ** 2 * (x - 1) + sin(x * y)

    def pow_of_sum(node: Expr) -> bool:
        return node.type == ExprType.Pow and node.args[0].type == ExprType.Sum

    assert find_if_node(p, pow_of_sum) is not None
    expanded = p.expand()
    assert find_if_node(expanded, pow_of_sum) is None
    assert find_if_node(expanded, lambda n: n.type == ExprType.Sin) is not None
    assert expanded.evaluate({x: 2, y: 0}) == pytest.approx(
        float(p.evaluate({x: 2, y: 0}))
    )


@pytest.mark.unit
def test_derivative_matches_central_difference_for_composed_function() -> None:
    # g(x) = exp(sin x) with g'(x) = cos x exp(sin x). Compare the symbolic
    # derivative after replace-based substitution with a numerical estimate.
    x = symbolic.variable("x")
    t = symbolic.variable("t")
    g = exp(sin(t))
    dg_dt = g.diff(t)
    dg_dx = dg_dt.replace({t: x})
    h = 1e-6
    for point in (0.3, 1.1):
        numerical = (math.exp(math.sin(point + h)) - math.exp(math.sin(point - h))) / (
            2 * h
        )
        assert float(dg_dx.evaluate({x: point})) == pytest.approx(numerical, rel=1e-6)
    assert dg_dx.free_symbols == frozenset({x})
    assert dg_dt.free_symbols == frozenset({t})


@pytest.mark.unit
def test_relabelling_a_dummy_index_gives_the_renamed_contraction() -> None:
    A = tensor("A", [Minkowski, Minkowski])
    B = tensor("B", [Minkowski])
    mu, nu, rho = Minkowski.indices(3)
    contraction = A(mu, nu) * B(mu.flipped())
    renamed = contraction.relabel_indices({mu: rho, mu.flipped(): rho.flipped()})
    (dummy,) = renamed.dummy_indices
    (free,) = renamed.free_indices
    assert renamed == A(dummy, free) * B(dummy.flipped())
