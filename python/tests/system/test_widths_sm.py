"""Port of tests/system/src/widthsSM.cpp: squared amplitudes behind the SM widths.

Only the symbolic squared amplitudes are computed here. The library and the
comparison with the reference widths belong to phase 6.
"""

import pytest

from pymarty.calculation import (
    FeynOptions,
    Insertion,
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    force_particle,
    incoming,
    outgoing,
)
from pymarty.modeling.models.sm import SM
from pymarty.symbolic import Expr


@pytest.fixture(scope="module")
def sm() -> SM:
    return SM()


@pytest.mark.parametrize(
    "legs",
    [
        [incoming("h"), outgoing("W"), outgoing(anti("W"))],
        [incoming("h"), outgoing("Z"), outgoing("Z")],
        [incoming("h"), outgoing("b"), outgoing(anti("b"))],
        [incoming("W"), outgoing("nu_mu"), outgoing(anti("mu"))],
        [incoming("W"), outgoing("c"), outgoing(anti("b"))],
        [incoming("Z"), outgoing("s"), outgoing(anti("s"))],
    ],
    ids=["hWW", "hZZ", "hbb", "Wlnu", "Wcb", "Zss"],
)
def test_tree_level_squared_amplitude_of_a_width(sm: SM, legs: list[Insertion]) -> None:
    amplitude = compute_amplitude(sm, Order.TREE, legs)
    assert amplitude.diagrams
    assert isinstance(compute_squared_amplitude(sm, amplitude), Expr)


@pytest.mark.slow
def test_one_loop_h_to_photons_through_top_loop(sm: SM) -> None:
    amplitude = compute_amplitude(
        sm,
        Order.ONE_LOOP,
        [incoming("h"), outgoing("A"), outgoing(anti("A"))],
        options=FeynOptions(filters=[force_particle("t")]),
    )
    assert amplitude.diagrams
    assert all(any(p.startswith("t_") for p in d.particles) for d in amplitude.diagrams)
    assert isinstance(compute_squared_amplitude(sm, amplitude), Expr)
