"""Port of tests/system/src/SM_triplet.cpp: amplitudes of the scalar-triplet model.

Only the symbolic amplitudes and squared amplitudes are computed here. The
library generation and the comparison with reference results belong to
phase 6.
"""

import pytest

from pymarty.calculation import (
    Insertion,
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.modeling.models.sm_triplet import SMTriplet
from pymarty.symbolic import Expr


@pytest.fixture(scope="module")
def model() -> SMTriplet:
    return SMTriplet()


@pytest.mark.parametrize(
    "legs",
    [
        [incoming("N_1"), outgoing("e"), outgoing(anti("\\eta^-"))],
        [incoming("Z"), outgoing("nu_e"), outgoing("nu_e")],
        [incoming("\\Delta^{--}"), outgoing("e"), outgoing("\\mu")],
        [incoming("\\Delta^{--}"), outgoing("\\eta^-"), outgoing("\\eta^-")],
        [incoming("\\Delta^{--}"), outgoing("e"), outgoing("e")],
        [incoming("\\eta^-"), incoming("\\eta^-"), outgoing("e"), outgoing("e")],
    ],
    ids=["N1_e_etaP", "Z_nu_nu", "Dmm_e_mu", "Dmm_eta_eta", "Dmm_e_e", "eta_eta_e_e"],
)
def test_amplitude_and_squared_amplitude_are_symbolic(
    model: SMTriplet, legs: list[Insertion]
) -> None:
    amplitude = compute_amplitude(model, Order.ONE_LOOP, legs)
    assert amplitude.diagrams
    assert isinstance(compute_squared_amplitude(model, amplitude), Expr)
