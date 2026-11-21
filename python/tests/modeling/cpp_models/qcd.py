"""QCD with two colour-triplet quarks, ported from src/marty/models/qcd.cpp."""

from pymarty.group_theory import SU
from pymarty.modeling import GaugeGroup, Model
from pymarty.symbolic import constant


def build() -> Model:
    """Return the QCD model: an SU(3) gauge group with the u and d quark triplets."""
    model = Model(gauge=[GaugeGroup(SU(3), name="C", boson_name="G", coupling="g")])
    for name in ("u", "d"):
        quark = model.dirac_fermion(name)
        quark.set_rep("C", [1, 0])
        model.add_fermionic_mass(quark, constant(f"m_{name}"))
        model.add_particle(quark)
    return model
