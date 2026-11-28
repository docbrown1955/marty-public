"""QCD with two colour-triplet quarks, ported from src/marty/models/qcd.cpp."""

from pymarty.group_theory import SU
from pymarty.modeling import GaugeGroup, Model
from pymarty.symbolic import constant


class QCD(Model):
    """QCD: an SU(3) gauge group with the u and d quark triplets.

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[GaugeGroup(SU(3), name="C", boson_name="G", coupling="g")]
        )
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:
    for name in ("u", "d"):
        model.add_dirac_fermion(
            name, gauge_rep={"C": [1, 0]}, mass=constant(f"m_{name}")
        )
