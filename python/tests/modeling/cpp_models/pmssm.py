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

    gaugino_B = model.weyl_fermion("sB", chirality="L")
    gaugino_W = model.weyl_fermion("sW", chirality="L")
    gaugino_G = model.weyl_fermion("sG", chirality="L")
    for gaugino in (gaugino_B, gaugino_W, gaugino_G):
        gaugino.set_self_conjugate(True)
    gaugino_W.set_rep("L", (2,))
    gaugino_G.set_rep("C", (1, 1))
    for gaugino in (gaugino_B, gaugino_W, gaugino_G):
        model.add_particle(gaugino)

    L_L = model.weyl_fermion("L_L", chirality="L")
    E_R = model.weyl_fermion("E_R", chirality="R")
    L_L.set_rep("L", 1)
    L_L.set_rep("Y", Fraction(-1, 2))
    E_R.set_rep("Y", -1)
    for lepton in (L_L, E_R):
        lepton.set_fundamental_flavor_rep("SM_flavor")
        model.add_particle(lepton)

    sL_L = model.scalar_boson("sL_L")
    sE_R = model.scalar_boson("sE_R")
    sL_L.set_rep("L", 1)
    sL_L.set_rep("Y", Fraction(-1, 2))
    sE_R.set_rep("Y", -1)
    for slepton in (sL_L, sE_R):
        slepton.set_fundamental_flavor_rep("SM_flavor")
        model.add_particle(slepton)

    Q_L = model.weyl_fermion("Q_L", chirality="L")
    U_R = model.weyl_fermion("U_R", chirality="R")
    D_R = model.weyl_fermion("D_R", chirality="R")
    Q_L.set_rep("C", (1, 0))
    Q_L.set_rep("L", 1)
    Q_L.set_rep("Y", Fraction(1, 6))
    U_R.set_rep("C", (1, 0))
    U_R.set_rep("Y", Fraction(2, 3))
    D_R.set_rep("C", (1, 0))
    D_R.set_rep("Y", Fraction(-1, 3))
    for quark in (Q_L, U_R, D_R):
        quark.set_fundamental_flavor_rep("SM_flavor")
        model.add_particle(quark)

    squarks = [
        model.scalar_boson("sQ_L"),
        model.scalar_boson("sU_R"),
        model.scalar_boson("sD_R"),
    ]
    squarks[0].set_rep("C", (1, 0))
    squarks[0].set_rep("L", 1)
    squarks[0].set_rep("Y", Fraction(1, 6))
    squarks[1].set_rep("C", (1, 0))
    squarks[1].set_rep("Y", Fraction(2, 3))
    squarks[2].set_rep("C", (1, 0))
    squarks[2].set_rep("Y", Fraction(-1, 3))
    for squark in squarks:
        squark.set_fundamental_flavor_rep("SM_flavor")
        model.add_particle(squark)

    Hu = model.scalar_boson("Hu")
    Hd = model.scalar_boson("Hd")
    Hu.set_rep("L", 1)
    Hu.set_rep("Y", Fraction(1, 2))
    Hd.set_rep("L", 1)
    Hd.set_rep("Y", Fraction(-1, 2))
    model.add_particle(Hu)
    model.add_particle(Hd)

    s_Hu = model.weyl_fermion("sHu", chirality="L")
    s_Hd = model.weyl_fermion("sHd", chirality="L")
    s_Hu.set_rep("L", 1)
    s_Hu.set_rep("Y", Fraction(1, 2))
    s_Hd.set_rep("L", 1)
    s_Hd.set_rep("Y", Fraction(-1, 2))
    model.add_particle(s_Hu)
    model.add_particle(s_Hd)

    return model
