"""Port of testConjugation.cpp: relative signs of fermion-number-violating diagrams.

Only the symbolic squared amplitudes are computed here. The numerical
comparison with the validated values of the C++ test is done in phase 6.
"""

import pytest

from pymarty.calculation import (
    FeynOptions,
    Order,
    compute_amplitude,
    compute_squared_amplitude,
    disable_particles,
    incoming,
    outgoing,
    set_options,
)
from pymarty.modeling.models.conjugation import Conjugation
from pymarty.symbolic import EvalMode


@pytest.fixture(scope="module")
def model() -> Conjugation:
    return Conjugation()


def _squared_amplitude(model: Conjugation, disabled: str, last_leg: str) -> str:
    options = FeynOptions(filters=[disable_particles(disabled)])
    with set_options(order_external_fermions=False):
        amplitude = compute_amplitude(
            model,
            Order.TREE,
            [
                incoming("\\psi"),
                incoming("\\psi"),
                outgoing("\\psi"),
                outgoing("\\lambda"),
                outgoing(last_leg),
            ],
            options=options,
        )
        squared = compute_squared_amplitude(model, amplitude)
    evaluated = squared.evaluate(mode=EvalMode.Abbreviation)
    assert evaluated.free_indices == ()
    return str(evaluated)


def test_squared_amplitudes_are_symbolic_and_differ(model: Conjugation) -> None:
    with_phi = _squared_amplitude(model, "A", "\\phi")
    with_vector = _squared_amplitude(model, "\\phi", "A")
    assert with_phi
    assert with_vector
    assert with_phi != with_vector
