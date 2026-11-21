"""QED with a single Dirac fermion, ported from src/marty/models/qed.cpp."""

from pymarty.group_theory import U
from pymarty.modeling import GaugeGroup, Model
from pymarty.symbolic import constant


def build() -> Model:
    """Return the QED model: a U(1) gauge group and one charged Dirac fermion."""
    model = Model(gauge=[GaugeGroup(U(1), name="em", boson_name="A", coupling="e")])
    psi = model.dirac_fermion("\\psi")
    psi.set_rep("em", -1)
    model.add_fermionic_mass(psi, constant("m"))
    model.add_particle(psi)
    return model
