"""QED with a single Dirac fermion, ported from src/marty/models/qed.cpp."""

from pymarty.group_theory import U
from pymarty.modeling import GaugeGroup, Model
from pymarty.symbolic import constant


def build() -> Model:
    """Return the QED model: a U(1) gauge group and one charged Dirac fermion."""
    model = Model(gauge=[GaugeGroup(U(1), name="em", boson_name="A", coupling="e")])
    model.add_dirac_fermion("\\psi", gauge_rep={"em": -1}, mass=constant("m"))
    return model
