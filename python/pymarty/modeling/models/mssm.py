"""MSSM high-energy model, ported from src/marty/models/mssm.cpp (MSSM_HEM).

Ported: the three gauge groups (SU(3) C, SU(2) L, U(1) Y) and the flavour
group, the SM-like fermions and their superpartners, the Higgs doublets, the
higgsinos (with their Higgsino mass term), the gauginos (Majorana, so
sB, sW and sG are split into sB_L/sB_R, sW_L/sW_R and sG_L/sG_R) and the
soft slepton and squark masses (M2_s_L, M2_s_E, M2_s_Q, M2_s_U, M2_s_D).

Not ported:
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
from pymarty.modeling import FlavorGroup, GaugeGroup, Model
from pymarty.symbolic import ComplexProperty, Tensor, constant, epsilon, tensor


class MSSM(Model):
    """Ported part of the MSSM high-energy model (src/marty/models/mssm.cpp).

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[
                GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
                GaugeGroup(SU(2), name="L", boson_name="W", coupling="g"),
                GaugeGroup(U(1), name="Y", boson_name="B", coupling="g'"),
            ],
            flavor=[FlavorGroup("SM_flavor", 3)],
        )
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:

    model.add_majorana_fermion("sB ; \\tilde{B}")
    model.add_majorana_fermion("sW ; \\tilde{W}", gauge_rep={"L": 2})
    model.add_majorana_fermion("sG; \\tilde{G}", gauge_rep={"C": [1, 1]})

    model.add_weyl_fermion(
        "L_L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "E_R", chirality="R", gauge_rep={"Y": -1}, flavor_rep="SM_flavor"
    )
    model.add_weyl_fermion(
        "Q_L",
        chirality="L",
        gauge_rep={"C": [1, 0], "L": 1, "Y": Fraction(1, 6)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "U_R",
        chirality="R",
        gauge_rep={"C": [1, 0], "Y": Fraction(2, 3)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "D_R",
        chirality="R",
        gauge_rep={"C": [1, 0], "Y": Fraction(-1, 3)},
        flavor_rep="SM_flavor",
    )

    scalar_reps: dict[str, dict[str, object]] = {
        "sL_L ; \\tilde{L_L}": {"L": 1, "Y": Fraction(-1, 2)},
        "sE_R ; \\tilde{E_R}": {"Y": -1},
        "sQ_L ; \\tilde{Q_L}": {"C": [1, 0], "L": 1, "Y": Fraction(1, 6)},
        "sU_R ; \\tilde{U_R}": {"C": [1, 0], "Y": Fraction(2, 3)},
        "sD_R ; \\tilde{D_R}": {"C": [1, 0], "Y": Fraction(-1, 3)},
    }
    for name, reps in scalar_reps.items():
        model.add_scalar_boson(
            name, gauge_rep=reps, flavor_rep="SM_flavor", self_conjugate=False
        )

    model.add_scalar_boson(
        "Hu;H_{u}",
        gauge_rep={"L": 1, "Y": Fraction(1, 2)},
        self_conjugate=False,
    )

    model.add_scalar_boson(
        "Hd;H_{d}",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        self_conjugate=False,
    )

    for name in ("sHu ; \\tilde{Hu}", "sHd ; \\tilde{Hd}"):
        model.add_weyl_fermion(name, chirality="L", gauge_rep={"L": 1})

    _add_higgsino_mass(model)
    _add_soft_masses(model)


def _add_higgsino_mass(model: Model) -> None:
    """Add the Higgsino mass term -mu sHu eps sHd with its Hermitian conjugate."""
    mu = constant("mu_h;\\mu_{h}")
    s_hu = model.particle("sHu")
    s_hd = model.particle("sHd")
    i = model.generate_index("L", "sHu")
    j = model.generate_index("L", "sHu")
    a = model.generate_dirac_index()
    model.add_term(-mu * s_hu(i, a) * epsilon(i, j) * s_hd(j, a), add_hermitic=True)


def _soft_mass(model: Model, name: str, latex: str) -> Tensor:
    """Hermitian, complex flavour matrix of a soft mass, added as a tensor coupling."""
    flavor = model.vector_space("SM_flavor")
    m2 = tensor(
        f"{name};{latex}",
        [flavor, flavor],
        complex_property=ComplexProperty.Complex,
    )
    fI = flavor.index("I")
    fJ = flavor.index("J")
    m2.add_hermitian_property(flavor, m2(fI, fJ), m2(fI, fJ))
    model.add_tensor_coupling(m2)
    return m2


def _add_soft_masses(model: Model) -> None:
    """Add the soft slepton and squark masses (initSLeptons, initSQuarks in C++)."""
    flavor = model.vector_space("SM_flavor")
    fI = flavor.index("I")
    fJ = flavor.index("J")

    m2_L = _soft_mass(model, "M2_s_L", "M^{2}_{\\tilde{L}}")
    sL = model.particle("sL_L")
    il = model.generate_index("L", "sL_L")
    model.add_term(-m2_L(fI, fJ) * sL(fI, il).conjugate() * sL(fJ, il))

    m2_E = _soft_mass(model, "M2_s_E", "M^{2}_{\\tilde{E}}")
    sE = model.particle("sE_R")
    model.add_term(-m2_E(fI, fJ) * sE(fI).conjugate() * sE(fJ))

    m2_Q = _soft_mass(model, "M2_s_Q", "M^{2}_{\\tilde{Q}}")
    sQ = model.particle("sQ_L")
    a = model.generate_index("C", "sQ_L")
    il = model.generate_index("L", "sQ_L")
    model.add_term(-m2_Q(fI, fJ) * sQ(fI, a, il).conjugate() * sQ(fJ, a, il))

    m2_U = _soft_mass(model, "M2_s_U", "M^{2}_{\\tilde{U}}")
    sU = model.particle("sU_R")
    a = model.generate_index("C", "sU_R")
    model.add_term(-m2_U(fI, fJ) * sU(fI, a).conjugate() * sU(fJ, a))

    m2_D = _soft_mass(model, "M2_s_D", "M^{2}_{\\tilde{D}}")
    sD = model.particle("sD_R")
    a = model.generate_index("C", "sD_R")
    model.add_term(-m2_D(fI, fJ) * sD(fI, a).conjugate() * sD(fJ, a))
