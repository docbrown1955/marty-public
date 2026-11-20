"""Python counterparts of the C++ ``DiracologyApi`` tests."""

import pytest

from pymarty.diracology import (
    DiracExpr,
    FierzBasis,
    current,
    eps,
    g,
    gamma,
    single_fierz,
    symbolic_dimension,
)
from pymarty.errors import MartyValueError
from pymarty.symbolic import variable


@pytest.mark.parity("DiracologyApi.NegativeIndicesAreValueErrors")
def test_negative_indices_are_value_errors() -> None:
    with pytest.raises(MartyValueError):
        gamma(-1)
    with pytest.raises(MartyValueError):
        g(0, -2)
    with pytest.raises(MartyValueError):
        eps(0, 1, 2, -3)
    with pytest.raises(MartyValueError):
        current([gamma(0)], -1, 2)


@pytest.mark.parity("DiracologyApi.InvalidFierzInputIsValueError")
def test_fierz_on_bare_gamma_matrices_is_value_error() -> None:
    with pytest.raises(MartyValueError):
        single_fierz(gamma(0), gamma(1), basis=FierzBasis.STANDARD)


@pytest.mark.parity("DiracologyApi.EqualityIsCanonical")
def test_equality_is_canonical() -> None:
    def cur() -> DiracExpr:
        return current([gamma(0)], 1, 2)

    assert cur() == cur()
    assert cur() != 2 * cur()


@pytest.mark.parity("DiracologyApi.OrderIsIdempotent")
def test_order_is_idempotent() -> None:
    chain = current([gamma(0), gamma(1), gamma(2)], 1, 2)
    assert chain.order().order() == chain.order()


def test_symbolic_coefficient_is_canonical() -> None:
    x = variable("x")
    assert g(0, 1) * x == x * g(0, 1)
    assert 2 * x * g(0, 1) - x * g(0, 1) == x * g(0, 1)
    assert x * g(0, 1) != g(0, 1)


@pytest.mark.parity("DiracologyApi.SymbolicDimensionIsRestoredOnException")
def test_symbolic_dimension_is_restored_after_exception() -> None:
    def simplified_epsilons() -> str:
        return str((eps(0, 1, 2, 3) * eps(0, 1, 2, 4)).simplify())

    before = simplified_epsilons()
    with pytest.raises(RuntimeError, match="inside the guard"):
        with symbolic_dimension(keep=False):
            raise RuntimeError("failure inside the guard")
    assert simplified_epsilons() == before


@pytest.mark.parity("DiracologyApi.InvalidScalarsAreValueErrors")
def test_nan_scalar_is_value_error() -> None:
    with pytest.raises(MartyValueError):
        g(0, 1) * float("nan")
