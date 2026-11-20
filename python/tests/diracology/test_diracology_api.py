"""Python counterparts of the C++ ``DiracologyApi`` tests, and Python-only checks."""

import os
import subprocess
import sys
import textwrap

import pytest

from pymarty.diracology import (
    P_L,
    P_R,
    DiracExpr,
    FierzBasis,
    current,
    eps,
    g,
    gamma,
    single_fierz,
    symbolic_dimension,
    trace,
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


@pytest.mark.parity("DiracologyApi.ProjectorAndTraceIdentities")
def test_projector_and_trace_identities() -> None:
    def cur(gammas: list[DiracExpr]) -> DiracExpr:
        return current(gammas, 1, 2)

    assert cur([P_L(), P_L()]).simplify() == cur([P_L()]).simplify()
    assert cur([P_L(), P_R()]).simplify() == 0 * cur([P_L()]).simplify()
    assert trace([gamma(0), gamma(1)]).simplify() == 4 * g(0, 1).simplify()


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


def test_negative_index_message_names_the_index() -> None:
    with pytest.raises(MartyValueError, match="must be non-negative, got -1"):
        gamma(-1)


def test_infinite_scalar_is_value_error() -> None:
    with pytest.raises(MartyValueError, match="must be finite"):
        g(0, 1) * float("inf")


def test_fierz_on_sum_argument_is_value_error() -> None:
    chain = current([gamma(0)], 0, 1) + current([gamma(1)], 0, 1)
    with pytest.raises(MartyValueError, match="not a pure gamma-index current"):
        single_fierz(chain, current([P_L()], 2, 3), basis=FierzBasis.CHIRAL)


def test_trace_of_non_gamma_index_chain_is_runtime_error() -> None:
    with pytest.raises(RuntimeError, match="not a gamma index"):
        trace([g(0, 1)])


def test_zero_comparisons_with_numbers() -> None:
    assert eps(0, 0, 1, 3) == 0
    assert 0 == eps(0, 0, 1, 3)
    assert eps(0, 1, 2, 3) != 0
    assert 0 != eps(0, 1, 2, 3)


def test_import_without_other_pymarty_modules() -> None:
    code = textwrap.dedent(
        """
        import sys

        for name in (
            "pymarty.modeling",
            "pymarty.calculation",
            "pymarty.simulation",
            "pymarty.group_theory",
        ):
            sys.modules[name] = None

        from pymarty.diracology import (
            D, DiracExpr, FierzBasis, I, C, P_L, P_R, current, double_fierz,
            eps, g, gamma, gamma5, single_fierz, symbolic_dimension, trace,
        )

        print(trace([gamma(0), gamma(1)]) == 4 * g(0, 1))
        """
    )
    result = subprocess.run(
        [sys.executable, "-c", code],
        capture_output=True,
        text=True,
        check=False,
        env=os.environ.copy(),
    )
    assert result.returncode == 0, result.stderr
    assert result.stdout.strip() == "True"
