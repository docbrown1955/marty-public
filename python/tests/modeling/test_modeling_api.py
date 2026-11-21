"""Python counterparts of the C++ ``ModelingApi`` tests."""

from fractions import Fraction

import pytest

from pymarty.errors import MartyLookupError, MartyTypeError, MartyValueError
from pymarty.group_theory import SU, U
from pymarty.modeling import GaugeGroup, Model


def make_toy() -> Model:
    return Model(
        gauge=[
            GaugeGroup(SU(2), name="L", boson_name="W"),
            GaugeGroup(U(1), name="Y", boson_name="B"),
        ]
    )


@pytest.mark.parity("ModelingApi.UnknownParticleIsLookupError")
def test_unknown_particle_is_lookup_error() -> None:
    toy = make_toy()
    assert "no_such_particle" not in toy
    with pytest.raises(MartyLookupError):
        toy.particle("no_such_particle")


@pytest.mark.parity("ModelingApi.ChargeOnNonU1GroupIsTypeError")
def test_charge_on_non_u1_group_is_type_error() -> None:
    q = make_toy().weyl_fermion("Q_L", chirality="L")
    with pytest.raises(MartyTypeError):
        q.set_rep("L", Fraction(1, 6))
    q.set_rep("Y", Fraction(1, 6))


@pytest.mark.parity("ModelingApi.LabelsOnU1GroupIsTypeError")
def test_labels_on_u1_group_is_type_error() -> None:
    q = make_toy().weyl_fermion("Q_L", chirality="L")
    with pytest.raises(MartyTypeError):
        q.set_rep("Y", [1])


@pytest.mark.parity("ModelingApi.UnknownChiralityIsValueError")
def test_unknown_chirality_is_value_error() -> None:
    with pytest.raises(MartyValueError):
        make_toy().weyl_fermion("Q_L", chirality="X")
