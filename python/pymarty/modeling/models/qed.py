"""QED with a single Dirac fermion, ported from src/marty/models/qed.cpp."""

from pymarty.group_theory import U
from pymarty.modeling import GaugeGroup, Model
from pymarty.symbolic import constant


class QED(Model):
    """QED: a U(1) gauge group and one charged Dirac fermion.

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[GaugeGroup(U(1), name="em", boson_name="A", coupling="e")]
        )
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:
    model.add_dirac_fermion("\\psi", gauge_rep={"em": -1}, mass=constant("m"))
