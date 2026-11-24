"""Port of tests/system/src/EWGauge.cpp: tree-level W Z -> W Z squared amplitudes.

Only the symbolic amplitudes and the subset squared amplitudes are computed
here. The quadruple-precision library, the numerical values and the comparison
with the reference data belong to phase 6.
"""

import pytest

from pymarty.calculation import (
    Order,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.modeling.models.sm import SM
from pymarty.symbolic import Expr


@pytest.fixture(scope="module")
def model() -> SM:
    return SM()


def test_ew_gauge_subset_squared_amplitudes(model: SM) -> None:
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("W"), incoming("Z"), outgoing("W"), outgoing("Z")],
    )
    assert amplitude.diagrams

    no_higgs = amplitude.filter(lambda d: "h" not in d.particles)
    triple_gauge = no_higgs.filter(lambda d: "W" in d.mediators or "Gp" in d.mediators)
    quartic = no_higgs.filter(
        lambda d: "W" not in d.mediators and "Gp" not in d.mediators
    )
    assert len(triple_gauge.diagrams) + len(quartic.diagrams) == len(no_higgs.diagrams)
    assert quartic.diagrams

    for subset in (amplitude, no_higgs, triple_gauge, quartic):
        assert isinstance(compute_squared_amplitude(model, subset), Expr)
