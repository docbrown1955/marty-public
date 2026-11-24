"""Port of majoranaLoop.cpp: fermion loops with a self-conjugate fermion.

Only the symbolic one-loop amplitudes are computed here. The check of the
simplified diagrams is the one of the C++ test: no free Dirac or Minkowski
index may remain in a diagram.
"""

from collections.abc import Iterator

import pytest

from pymarty.calculation import Order, anti, compute_amplitude, incoming, outgoing
from pymarty.modeling.models.majorana_loop import MajoranaLoop
from pymarty.symbolic import EvalMode, Expr, Index, PrimaryType


@pytest.fixture(scope="module")
def model() -> MajoranaLoop:
    return MajoranaLoop()


def _subexpressions(expr: Expr) -> Iterator[Expr]:
    yield expr
    for arg in expr.args:
        yield from _subexpressions(arg)


def _only_field_indices_remain(expr: Expr) -> bool:
    """True when every free index of expr is carried by a field element."""
    field_indices: set[Index] = set()
    for sub in _subexpressions(expr):
        if sub.primary_type == PrimaryType.Field:
            field_indices.update(sub.free_indices)
    return set(expr.free_indices) <= field_indices


@pytest.mark.loops
@pytest.mark.parametrize("scalar", ["S", "P"])
def test_scalar_pair_one_loop_diagrams_have_no_free_indices(
    model: MajoranaLoop, scalar: str
) -> None:
    amplitude = compute_amplitude(
        model,
        Order.ONE_LOOP,
        [
            incoming(scalar),
            incoming(anti(scalar)),
            outgoing(scalar),
            outgoing(anti(scalar)),
        ],
    )
    assert amplitude.diagrams
    for diagram in amplitude.diagrams:
        value = diagram.expression.evaluate(mode=EvalMode.Abbreviation)
        assert _only_field_indices_remain(value)
