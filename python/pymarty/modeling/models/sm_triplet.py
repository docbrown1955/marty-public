"""Toy SU(2)xU(1) model with a scalar triplet (tests/system/src/SM_triplet.cpp).

The Lagrangian couples the lepton doublet to a Higgs doublet H, an inert
doublet eta and a scalar triplet Del, with three self-conjugate neutrinos N.
The model is then broken: the Yukawa mass matrices, the gauge symmetries, the
flavour symmetry, the Higgs mechanism, the field rotations and the
electroweak couplings are applied in the C++ order.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model, dirac_C, renamed_indices
from pymarty.modeling.models._symbols import constant
from pymarty.symbolic import (
    ComplexProperty,
    Expr,
    I,
    Tensor,
    cos,
    epsilon,
    matrix_tensor,
    sin,
    sqrt,
    tensor,
)

FLAVOR = "SM_flavor"


class SMTriplet(Model):
    """Toy model with a scalar triplet, broken as in SM_triplet.cpp.

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[
                GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
                GaugeGroup(U(1), name="Y", boson_name="B", coupling="g_Y"),
            ],
            flavor=[FlavorGroup(FLAVOR, 3)],
        )
        yukawas = _add_content(self)
        _break_model(self, yukawas)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> dict[str, Tensor]:
    """Add the particles and the Lagrangian; return the Yukawa-like tensors."""
    model.add_scalar_boson(
        "H", gauge_rep={"L": 1, "Y": Fraction(-1, 2)}, self_conjugate=False
    )
    model.add_scalar_boson("Del", gauge_rep={"L": 2, "Y": -1}, self_conjugate=False)
    model.add_scalar_boson(
        "et", gauge_rep={"L": 1, "Y": Fraction(-1, 2)}, self_conjugate=False
    )
    model.add_weyl_fermion(
        "L_L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion("E_R", chirality="R", gauge_rep={"Y": -1}, flavor_rep=FLAVOR)
    model.add_weyl_fermion(
        "N", chirality="R", gauge_rep={"Y": 0}, flavor_rep=FLAVOR, self_conjugate=True
    )

    H = model.particle("H")
    Del = model.particle("Del")
    et = model.particle("et")
    Li = model.particle("L_L")
    Ei = model.particle("E_R")
    Ni = model.particle("N")

    flavor_space = model.vector_space(FLAVOR)
    I_f = flavor_space.index()
    J_f = flavor_space.index()
    i = model.generate_index("L", "L_L")
    j = model.generate_index("L", "L_L")
    eps = epsilon(i, j)
    pauli = model.get_generator("L", "L_L")
    ii = [model.generate_index("L", "L_L") for _ in range(3)]
    A = model.generate_index("L", "Del")
    B = model.generate_index("L", "Del")
    a = model.generate_dirac_index()
    al = [model.generate_dirac_index(), model.generate_dirac_index()]
    C = dirac_C

    Ye = tensor(
        "Y_e", [flavor_space, flavor_space], complex_property=ComplexProperty.Complex
    )
    Ynu = tensor(
        "Y_{\\nu}",
        [flavor_space, flavor_space],
        complex_property=ComplexProperty.Complex,
    )
    MN = tensor("M_N", [flavor_space, flavor_space])

    muH2 = constant("muH2")
    muet2 = constant("muet2")
    muD2 = constant("muD2")
    muetD = constant("\\mu_{\\eta \\Delta}")
    muHD = constant("\\mu_{H \\Delta}")
    lh = constant("\\lambdaH")
    let = constant("\\lambda_{\\eta}")
    leth1 = constant("\\lambda_{\\eta H}")
    leth2 = constant("\\lambda^{\\prime}}_{\\eta H}")
    lD = constant("\\lambda_{\\Delta}")

    model.add_term(
        -H(i) * eps * Ei(I_f, a).conjugate() * Ye(I_f, J_f) * Li(J_f, j, a),
        add_hermitic=True,
    )
    model.add_term(
        -Ni(I_f, al[0])
        * Ynu(I_f, J_f)
        * et(i).conjugate()
        * C(al[0], al[1])
        * Li(J_f, i, al[1]),
        add_hermitic=True,
    )
    model.add_term(
        -Ni(I_f, al[0]) * MN(I_f, J_f) * C(al[0], al[1]) * Ni(J_f, al[1]),
        add_hermitic=True,
    )
    model.add_term(
        -Li(I_f, ii[0], al[0])
        * C(al[0], al[1])
        * Ynu(I_f, J_f)
        * Del(A).conjugate()
        * epsilon(ii[0], ii[1])
        * pauli(A, ii[1], ii[2])
        * Li(J_f, ii[2], al[1]),
        add_hermitic=True,
    )

    model.add_term(muH2 * H(i).conjugate() * H(i))
    prod = H(i).conjugate() * H(i)
    model.add_term(lh * prod * renamed_indices(prod))

    model.add_term(muet2 * et(i).conjugate() * et(i))
    prod1 = et(i).conjugate() * et(i)
    model.add_term(let * prod1 * renamed_indices(prod1))

    prod2 = H(i).conjugate() * et(i)
    model.add_term(leth1 * prod2 * renamed_indices(prod2), add_hermitic=True)
    model.add_term(leth2 * H(i).conjugate() * H(i) * et(j).conjugate() * et(j))

    model.add_term(
        muD2
        * Del(A)
        * pauli(A, ii[0], ii[1])
        * Del(B).conjugate()
        * pauli(B, ii[1], ii[0])
    )
    prod3 = (
        Del(A) * pauli(A, ii[0], ii[1]) * Del(B).conjugate() * pauli(B, ii[1], ii[0])
    )
    model.add_term(lD * prod3 * renamed_indices(prod3))

    model.add_term(
        muetD
        * et(ii[0])
        * Del(A).conjugate()
        * epsilon(ii[0], ii[1])
        * pauli(A, ii[1], ii[2])
        * et(ii[2]),
        add_hermitic=True,
    )
    model.add_term(
        muHD
        * H(ii[0])
        * Del(A).conjugate()
        * epsilon(ii[0], ii[1])
        * pauli(A, ii[1], ii[2])
        * H(ii[2]),
        add_hermitic=True,
    )
    return {"Y_e": Ye, "M_N": MN}


def _break_model(model: Model, tensors: dict[str, Tensor]) -> None:
    """Symmetry breaking, Higgs mechanism and field rotations of SM_triplet.cpp."""
    flavor_space = model.vector_space(FLAVOR)
    zero = Expr(0)
    m_e = constant("m_e")
    m_mu = constant("m_{\\mu}")
    m_tau = constant("m_{\\tau}")
    mN1 = constant("m_N1")
    mN2 = constant("m_N2")
    mN3 = constant("m_N3")
    vh = constant("vh")
    vD = constant("vD")

    M_l = matrix_tensor(
        "M_l",
        flavor_space,
        [[m_e, zero, zero], [zero, m_mu, zero], [zero, zero, m_tau]],
    )
    M_N = matrix_tensor(
        "M_N",
        flavor_space,
        [[mN1, zero, zero], [zero, mN2, zero], [zero, zero, mN3]],
    )
    f_i = flavor_space.index()
    f_j = flavor_space.index()
    model.replace(tensors["Y_e"], sqrt(2) / vh * M_l(f_i, f_j))
    model.replace(tensors["M_N"], M_N(f_i, f_j))

    model.break_gauge_symmetry("Y")
    model.break_gauge_symmetry("L")

    model.rename_particle("L_L_1", "Nu_L")
    model.rename_particle("L_L_2", "E_L")
    model.rename_particle("H_1", "H0")
    model.rename_particle("H_2", "H^-")
    model.rename_particle("et_1", "et0")
    model.rename_particle("et_2", "\\eta^-")

    model.break_flavor_symmetry(FLAVOR)
    model.rename_particle("E_L_1", "e_L")
    model.rename_particle("E_L_2", "\\mu_L")
    model.rename_particle("E_L_3", "\\tau_L")
    model.rename_particle("E_R_1", "e_R")
    model.rename_particle("E_R_2", "\\mu_R")
    model.rename_particle("E_R_3", "\\tau_R")

    for n in ("N_1", "N_2", "N_3"):
        model.promote_to_majorana(model.particle(n))

    _higgs_mechanism(model, vh, vD)
    _rotations_and_couplings(model)


def _higgs_mechanism(model: Model, vh: Expr, vD: Expr) -> None:
    """Replace the gauge bosons, triplet and doublets by their vev expansions."""
    W_1 = model.particle("W_1")
    W_2 = model.particle("W_2")
    W = W_1.generate_similar_particle("W")
    W.set_self_conjugate(False)
    mu = model.generate_minkowski_index("mu")
    nu = model.generate_minkowski_index("nu")
    model.replace(W_1(mu), (W(mu) + W(mu).conjugate()) / sqrt(2))
    model.replace(
        W_1.field_strength()(mu, nu), (W(mu, nu) + W(mu, nu).conjugate()) / sqrt(2)
    )
    model.replace(W_2(mu), I * (W(mu) - W(mu).conjugate()) / sqrt(2))
    model.replace(
        W_2.field_strength()(mu, nu), I * (W(mu, nu) - W(mu, nu).conjugate()) / sqrt(2)
    )

    Del_1 = model.particle("Del_1")
    Del_2 = model.particle("Del_2")
    Del_3 = model.particle("Del_3")
    Dmm = Del_1.generate_similar_particle("\\Delta^{--}")
    Dmm.set_self_conjugate(False)
    Dm = Del_2.generate_similar_particle("\\Delta^-")
    Dm.set_self_conjugate(False)
    D0 = Del_3.generate_similar_particle("D0")
    DR = D0.generate_similar_particle("\\Delta_R")
    DR.set_self_conjugate(True)
    DI = D0.generate_similar_particle("\\Delta_I")
    DI.set_self_conjugate(True)

    model.replace(Del_1, (Dmm() + D0()) / sqrt(2))
    model.replace(Del_2, I * (D0() - Dmm()) / sqrt(2))
    model.replace(Del_3, Dm() / sqrt(2))
    model.replace(D0, (vD + DR() + I * DI()) / sqrt(2))

    H0 = model.particle("H0")
    hh = H0.generate_similar_particle("hh")
    hh.set_self_conjugate(True)
    Ah = H0.generate_similar_particle("Ah")
    Ah.set_self_conjugate(True)
    model.replace(H0, (vh + hh() + I * Ah()) / sqrt(2))

    et0 = model.particle("et0")
    etR = et0.generate_similar_particle("\\eta_R")
    etR.set_self_conjugate(True)
    etI = et0.generate_similar_particle("\\eta_I")
    etI.set_self_conjugate(True)
    model.replace(et0, (etR() + I * etI()) / sqrt(2))


def _rotations_and_couplings(model: Model) -> None:
    """Field rotations to mass eigenstates, Goldstones and the electroweak couplings."""
    p = model.particle
    model.rotate_fields([p("\\eta_R"), p("\\eta_I")], diagonalize_masses=True)
    model.rotate_fields([p("hh"), p("\\Delta_R")], diagonalize_masses=True)
    p("\\Delta_R").width = constant("GDR")
    model.rotate_fields([p("Ah"), p("\\Delta_I")], diagonalize_masses=True)
    p("\\Delta_I").width = constant("GDI")
    model.rotate_fields([p("H^-"), p("\\Delta^-")], diagonalize_masses=True)
    p("\\Delta^-").width = constant("GDm")

    for n in ("Nu_L_1", "Nu_L_2", "Nu_L_3"):
        p(n).set_self_conjugate(True)
    for n in ("Nu_L_1", "Nu_L_2", "Nu_L_3"):
        model.promote_to_majorana(p(n))
    model.rename_particle("Nu_L_1", "nu_e ; \\nu _e")
    model.rename_particle("Nu_L_2", "nu_mu ; \\nu _{\\mu}")
    model.rename_particle("Nu_L_3", "nu_tau ; \\nu _{\\tau}")
    model.rotate_fields([p("nu_e"), p("nu_mu"), p("nu_tau")], diagonalize_masses=True)
    p("\\Delta^{--}").width = constant("GDmm")

    thetaW = constant("thetaW")
    cW = cos(thetaW)
    sW = sin(thetaW)
    model.rotate_fields(["W_3", "B"], ["Z", "A"], [[cW, sW], [-sW, cW]])
    model.promote_to_goldstone("H^-", "W")
    model.promote_to_goldstone("Ah", "Z")

    e = constant("e")
    model.replace(model.coupling("g_Y"), e / cW)
    model.replace(model.coupling("g_L"), e / sW)
