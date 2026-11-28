"""SU(3) gauge theory with one top quark, for the gg -> ttbar process of gg_to_tt.cpp.

The C++ test can use the full Standard Model or this minimal model. The tree-level
gg -> ttbar result of this model agrees with the reference data of the test.
"""

from pymarty.group_theory import SU
from pymarty.modeling import GaugeGroup, Model
from pymarty.symbolic import constant


class GgToTt(Model):
    """QCD with a single colour-triplet top quark of mass ``m_t``.

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s")]
        )
        self.add_dirac_fermion("t", gauge_rep={"C": [1, 0]}, mass=constant("m_t"))
        if finalize:
            self.finalize()
