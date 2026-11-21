"""Operators and symbols of the symbolic API."""

import pytest

from pymarty import symbolic
from pymarty._pymarty import _debug
from pymarty.errors import MartyValueError, MartyZeroDivisionError
from pymarty.symbolic import Expr


@pytest.mark.unit
def test_sum_and_product_equality_is_canonical() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert 2 + a + 1 == a + 3
    assert 2 + a + 1 != a + 2
    assert a + b + 2 * x == 3 * x + b - x + a
    assert (2 * x * (x * x) * y) / y == 2 * x**3
    assert (2 * x * (x * x) * y) / y * y != 2 * x**3


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.DivisionByZeroGivesInfinity")
def test_division_by_zero_gives_infinity() -> None:
    assert Expr(1) / Expr(0) == symbolic.infinity
    assert 1 / Expr(0) == symbolic.infinity


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ZeroPowerNegativeRaises")
def test_zero_to_negative_power_raises() -> None:
    with pytest.raises(MartyZeroDivisionError):
        Expr(0) ** -1
    with pytest.raises(MartyValueError):
        symbolic.infinity**2


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ConstantAcceptsAnyName")
def test_constant_and_variable_accept_any_name() -> None:
    symbolic.constant("c")
    symbolic.constant("weird name 1+2")
    symbolic.variable("v")
    symbolic.variable("weird name 1+2")


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.SameNameDifferentSymbol")
def test_same_name_gives_different_symbol() -> None:
    a = symbolic.variable("x")
    b = symbolic.variable("x")
    assert a == a
    assert a != b
    assert a - b != 0


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.SumProductPowerCanonicalisation")
def test_sum_product_power_are_canonical() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert x + x == 2 * x
    assert x * x == x**2
    assert x + y == y + x
    assert x * y == y * x
    assert x - x == 0
    assert x * 1 == x
    assert x + 0 == x


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.OperationsLeaveGlobalOptionsUntouched")
def test_operations_leave_global_state_untouched() -> None:
    before = _debug.state_snapshot()
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    _ = x + y
    _ = x * y
    _ = x / y
    _ = x**2
    _ = symbolic.sin(x)
    _ = x == y
    _ = Expr(1) / Expr(0)
    assert _debug.state_snapshot() == before
