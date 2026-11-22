"""Partial pure-pymarty port of the C++ Standard Model (src/marty/models/sm.cpp).

Follows the C++ order: gauge groups, fermions, Higgs potential, Yukawas,
gauge symmetry breaking with the C++ renamings, the W +- replacements, the
Higgs VEV expansion, mass matrix diagonalization, the Yukawa replacements
with the CKM matrix, the flavour symmetry breaking and the Goldstone
promotion.

The W and Z masses are set as in ``SM_Model::adjust``, which also adds the
gauge-fixing terms.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model
from pymarty.modeling.models._symbols import constant
from pymarty.symbolic import (
    ComplexProperty,
    Expr,
    I,
    cos,
    diagonal_tensor,
    epsilon,
    sin,
    sqrt,
    tensor,
    unitary_tensor,
)

FLAVOR = "SM_flavor"


def ckm_matrix() -> list[list[Expr]]:
    """Return the CKM entries as symbolic constants named as in the C++ ``sm_input``."""
    complex_entries = {"V_ub", "V_cd", "V_cs", "V_td", "V_ts"}
    names = [
        ["V_ud", "V_us", "V_ub"],
        ["V_cd", "V_cs", "V_cb"],
        ["V_td", "V_ts", "V_tb"],
    ]
    return [
        [
            constant(
                f"{name};{name[0]}_{{{name[2:]}}}",
                complex_property=(
                    ComplexProperty.Complex
                    if name in complex_entries
                    else ComplexProperty.Real
                ),
            )
            for name in row
        ]
        for row in names
    ]


def break_sm_flavor(model: Model) -> None:
    """Split the ``SM_flavor`` fermions into generations.

    Port of ``SM_Model::flavorSymmetryBreaking``.
    """
    model.break_flavor_symmetry(
        FLAVOR,
        renaming={
            "U_L": ["u_L", "c_L", "t_L"],
            "D_L": ["d_L", "s_L", "b_L"],
            "E_L": ["e_L", "mu_L;\\mu_L", "tau_L;\\tau_L"],
            "Nu_L": ["nu_e;\\nu_e", "nu_mu;\\nu_\\mu", "nu_tau;\\nu_\\tau"],
            "U_R": ["u_R", "c_R", "t_R"],
            "D_R": ["d_R", "s_R", "b_R"],
            "E_R": ["e_R", "mu_R;\\mu_R", "tau_R;\\tau_R"],
        },
    )


class SM(Model):
    """Partially ported Standard Model (src/marty/models/sm.cpp).

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[
                GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
                GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
                GaugeGroup(U(1), name="Y", boson_name="B", coupling="g_Y"),
            ],
            flavor=[FlavorGroup(FLAVOR, 3)],
        )
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:

    model.add_weyl_fermion(
        "Q",
        chirality="L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
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
        "L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion(
        "E_R",
        chirality="R",
        gauge_rep={"Y": -1},
        flavor_rep=FLAVOR,
    )

    model.add_scalar_boson(
        "H", gauge_rep={"L": 1, "Y": Fraction(1, 2)}, self_conjugate=False
    )
    H = model.particle("H")
    i = model.generate_index("L", "H")
    j = model.generate_index("L", "H")
    H2 = H(i).conjugate() * H(i)
    H2_j = H(j).conjugate() * H(j)
    m_h = constant("m_h;m_{h}")
    v = constant("v")
    model.add_term(m_h * m_h / 2 * H2)
    model.add_term(-(m_h * m_h) / (2 * v * v) * H2 * H2_j)

    flavor = model.vector_space(FLAVOR)
    Yu = tensor("Yu", [flavor, flavor], complex_property=ComplexProperty.Complex)
    Yd = tensor("Yd", [flavor, flavor], complex_property=ComplexProperty.Complex)
    Ye = tensor("Ye", [flavor, flavor], complex_property=ComplexProperty.Complex)
    fI = flavor.index("I")
    fJ = flavor.index("J")
    a = model.generate_index("C", "Q")
    il = model.generate_index("L", "Q")
    jl = model.generate_index("L", "Q")
    al = model.generate_dirac_index()
    Q = model.particle("Q")
    U_R = model.particle("U_R")
    D_R = model.particle("D_R")
    L = model.particle("L")
    E_R = model.particle("E_R")

    model.add_term(
        Yu(fI, fJ)
        * H(il).conjugate()
        * epsilon(il, jl)
        * Q(fI, a, jl, al).conjugate()
        * U_R(fJ, a, al),
        add_hermitic=True,
    )
    model.add_term(
        -Yd(fI, fJ) * H(il) * Q(fI, a, il, al).conjugate() * D_R(fJ, a, al),
        add_hermitic=True,
    )
    model.add_term(
        -Ye(fI, fJ) * H(il) * L(fI, il, al).conjugate() * E_R(fJ, al),
        add_hermitic=True,
    )
    model.add_tensor_coupling(Ye)
    model.add_tensor_coupling(Yu)
    model.add_tensor_coupling(Yd)

    model.break_gauge_symmetry("Y")
    model.break_gauge_symmetry(
        "L",
        renaming={"Q": ["U_L", "D_L"], "L": ["Nu_L;\\nu_{L}", "E_L"]},
    )

    W1 = model.particle("W_1")
    W2 = model.particle("W_2")
    W_SM = model.add_similar_particle(
        "W", reference="W_1", self_conjugate=False, initialize_terms=False
    )
    cWp = model.add_ghost_boson("c_Wp ; c_{+}", W_SM)
    cWm = model.add_ghost_boson("c_Wm; c_{-}", W_SM, conjugated=True)
    W_SM.set_conjugated_ghost_boson(cWm)
    mu = model.generate_minkowski_index("mu")
    nu = model.generate_minkowski_index("nu")
    W_p = W_SM(mu)
    W_m = W_SM(mu).conjugate()
    F_W_p = W_SM.field_strength()(mu, nu)
    F_W_m = F_W_p.conjugate()

    def w1(p: Expr, m: Expr) -> Expr:
        return (p + m) / sqrt(2)

    def w2(p: Expr, m: Expr) -> Expr:
        return I * (p - m) / sqrt(2)

    model.replace(W1(mu), w1(W_p, W_m))
    model.replace(W2(mu), w2(W_p, W_m))
    model.replace(W1.field_strength()(mu, nu), w1(F_W_p, F_W_m))
    model.replace(W2.field_strength()(mu, nu), w2(F_W_p, F_W_m))
    model.replace(model.particle("c_W_1"), w1(cWp(), cWm()))
    model.replace(model.particle("c_W_2"), w2(cWp(), cWm()))

    h0 = model.add_scalar_boson("h", self_conjugate=True, initialize_terms=False)
    Gp = model.add_scalar_boson(
        "Gp ; G^+", self_conjugate=False, initialize_terms=False
    )
    G0 = model.add_scalar_boson("G0 ; G^0", self_conjugate=True, initialize_terms=False)
    model.replace(model.particle("H_1"), Gp())
    model.replace(model.particle("H_2"), (h0() + I * G0() + v) / sqrt(2))

    model.diagonalize_mass_matrices()
    model.rename_particle("B", "A")
    model.rename_particle("W_3", "Z")

    gY = model.coupling("g_Y")
    gL = model.coupling("g_L")
    theta_W = constant("theta_W")
    e_em = constant("e_em")
    model.replace(gL * gL + gY * gY, (gL / cos(theta_W)) ** 2)
    model.replace(gY, e_em / cos(theta_W))
    model.replace(gL, e_em / sin(theta_W))

    fi = flavor.index("fi")
    fj = flavor.index("fj")
    fk = flavor.index("fk")
    factor = sqrt(2) / v
    m_e = [
        constant("m_e;m_{e}"),
        constant("m_mu;m_{\\mu}"),
        constant("m_tau;m_{\\tau}"),
    ]
    M_e = diagonal_tensor("M_e", flavor, m_e)
    model.replace(Ye(fi, fj), factor * M_e(fi, fj))

    m_u = [constant("m_u;m_{u}"), constant("m_c;m_{c}"), constant("m_t;m_{t}")]
    M_u = diagonal_tensor("M_u", flavor, m_u)
    model.replace(Yu(fi, fj), factor * M_u(fi, fj))

    m_d = [constant("m_d;m_{d}"), constant("m_s;m_{s}"), constant("m_b;m_{b}")]
    M_d = diagonal_tensor("M_d", flavor, m_d)
    V = unitary_tensor("V_CKM", flavor, ckm_matrix())
    model.replace(Yd(fi, fj), factor * V(fi, fk) * M_d(fk, fj))
    D_L = model.particle("D_L")
    dirac = model.generate_dirac_index()
    colour = model.generate_index("C", "D_L")
    model.replace(D_L(fj, colour, dirac), V(fj, fk) * D_L(fk, colour, dirac))

    break_sm_flavor(model)

    model.gather_mass("W")
    model.gather_mass("Z")
    _adjust(model)


def _adjust(model: Model) -> None:
    """Apply the substitutions of ``SM_Model::adjust``."""
    m_w = constant("M_W;M_{W}")
    m_z = constant("M_Z;M_{Z}")
    theta_w = constant("theta_W")
    e = constant("e_em")
    v = constant("v")
    model.replace(v, (2 * m_w * sin(theta_w)) / e)
    w = model.particle("W")
    z = model.particle("Z")
    model.replace(w.mass, m_w)
    w.mass = m_w
    model.replace(z.mass, m_z)
    z.mass = m_z
    model.promote_to_goldstone("Gp", "W")
    model.promote_to_goldstone("G0", "Z")
    _add_gauge_fixing_terms(model)


def _add_gauge_fixing_terms(model: Model) -> None:
    """Add the gauge-fixing terms of ``SM_Model::addGaugeFixingTerms``."""
    theta_w = constant("theta_W")
    e = constant("e_em")
    v = constant("v")
    m_w = constant("M_W;M_{W}")
    m_z = constant("M_Z;M_{Z}")
    cos_w = cos(theta_w)
    g = e / sin(theta_w)
    h0 = model.particle("h")()
    G0 = model.particle("G0")()
    Gp = model.particle("Gp")()
    ap = constant("ap;a_{p}")
    am = constant("am;a_{m}")
    az = constant("az;a_{z}")
    aa = constant("aa;a_{a}")
    delta_G0 = -g / 2 * (am * Gp + ap * Gp.conjugate()) + g / (2 * cos_w) * az * (
        v + h0
    )
    delta_Gp = (
        -I * g / 2 * (v + h0 + I * G0) * ap
        - I * g * cos(2 * theta_w) / (2 * cos_w) * Gp * az
        + I * e * Gp * aa
    )
    delta_Gm = delta_Gp.replace({ap: am}).conjugate()
    xi_w = model.particle("W").gauge_parameter
    xi_z = model.particle("Z").gauge_parameter
    dF_Wp = -I * m_w * xi_w * delta_Gp
    dF_Wm = I * m_w * xi_w * delta_Gm
    dF_Z = -m_z * xi_z * delta_G0

    c_A = model.particle("c_A")()
    c_Z = model.particle("c_Z")()
    c_Wp = model.particle("c_Wp")()
    c_Wm = model.particle("c_Wm")()
    for c1, delta in [(c_Z, dF_Z), (c_Wp, dF_Wp), (c_Wm, dF_Wm)]:
        for a, c2 in [(aa, c_A), (az, c_Z), (ap, c_Wp), (am, c_Wm)]:
            term = delta.diff(a)
            if term != 0:
                model.add_term(c1.conjugate() * term * c2)
