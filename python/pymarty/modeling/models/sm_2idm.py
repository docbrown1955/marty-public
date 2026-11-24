"""Toy model of SM_2IDM.cpp: a two-Higgs-doublet extension with a dark sector.

Mirrors the model building, breaking, Yukawa diagonalisation and field
rotations of tests/system/src/SM_2IDM.cpp. The library generation is left to
phase 6.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model, dirac_C, renamed_indices
from pymarty.modeling.models._symbols import constant
from pymarty.modeling.models.sm import ckm_matrix
from pymarty.symbolic import (
    ComplexProperty,
    I,
    cos,
    epsilon,
    sin,
    sqrt,
    tensor,
    unitary_tensor,
)

FLAVOR = "F"


class SM2IDM(Model):
    """Two-Higgs-doublet toy model with a dark sector, broken as in SM_2IDM.cpp.

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[
                GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
                GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
                GaugeGroup(U(1), name="Y", boson_name="A_Y", coupling="g_Y"),
                GaugeGroup(U(1), name="D", boson_name="A_D", coupling="g_D"),
            ],
            flavor=[FlavorGroup(FLAVOR, 3)],
        )
        _add_content(self)
        _add_lagrangian(self)
        _break_model(self)
        _diagonalize_yukawas(self)
        _expand_around_vevs(self)
        _rotate_fields(self)
        if finalize:
            self.finalize()


def _square(expr):  # type: ignore[no-untyped-def]
    """Square an expression whose indices are summed, renaming the dummies."""
    return expr * renamed_indices(expr)


def _add_content(model: Model) -> None:
    """Add the particles of the model."""
    model.add_weyl_fermion(
        "Q_L",
        chirality="L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion(
        "L_L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion(
        "U_R",
        chirality="R",
        gauge_rep={"C": (1, 0), "Y": Fraction(2, 3)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion(
        "D_R",
        chirality="R",
        gauge_rep={"C": (1, 0), "Y": Fraction(-1, 3)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion(
        "E_R",
        chirality="R",
        gauge_rep={"Y": -1},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion("XL", chirality="L", gauge_rep={"D": -1})
    model.add_weyl_fermion("XR", chirality="L", gauge_rep={"D": 1})

    model.add_scalar_boson(
        "H", gauge_rep={"L": 1, "Y": Fraction(1, 2)}, self_conjugate=False
    )
    model.add_scalar_boson(
        "etaA",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2), "D": -1},
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "etaB",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2), "D": -1},
        self_conjugate=False,
    )
    model.add_scalar_boson("phi", gauge_rep={"D": 2}, self_conjugate=False)


def _add_lagrangian(model: Model) -> None:
    """Add the scalar potential, the Yukawa and the fermion-number violating terms."""
    H = model.particle("H")
    etaA = model.particle("etaA")
    etaB = model.particle("etaB")
    phi = model.particle("phi")
    QL = model.particle("Q_L")
    LL = model.particle("L_L")
    UR = model.particle("U_R")
    DR = model.particle("D_R")
    ER = model.particle("E_R")
    XL = model.particle("XL")
    XR = model.particle("XR")

    flavor_space = model.vector_space(FLAVOR)
    I_f = flavor_space.index()
    J_f = flavor_space.index()
    A = model.generate_index("C", "Q_L")
    i = model.generate_index("L", "Q_L")
    j = model.generate_index("L", "Q_L")
    a = model.generate_dirac_index()
    b = model.generate_dirac_index()
    C = dirac_C

    m2H = constant("m2_H")
    m2A = constant("m2_eA")
    m2B = constant("m2_eB")
    m2AB = constant("m2_eAB")
    mp = constant("m2_phi")
    lH = constant("l_H")
    lA = constant("l_A")
    lB = constant("l_B")
    lAB1 = constant("l_AB1")
    lAB2 = constant("l_AB2")
    lAB3 = constant("l_AB3")
    lAB4 = constant("l_AB4")
    lHA1 = constant("l_HAA1")
    lHAB1 = constant("l_HAB1")
    lHB1 = constant("l_HBB1")
    lHA3 = constant("l_HAA3")
    lHAB3 = constant("l_HAB3")
    lHB3 = constant("l_HBB3")
    lHp = constant("l_Hphi")
    lp = constant("l_phi")

    model.add_term(m2H * H(i).conjugate() * H(i))
    model.add_term(-m2A * etaA(i).conjugate() * etaA(i))
    model.add_term(-m2B * etaB(i).conjugate() * etaB(i))
    model.add_term(-m2AB * etaA(i).conjugate() * etaB(i), add_hermitic=True)
    model.add_term(mp * phi().conjugate() * phi())

    model.add_term(-lH * _square(H(i).conjugate() * H(i)))
    model.add_term(-lp * _square(phi().conjugate() * phi()))
    model.add_term(-lHp * H(i).conjugate() * H(i) * phi().conjugate() * phi())

    model.add_term(-lA * _square(etaA(i).conjugate() * etaA(i)))
    model.add_term(-lB * _square(etaB(i).conjugate() * etaB(i)))
    model.add_term(
        -lAB1 * etaA(i).conjugate() * etaA(i) * etaB(j).conjugate() * etaB(j)
    )
    model.add_term(
        -lAB2 * etaA(i).conjugate() * etaB(i) * etaA(j).conjugate() * etaB(j),
        add_hermitic=True,
    )
    model.add_term(
        -lAB3 * etaA(i).conjugate() * etaA(i) * etaA(j).conjugate() * etaB(j),
        add_hermitic=True,
    )
    model.add_term(
        -lAB4 * etaB(i).conjugate() * etaB(i) * etaB(j).conjugate() * etaA(j),
        add_hermitic=True,
    )

    model.add_term(-lHA1 * H(i).conjugate() * H(i) * etaA(j).conjugate() * etaA(j))
    model.add_term(-lHB1 * H(i).conjugate() * H(i) * etaB(j).conjugate() * etaB(j))
    model.add_term(
        -lHAB1 * H(i).conjugate() * H(i) * etaA(j).conjugate() * etaB(j),
        add_hermitic=True,
    )
    model.add_term(
        -lHA3 * H(i).conjugate() * etaA(i) * etaA(j).conjugate() * H(j),
        add_hermitic=True,
    )
    model.add_term(
        -lHB3 * H(i).conjugate() * etaB(i) * etaB(j).conjugate() * H(j),
        add_hermitic=True,
    )
    model.add_term(
        -lHAB3 * H(i).conjugate() * etaA(i) * etaB(j).conjugate() * H(j),
        add_hermitic=True,
    )

    mD = constant("m_D")
    YX = constant("Y_X", complex_property=ComplexProperty.Complex)
    YXc = constant("Y_Xc", complex_property=ComplexProperty.Complex)
    model.add_term(-mD * XR(a) * C(a, b) * XL(b), add_hermitic=True)
    model.add_term(
        -Fraction(1, 2) * YX * phi() * XL(a) * C(a, b) * XL(b), add_hermitic=True
    )
    model.add_term(
        -Fraction(1, 2) * YXc * phi().conjugate() * XR(a) * C(a, b) * XR(b),
        add_hermitic=True,
    )

    Ye = tensor(
        "Y_e", [flavor_space, flavor_space], complex_property=ComplexProperty.Complex
    )
    Yu = tensor(
        "Y_u", [flavor_space, flavor_space], complex_property=ComplexProperty.Complex
    )
    Yd = tensor(
        "Y_d", [flavor_space, flavor_space], complex_property=ComplexProperty.Complex
    )
    for yukawa in (Ye, Yu, Yd):
        model.add_tensor_coupling(yukawa)

    eps = epsilon(i, j)
    model.add_term(
        -QL(I_f, A, i, a).conjugate()
        * Yu(I_f, J_f)
        * UR(J_f, A, a)
        * eps
        * H(j).conjugate(),
        add_hermitic=True,
    )
    model.add_term(
        -QL(I_f, A, i, a).conjugate() * Yd(I_f, J_f) * DR(J_f, A, a) * H(i),
        add_hermitic=True,
    )
    model.add_term(
        -LL(I_f, i, a).conjugate() * Ye(I_f, J_f) * ER(J_f, a) * H(i),
        add_hermitic=True,
    )

    YX1A = tensor("Y_X1A", [flavor_space], complex_property=ComplexProperty.Complex)
    YX1B = tensor("Y_X1B", [flavor_space], complex_property=ComplexProperty.Complex)
    model.add_term(
        -YX1A(I_f) * XL(a) * C(a, b) * LL(I_f, i, b) * etaA(i).conjugate(),
        add_hermitic=True,
    )
    model.add_term(
        -YX1B(I_f) * XL(a) * C(a, b) * LL(I_f, i, b) * etaB(i).conjugate(),
        add_hermitic=True,
    )


def _break_model(model: Model) -> None:
    """Break the gauge and flavour symmetries, renaming the states as in SM_2IDM.cpp."""
    model.break_gauge_symmetry("D")
    model.break_gauge_symmetry("Y")
    model.break_gauge_symmetry(
        "L",
        renaming={
            "Q_L": ["U_L", "D_L"],
            "L_L": ["Nu_L", "E_L"],
            "H": ["H_1", "H_2"],
            "etaA": ["etaA0; \\eta ^0_{A}", "eta1; \\eta ^-_{A}"],
            "etaB": ["etaB0; \\eta ^0_{B}", "eta2; \\eta ^-_{B}"],
        },
    )


def _diagonalize_yukawas(model: Model) -> None:
    """Diagonalise the Yukawas, applying the CKM matrix on the down sector.

    The global factor ``sqrt(2)/v_H`` multiplies each mass on the diagonal.
    """
    vH = constant("v_H")
    factor = sqrt(2) / vH
    flavor_space = model.vector_space(FLAVOR)
    V_CKM = unitary_tensor("V_CKM", flavor_space, ckm_matrix())

    model.diagonalize_yukawa(
        "Y_e", [factor * constant(name) for name in ("m_e", "m_mu", "m_tau")]
    )
    model.diagonalize_yukawa(
        "Y_u", [factor * constant(name) for name in ("m_u", "m_c", "m_t")]
    )
    DL = model.particle("D_L")
    DR = model.particle("D_R")
    model.diagonalize_yukawa(
        "Y_d",
        [factor * constant(name) for name in ("m_d", "m_s", "m_b")],
        V_CKM,
        [DL, DR],
    )

    model.break_flavor_symmetry(
        FLAVOR,
        renaming={
            "U_L": ["u_L", "c_L", "t_L"],
            "U_R": ["u_R", "c_R", "t_R"],
            "D_L": ["d_L", "s_L", "b_L"],
            "D_R": ["d_R", "s_R", "b_R"],
            "E_L": ["e_L", "mu_L; \\mu_L", "tau_L; \\tau_L"],
            "E_R": ["e_R", "mu_R; \\mu_R", "tau_R; \\tau_R"],
            "Nu_L": [
                "nu_e_L; \\nu_{e_L}",
                "nu_mu_L; \\nu_{\\mu_L}",
                "nu_tau_L; \\nu_{\\tau_L}",
            ],
        },
    )


def _expand_around_vevs(model: Model) -> None:
    """Expand the doublets and singlets around their vacuum expectation values."""
    vH = constant("v_H")
    vph = constant("vphi")
    H1 = model.particle("H_1")
    H2 = model.particle("H_2")
    phi = model.particle("phi")
    phiR = phi.generate_similar_particle("phiR ; \\phi_R")
    phiI = phi.generate_similar_particle("phiI")
    Hp = H1.generate_similar_particle("Hp")
    Ah = H2.generate_similar_particle("Ah")
    hh = H2.generate_similar_particle("hh ; h^0")
    for particle in (phiR, phiI, Ah, hh):
        particle.set_self_conjugate(True)

    model.replace(phi, (vph + phiR() + I * phiI()) / sqrt(2))
    model.replace(H1, Hp())
    model.replace(H2, (vH + hh() + I * Ah()) / sqrt(2))

    W1 = model.particle("W_1")
    W2 = model.particle("W_2")
    W = W1.generate_similar_particle("W")
    W.set_self_conjugate(False)
    mu = model.generate_minkowski_index("mu")
    nu = model.generate_minkowski_index("nu")
    W_p = W(mu)
    W_m = W(mu).conjugate()
    F_W_p = W.field_strength()(mu, nu)
    F_W_m = F_W_p.conjugate()

    model.replace(W1(mu), (W_p + W_m) / sqrt(2))
    model.replace(W2(mu), I * (W_p - W_m) / sqrt(2))
    model.replace(W1.field_strength()(mu, nu), (F_W_p + F_W_m) / sqrt(2))
    model.replace(W2.field_strength()(mu, nu), I * (F_W_p - F_W_m) / sqrt(2))

    gL = model.coupling("g_L")
    gY = model.coupling("g_Y")
    e_em = constant("e_em")
    theta_W = constant("theta_W")
    model.replace(gL * gL + gY * gY, (gL / cos(theta_W)) ** 2)
    model.replace(gY, e_em / cos(theta_W))
    model.replace(gL, e_em / sin(theta_W))


def _rotate_fields(model: Model) -> None:
    """Rotate neutral gauge bosons, scalars and fermions to mass eigenstates."""
    model.rename_particle("W_3", "Z")
    model.rename_particle("A_D", "ZD")
    model.rename_particle("A_Y", "A")

    model.rotate_fields(
        [model.particle(name) for name in ("A", "Z", "ZD")],
        diagonalize_masses=True,
        n_mass_less_fields=1,
    )
    model.rotate_fields(
        [model.particle("eta1"), model.particle("eta2")], diagonalize_masses=True
    )
    model.rotate_fields(
        [model.particle("etaA0"), model.particle("etaB0")], diagonalize_masses=True
    )

    XL = model.particle("XL")
    XR = model.particle("XR")
    XL_mass, XR_mass = XL.mass, XR.mass
    model.rotate_fields([XL, XR], diagonalize_masses=True)
    xi1 = model.add_dirac_fermion("xi_1 ; \\xi_1")
    xi2 = model.add_dirac_fermion("xi_2 ; \\xi_2")
    xi1.set_self_conjugate(True)
    xi2.set_self_conjugate(True)
    xi1.mass = XL_mass
    xi2.mass = XR_mass
    a = model.generate_dirac_index()
    model.replace(XL(a), xi1(a) / sqrt(2))
    model.replace(XR(a), xi2(a) / sqrt(2))
    xi1.width = constant("GX1")
    xi2.width = constant("GX2")

    model.rotate_fields(
        [model.particle("hh"), model.particle("phiR")], diagonalize_masses=True
    )
    model.rotate_fields(
        [model.particle("Ah"), model.particle("phiI")], diagonalize_masses=True
    )

    model.rename_particle("Ah", "G01")
    model.rename_particle("phiI", "G02")
    model.rename_particle("Hp", "Gp")
    model.promote_to_goldstone("Gp", "W")
    model.promote_to_goldstone("G01", "Z")
    model.promote_to_goldstone("G02", "ZD")
    model.refresh()
