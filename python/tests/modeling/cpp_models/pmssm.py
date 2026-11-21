"""Field content of the C++ PMSSM model (src/marty/models/pmssm.cpp).

The C++ builder inherits the MSSM content (MSSM_Model::initContent) and then
applies interactions and a low-energy breaking chain. This port reproduces the
field content only: gauge groups, fermions, sfermions, Higgs doublets and
higgsinos. The interaction terms, mass parameters and the breaking chain need
bindings that the declarative API does not expose (see the test module).
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model


def build() -> Model:
    """Build the PMSSM field content, before interactions and symmetry breaking."""
    model = Model(
        gauge=[
            GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g"),
            GaugeGroup(U(1), name="Y", boson_name="B", coupling="g'"),
        ],
        flavor=[FlavorGroup("SM_flavor", 3)],
    )

    model.add_weyl_fermion("sB", "L", self_conjugate=True)
    model.add_weyl_fermion("sW", "L", gauge_rep={"L": (2,)}, self_conjugate=True)
    model.add_weyl_fermion("sG", "L", gauge_rep={"C": (1, 1)}, self_conjugate=True)

    model.add_weyl_fermion(
        "L_L",
        "L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion("E_R", "R", gauge_rep={"Y": -1}, flavor_rep="SM_flavor")

    model.add_scalar_boson(
        "sL_L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "sE_R", gauge_rep={"Y": -1}, flavor_rep="SM_flavor", self_conjugate=False
    )

    model.add_weyl_fermion(
        "Q_L",
        "L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "U_R",
        "R",
        gauge_rep={"C": (1, 0), "Y": Fraction(2, 3)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "D_R",
        "R",
        gauge_rep={"C": (1, 0), "Y": Fraction(-1, 3)},
        flavor_rep="SM_flavor",
    )

    model.add_scalar_boson(
        "sQ_L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "sU_R",
        gauge_rep={"C": (1, 0), "Y": Fraction(2, 3)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "sD_R",
        gauge_rep={"C": (1, 0), "Y": Fraction(-1, 3)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )

    model.add_scalar_boson(
        "Hu", gauge_rep={"L": 1, "Y": Fraction(1, 2)}, self_conjugate=False
    )
    model.add_scalar_boson(
        "Hd", gauge_rep={"L": 1, "Y": Fraction(-1, 2)}, self_conjugate=False
    )

    model.add_weyl_fermion("sHu", "L", gauge_rep={"L": 1, "Y": Fraction(1, 2)})
    model.add_weyl_fermion("sHd", "L", gauge_rep={"L": 1, "Y": Fraction(-1, 2)})

    return model
