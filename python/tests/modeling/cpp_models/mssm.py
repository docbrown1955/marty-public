"""MSSM high-energy model, ported from src/marty/models/mssm.cpp (MSSM_HEM).

Ported: the three gauge groups (SU(3) C, SU(2) L, U(1) Y) and the flavour
group, the SM-like fermions and their superpartners, the Higgs doublets, the
higgsinos (with their Higgsino mass term) and the gauginos (Majorana, so
sB, sW and sG are split into sB_L/sB_R, sW_L/sW_R and sG_L/sG_R).

Not ported, because the declarative API has no binding for them:
  - Soft slepton and squark masses (M2_s_L, M2_s_E, M2_s_Q, M2_s_U, M2_s_D,
    addTensorCoupling and addLagrangianTerm in initSLeptons / initSQuarks).
    They need a flavour tensor built on the model's own flavour VectorSpace
    (C++ getVectorSpace("SM_flavor")); the Python Model does not expose that
    space, and a fresh Space with the same name is rejected by the tensor
    indices. Also needs Tensor::addHermitianProperty.
  - Gaugino-gauge and gaugino-fermion-scalar Yukawa terms in
    initGauginoInteractions / initYukawas / initLeptons. They need the
    charge-conjugation matrix (dirac4.C_matrix) and the gauge generators
    (gauged->getGenerator).
  - Hermitian Yukawa and trilinear tensors (addHermitianProperty).
  - Higgs potential and quartic D-terms in initHiggsPotential and
    initQuarticDTerms (they need covariant derivatives).
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model, dirac_index
from pymarty.symbolic import constant, epsilon


def build() -> Model:
    """Return the ported part of the MSSM high-energy model."""
    model = Model(
        gauge=[
            GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g"),
            GaugeGroup(U(1), name="Y", boson_name="B", coupling="g'"),
        ],
        flavor=[FlavorGroup("SM_flavor", 3)],
    )

    gaugino_b = model.majorana_fermion("sB ; \\tilde{B}")
    gaugino_w = model.majorana_fermion("sW ; \\tilde{W}")
    gaugino_w.set_rep("L", 2)
    gaugino_g = model.majorana_fermion("sG; \\tilde{G}")
    gaugino_g.set_rep("C", [1, 1])
    for gaugino in (gaugino_b, gaugino_w, gaugino_g):
        model.add_particle(gaugino)

    lepton = model.weyl_fermion("L_L", chirality="L")
    lepton.set_rep("L", 1)
    lepton.set_rep("Y", Fraction(-1, 2))
    lepton.set_fundamental_flavor_rep("SM_flavor")

    right_lepton = model.weyl_fermion("E_R", chirality="R")
    right_lepton.set_rep("Y", -1)
    right_lepton.set_fundamental_flavor_rep("SM_flavor")

    quark = model.weyl_fermion("Q_L", chirality="L")
    quark.set_rep("C", [1, 0])
    quark.set_rep("L", 1)
    quark.set_rep("Y", Fraction(1, 6))
    quark.set_fundamental_flavor_rep("SM_flavor")

    up = model.weyl_fermion("U_R", chirality="R")
    up.set_rep("C", [1, 0])
    up.set_rep("Y", Fraction(2, 3))
    up.set_fundamental_flavor_rep("SM_flavor")

    down = model.weyl_fermion("D_R", chirality="R")
    down.set_rep("C", [1, 0])
    down.set_rep("Y", Fraction(-1, 3))
    down.set_fundamental_flavor_rep("SM_flavor")

    for field in (lepton, right_lepton, quark, up, down):
        model.add_particle(field)

    scalar_reps: dict[str, dict[str, object]] = {
        "sL_L ; \\tilde{L_L}": {"L": 1, "Y": Fraction(-1, 2)},
        "sE_R ; \\tilde{E_R}": {"Y": -1},
        "sQ_L ; \\tilde{Q_L}": {"C": [1, 0], "L": 1, "Y": Fraction(1, 6)},
        "sU_R ; \\tilde{U_R}": {"C": [1, 0], "Y": Fraction(2, 3)},
        "sD_R ; \\tilde{D_R}": {"C": [1, 0], "Y": Fraction(-1, 3)},
    }
    for name, reps in scalar_reps.items():
        scalar = model.scalar_boson(name)
        for group, rep in reps.items():
            scalar.set_rep(group, rep)
        scalar.set_fundamental_flavor_rep("SM_flavor")
        model.add_particle(scalar)

    higgs_up = model.scalar_boson("Hu;H_{u}")
    higgs_up.set_rep("L", 1)
    higgs_up.set_rep("Y", Fraction(1, 2))
    model.add_particle(higgs_up)

    higgs_down = model.scalar_boson("Hd;H_{d}")
    higgs_down.set_rep("L", 1)
    higgs_down.set_rep("Y", Fraction(-1, 2))
    model.add_particle(higgs_down)

    for name in ("sHu ; \\tilde{Hu}", "sHd ; \\tilde{Hd}"):
        higgsino = model.weyl_fermion(name, chirality="L")
        higgsino.set_rep("L", 1)
        model.add_particle(higgsino)

    _add_higgsino_mass(model)
    return model


def _add_higgsino_mass(model: Model) -> None:
    """Add the Higgsino mass term -mu sHu eps sHd with its Hermitian conjugate."""
    mu = constant("mu_h;\\mu_{h}")
    s_hu = model.particle("sHu")
    s_hd = model.particle("sHd")
    i = model.generate_index("L", "sHu")
    j = model.generate_index("L", "sHu")
    a = dirac_index()
    model.add_term(-mu * s_hu(i, a) * epsilon(i, j) * s_hd(j, a), add_hermitic=True)
