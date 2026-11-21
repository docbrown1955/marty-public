"""Partial pure-pymarty port of the C++ generic two Higgs doublet model.

Ported from src/marty/models/gthdm.cpp and the THDM_Model<2> base in thdm.h,
following the order of THDM_Model::init: content (gauge, fermions, Higgs
potential, Yukawa Lagrangian), gauge symmetry breaking, the Higgs VEV
expansion, the 2x2 rotations, and the W+- replacements.

Not ported, because the declarative API has no binding for them:

* flavor symmetry breaking (SM_Model::flavorSymmetryBreaking), so the
  fermions keep their SM_flavor index instead of being split into generations;
* the Yukawa Lagrangian of initYukawas: its flavor tensors Y1u, ..., Y2e must
  be built on the model's own flavor space, and Python can only create a new
  Space("SM_flavor", 3), whose indices the model fields reject. Missing
  binding: a way to obtain the Space of a model flavor group (or a
  Model-level Yukawa/tensor-coupling builder, i.e. addTensorCoupling);
* the Yukawa replacements of replaceYukawas (Z2 coefficients, mass matrices,
  CKM matrix);
* ghost bosons (c_W_1, c_W_2, c_Wp, c_Wm, and the Goldstone ghosts);
* DiagonalizeMassMatrices, the W_3/B mixing and the g_L/g_Y replacements
  by e/sin(theta_W) and e/cos(theta_W);
* getMass/setMass and the trigonometric replacements of adjust();
* mergeTerms.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model
from pymarty.symbolic import Expr, I, Minkowski, constant, cos, sin, sqrt, tan

FLAVOR_GROUP = "SM_flavor"


def _add_fermions(model: Model) -> None:
    fermions: list[tuple[str, str, dict[str, object]]] = [
        ("Q", "L", {"C": (1, 0), "L": 1, "Y": Fraction(1, 6)}),
        ("U_R", "R", {"C": (1, 0), "Y": Fraction(2, 3)}),
        ("D_R", "R", {"C": (1, 0), "Y": Fraction(-1, 3)}),
        ("L", "L", {"L": 1, "Y": Fraction(-1, 2)}),
        ("E_R", "R", {"Y": Fraction(-1)}),
    ]
    for name, chirality, reps in fermions:
        model.add_weyl_fermion(
            name,
            chirality=chirality,
            gauge_rep=reps,
            flavor_rep=FLAVOR_GROUP,
        )


def _add_scalars(model: Model) -> None:
    for name in ("Phi1 ; \\Phi_{1}", "Phi2 ; \\Phi_{2}"):
        model.add_scalar_boson(
            name,
            gauge_rep={"L": 1, "Y": Fraction(1, 2)},
            self_conjugate=False,
        )


def _add_higgs_potential(model: Model) -> None:
    m11 = constant("m_11")
    m12 = constant("m_12")
    m22 = constant("m_22")
    lambda1 = constant("lambda_1;\\lambda_{1}")
    lambda2 = constant("lambda_2;\\lambda_{2}")
    lambda3 = constant("lambda_3;\\lambda_{3}")
    lambda4 = constant("lambda_4;\\lambda_{4}")
    lambda5 = constant("lambda_5;\\lambda_{5}")

    phi1 = model.particle("Phi1")
    phi2 = model.particle("Phi2")
    i = model.generate_index("L", "Phi1")
    j = model.generate_index("L", "Phi1")
    s11 = phi1(i).conjugate() * phi1(i)
    s12 = phi1(i).conjugate() * phi2(i)
    s21 = phi2(j).conjugate() * phi1(j)
    s22 = phi2(j).conjugate() * phi2(j)

    model.add_term(-m11 * m11 * s11)
    model.add_term(m12 * m12 * (s12 + s21))
    model.add_term(-m22 * m22 * s22)
    model.add_term(-lambda1 / 2 * s11**2)
    model.add_term(-lambda2 / 2 * s22**2)
    model.add_term(-lambda3 * s11 * s22)
    model.add_term(-lambda4 * s12 * s21)
    model.add_term(-lambda5 / 2 * (s12**2 + s21**2))


def _break_gauge_and_expand_higgs(model: Model) -> None:
    model.break_gauge_symmetry("Y")
    model.break_gauge_symmetry(
        "L",
        renaming={
            "Q": ["U_L", "D_L"],
            "L": ["Nu_L;\\nu_{L}", "E_L"],
        },
    )

    v1 = constant("v1")
    beta = constant("beta;\\beta")
    tan_beta = tan(beta)
    v2 = v1 * tan_beta

    phi1_c = model.add_scalar_boson("phi_1", self_conjugate=False)
    phi2_c = model.add_scalar_boson("phi_2", self_conjugate=False)
    rho_1 = model.add_scalar_boson("rho_1", self_conjugate=True)
    rho_2 = model.add_scalar_boson("rho_2", self_conjugate=True)
    eta_1 = model.add_scalar_boson("eta_1", self_conjugate=True)
    eta_2 = model.add_scalar_boson("eta_2", self_conjugate=True)

    model.replace(model.particle("Phi1_1"), phi1_c.expr())
    model.replace(
        model.particle("Phi1_2"),
        (rho_1.expr() + I * eta_1.expr() + v1) / sqrt(2),
    )
    model.replace(model.particle("Phi2_1"), phi2_c.expr())
    model.replace(
        model.particle("Phi2_2"),
        (rho_2.expr() + I * eta_2.expr() + v2) / sqrt(2),
    )

    m11 = constant("m_11")
    m12 = constant("m_12")
    m22 = constant("m_22")
    lambda1 = constant("lambda_1;\\lambda_{1}")
    lambda2 = constant("lambda_2;\\lambda_{2}")
    lambda3 = constant("lambda_3;\\lambda_{3}")
    lambda4 = constant("lambda_4;\\lambda_{4}")
    lambda5 = constant("lambda_5;\\lambda_{5}")
    model.replace(
        m11**2,
        -lambda1 / 2 * v1 * v1
        + v2 / v1 * m12 * m12
        - v2 * v2 / 2 * lambda3
        - v2 * v2 / 2 * lambda4
        - v2 * v2 / 2 * lambda5,
    )
    model.replace(
        m22**2,
        -lambda2 / 2 * v2 * v2
        + v1 / v2 * m12 * m12
        - v1 * v1 / 2 * lambda3
        - v1 * v1 / 2 * lambda4
        - v1 * v1 / 2 * lambda5,
    )


def _rotation(theta: Expr) -> list[list[Expr]]:
    return [[cos(theta), -sin(theta)], [sin(theta), cos(theta)]]


def _diagonalize_2x2(model: Model) -> None:
    beta = constant("beta;\\beta")
    alpha = constant("alpha;\\alpha")

    G0 = model.add_scalar_boson("G0; G^0", self_conjugate=True)
    A0 = model.add_scalar_boson("A0; A^0", self_conjugate=True)
    model.rotate_fields(
        [model.particle("eta_1"), model.particle("eta_2")],
        [G0, A0],
        _rotation(beta),
        diagonalize_masses=True,
        n_mass_less_fields=1,
    )

    Gp = model.add_scalar_boson("Gp; G^+", self_conjugate=False)
    Hp = model.add_scalar_boson("Hp; H^+", self_conjugate=False)
    model.rotate_fields(
        [model.particle("phi_1"), model.particle("phi_2")],
        [Gp, Hp],
        _rotation(beta),
        diagonalize_masses=True,
        n_mass_less_fields=1,
    )

    h = model.add_scalar_boson("h; h^0", self_conjugate=True)
    H = model.add_scalar_boson("H0; H^0", self_conjugate=True)
    model.rotate_fields(
        [model.particle("rho_1"), model.particle("rho_2")],
        [h, H],
        [
            [-sin(alpha), cos(alpha)],
            [cos(alpha), sin(alpha)],
        ],
        diagonalize_masses=True,
    )


def _replace_w_bosons(model: Model) -> None:
    W1 = model.particle("W_1")
    W2 = model.particle("W_2")
    W = model.add_similar_particle("W", reference="W_1", self_conjugate=False)

    mu = Minkowski.index()
    nu = Minkowski.index()
    W1_expr = (W(mu) + W(mu).conjugate()) / sqrt(2)
    W2_expr = I * (W(mu) - W(mu).conjugate()) / sqrt(2)
    F_W1 = (W(mu, nu) + W(mu, nu).conjugate()) / sqrt(2)
    F_W2 = I * (W(mu, nu) - W(mu, nu).conjugate()) / sqrt(2)
    model.replace(W1(mu), W1_expr)
    model.replace(W2(mu), W2_expr)
    model.replace(W1.field_strength()(mu, nu), F_W1)
    model.replace(W2.field_strength()(mu, nu), F_W2)


def build() -> Model:
    """Return the partially ported generic two Higgs doublet model."""
    model = Model(
        gauge=[
            GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
            GaugeGroup(U(1), name="Y", boson_name="B", coupling="g_Y"),
        ],
        flavor=[FlavorGroup(FLAVOR_GROUP, 3)],
    )
    _add_fermions(model)
    _add_scalars(model)
    _add_higgs_potential(model)
    _break_gauge_and_expand_higgs(model)
    _replace_w_bosons(model)
    _diagonalize_2x2(model)
    model.promote_to_goldstone("Gp", "W")
    model.finalize()
    return model
