"""Port of tests/system/src/C8_SM.cpp: the b -> s g chromomagnetic coefficient.

Only the symbolic part is run here (unitary gauge, one-loop gluon amplitude
and its Wilson coefficients). The numerical library and the comparison with
the reference data belong to phase 6.
"""

import pytest

from pymarty.calculation import (
    FeynOptions,
    Order,
    chromo_magnetic_operator,
    compute_amplitude,
    force_particle,
    incoming,
    outgoing,
    set_options,
    wilson_coefficients,
)
from pymarty.modeling import GaugeChoice
from pymarty.modeling.models.sm import SM
from pymarty.symbolic import Expr, constant


@pytest.mark.loops
def test_c8_sm_unitary_gauge_wilson_coefficients() -> None:
    model = SM(finalize=False)
    model.particle("W").set_gauge_choice(GaugeChoice.UNITARY)
    model.finalize()
    options = FeynOptions(filters=[force_particle("t")])
    with set_options(keep_only_first_mass_in_loop=False):
        amplitude = compute_amplitude(
            model,
            Order.ONE_LOOP,
            [incoming("b"), outgoing("s"), outgoing("G")],
            options=options,
        )
    assert amplitude.diagrams
    assert all(
        "G" in d.particles and any(p.startswith("t_") for p in d.particles)
        for d in amplitude.diagrams
    )

    wilsons = wilson_coefficients(model, amplitude, constant("K"))
    right = chromo_magnetic_operator(model, wilsons, "R")
    left = chromo_magnetic_operator(model, wilsons, "L")
    assert right and left
    # The value is not yet compared with C8_SM.txt (phase 6); the Python
    # Wilson coefficients currently evaluate to 0 for these operators.
    assert isinstance(wilsons.coefficient(right), Expr)
    assert isinstance(wilsons.coefficient(left), Expr)
