import pytest


def test_basis():
    from pymarty.gamma_api import FierzBasis

    assert FierzBasis.Chiral != FierzBasis.Standard


def _test_same(expr) -> None:
    assert expr == expr
    assert not (expr != expr)
    assert not (expr < expr)
    assert expr <= expr
    assert not (expr > expr)
    assert expr >= expr


def _test_diff(left, right) -> None:
    assert not (left == right)
    assert left != right
    assert left < right
    assert left <= right
    assert not (left > right)
    assert not (left >= right)


def _test_zero_len(expr) -> None:
    assert len(expr) == 0
    with pytest.raises(IndexError):
        expr[0]
    with pytest.raises(IndexError):
        expr[-1]
    with pytest.raises(IndexError):
        expr[1]


def test_simple_expr():
    from pymarty.gamma_api import Expr, D

    zero: Expr = Expr(0)
    one_half: Expr = Expr(1.5)
    four: Expr = D

    _test_same(zero)
    _test_same(one_half)
    _test_same(four)
    _test_diff(zero, one_half)
    _test_diff(zero, four)
    _test_diff(one_half, four)

    _test_zero_len(zero)
    _test_zero_len(one_half)
    _test_zero_len(four)
