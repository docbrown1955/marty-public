"""Port of tests/system/src/SM_2IDM.cpp: one-loop mu -> e Z amplitude of the 2IDM.

Only the symbolic amplitude is computed here. The library generation and the
comparison with the reference results belong to phase 6.
"""

import pytest

from pymarty.calculation import (
    Order,
    compute_amplitude,
    incoming,
    outgoing,
    set_options,
)
from pymarty.modeling.models.sm_2idm import SM2IDM


@pytest.fixture(scope="module")
def model() -> SM2IDM:
    return SM2IDM()


@pytest.mark.loops
def test_mu_to_e_z_one_loop_amplitude(model: SM2IDM) -> None:
    with set_options(
        decompose_in_operators=False,
        decompose_in_local_operator=False,
        exclude_tadpoles=True,
        exclude_mass_corrections=True,
        exclude_boxes=True,
    ):
        amplitude = compute_amplitude(
            model,
            Order.ONE_LOOP,
            [incoming("mu"), outgoing("e"), outgoing("Z")],
        )
    assert amplitude.diagrams
