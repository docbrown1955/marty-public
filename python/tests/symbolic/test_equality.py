"""Equality, hashing and structural keys of the symbolic API."""

from fractions import Fraction

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError
from pymarty.symbolic import Expr


@pytest.mark.unit
def test_numbers_compare_by_value() -> None:
    assert Expr(Fraction(-9, -3)) == 3
    assert Expr(3) != 2.5
    assert Expr(3) != 2
    assert Expr(Fraction(3, 2)) == Expr(Fraction(6, 4))
    assert Expr(Fraction(3, 2)) != Expr(Fraction(5, 4))
    assert Expr(0.0) == Expr(0j)


@pytest.mark.unit
def test_constant_and_variable_of_same_name_differ() -> None:
    x = symbolic.variable("x")
    assert symbolic.constant("a") != symbolic.variable("a")
    assert x != symbolic.variable("x")


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EqualityDocumentedSemantics")
def test_equality_follows_documented_semantics() -> None:
    x = symbolic.variable("x")
    assert Expr(3) == Expr(3.0)
    assert x == x
    assert x != 0
    assert symbolic.I != 1


@pytest.mark.unit
def test_hash_of_symbols_and_numbers_in_a_set() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("x")
    assert len({x, x}) == 1
    assert len({x, y}) == 2
    assert len({Expr(1), Expr(1.0), Expr(1)}) == 1


@pytest.mark.unit
def test_number_hash_matches_equality() -> None:
    values = [Expr(3), Expr(3.0), Expr(Fraction(3, 1))]
    assert len({hash(v) for v in values}) == 1
    assert hash(Expr(3)) == hash(3)


@pytest.mark.unit
def test_hash_of_compound_expression_raises() -> None:
    x = symbolic.variable("x")
    with pytest.raises(MartyTypeError):
        hash(x + 1)
    with pytest.raises(TypeError):
        hash(x * x)


@pytest.mark.unit
def test_structural_key_is_order_independent() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (x + y).structural_key() == (y + x).structural_key()
    assert (x * y).structural_key() == (y * x).structural_key()
