"""Partial pure-pymarty port of the two Higgs doublet model (src/marty/models/thdm.cpp).

The C++ ``THDM_Model<2>`` (type II, MSSM-like charges) is ported in the order of
``THDM_Model::init``: content, gauge symmetry breaking, Higgs VEV expansion,
2x2 rotations, adjust.

Ported: gauge groups C, L, Y; the fermions Q, U_R, D_R, L, E_R; the Higgs
doublets Phi1 and Phi2 with the potential; the gauge breaking; the Higgs VEV
expansion; the 2x2 rotations; the W_1/W_2 replacements; the gauge-boson masses
and the adjust substitutions.

Not ported (the declarative API has no binding for these C++ calls):
  - the Yukawa terms of ``THDM_Model::initYukawas``: the flavour tensors Y1u,
    ... must be indexed with the model's own ``SM_flavor`` space
    (``getVectorSpace("SM_flavor")``), which ``Model.generate_index`` does not
    expose (its ``Index.space`` is ``None``), and ``addTensorCoupling`` is absent;
  - flavour symmetry breaking (``SM_Model::flavorSymmetryBreaking``), so the
    fermions keep the ``SM_flavor`` index instead of being split by generation;
  - ``replaceYukawas`` (Z2 charge coefficients, Yukawa mass matrices and the CKM
    matrix from ``buildCKM``);
  - ``DiagonalizeMassMatrices`` for W_3 and B: the rotation to Z and A is written
    out explicitly, as in the gettingStarted port;
  - ghost-boson replacements (``c_W_1``, ``c_W_2``) of
    ``SM_Model::gaugeSymmetryBreaking``;
  - ``getMass``/``setMass`` (replaced by ``add_bosonic_mass``) and ``mergeTerms``;
  - the numerical ``sm_input`` values: masses and couplings stay symbolic.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model
from pymarty.symbolic import Expr, I, Minkowski, constant, cos, sin, sqrt, tan


def build() -> Model:
    """Return the ported part of the two Higgs doublet model (type II)."""
    model = Model(
        gauge=[
            GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
            GaugeGroup(U(1), name="Y", boson_name="B", coupling="g_Y"),
        ],
        flavor=[FlavorGroup("SM_flavor", 3)],
    )
    _add_fermions(model)
    _add_higgs_doublets(model)
    _add_higgs_potential(model)

    model.break_gauge_symmetry("Y")
    model.break_gauge_symmetry(
        "L", renaming={"Q": ["U_L", "D_L"], "L": ["Nu_L;\\nu_{L}", "E_L"]}
    )

    _replace_higgs(model)
    _diagonalize_2by2(model)
    _replace_w_bosons(model)
    _adjust(model)
    model.finalize()
    return model


def _add_fermions(model: Model) -> None:
    """Add the fermion content of ``SM_Model::initFermions``."""
    model.add_weyl_fermion(
        "Q",
        chirality="L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "U_R",
        chirality="R",
        gauge_rep={"C": (1, 0), "Y": Fraction(2, 3)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "D_R",
        chirality="R",
        gauge_rep={"C": (1, 0), "Y": Fraction(-1, 3)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "E_R",
        chirality="R",
        gauge_rep={"Y": -1},
        flavor_rep="SM_flavor",
    )


def _add_higgs_doublets(model: Model) -> None:
    """Add Phi1 and Phi2 of ``THDM_Model::initHiggsSector``."""
    for name in ("Phi1 ; \\Phi_{1}", "Phi2 ; \\Phi_{2}"):
        model.add_scalar_boson(
            name,
            gauge_rep={"L": 1, "Y": Fraction(1, 2)},
            self_conjugate=False,
        )


def _add_higgs_potential(model: Model) -> None:
    """Add the Higgs potential of ``THDM_Model::initHiggsSector``."""
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


def _replace_higgs(model: Model) -> None:
    """Expand the Higgs doublets around their vacuum (``THDM_Model::replaceHiggs``)."""
    v1 = constant("v1")
    beta = constant("beta;\\beta")
    tan_beta = tan(beta)
    v2 = v1 * tan_beta
    m11 = constant("m_11")
    m12 = constant("m_12")
    m22 = constant("m_22")
    lambda1 = constant("lambda_1;\\lambda_{1}")
    lambda2 = constant("lambda_2;\\lambda_{2}")
    lambda3 = constant("lambda_3;\\lambda_{3}")
    lambda4 = constant("lambda_4;\\lambda_{4}")
    lambda5 = constant("lambda_5;\\lambda_{5}")

    phi1_charged = model.add_scalar_boson("phi_1", self_conjugate=False)
    phi2_charged = model.add_scalar_boson("phi_2", self_conjugate=False)
    rho_1 = model.add_scalar_boson("rho_1", self_conjugate=True)
    rho_2 = model.add_scalar_boson("rho_2", self_conjugate=True)
    eta_1 = model.add_scalar_boson("eta_1", self_conjugate=True)
    eta_2 = model.add_scalar_boson("eta_2", self_conjugate=True)

    model.replace(model.particle("Phi1_1"), phi1_charged())
    model.replace(model.particle("Phi1_2"), (rho_1() + I * eta_1() + v1) / sqrt(2))
    model.replace(model.particle("Phi2_1"), phi2_charged())
    model.replace(model.particle("Phi2_2"), (rho_2() + I * eta_2() + v2) / sqrt(2))

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


def _diagonalize_2by2(model: Model) -> None:
    """Rotate the Higgs fields (``THDM_Model::diagonalize2By2Matrices``)."""
    alpha = constant("alpha;\\alpha")
    beta = constant("beta;\\beta")
    beta_rotation = [[cos(beta), -sin(beta)], [sin(beta), cos(beta)]]

    eta_1 = model.particle("eta_1")
    eta_2 = model.particle("eta_2")
    goldstone_0 = model.add_scalar_boson("G0; G^0", self_conjugate=True)
    pseudoscalar = model.add_scalar_boson("A0; A^0", self_conjugate=True)
    model.rotate_fields(
        [eta_1, eta_2],
        [goldstone_0, pseudoscalar],
        beta_rotation,
        diagonalize_masses=True,
        n_mass_less_fields=1,
    )

    phi_1 = model.particle("phi_1")
    phi_2 = model.particle("phi_2")
    goldstone_p = model.add_scalar_boson("Gp; G^+", self_conjugate=False)
    charged_higgs = model.add_scalar_boson("Hp; H^+", self_conjugate=False)
    model.rotate_fields(
        [phi_1, phi_2],
        [goldstone_p, charged_higgs],
        beta_rotation,
        diagonalize_masses=True,
        n_mass_less_fields=1,
    )

    rho_1 = model.particle("rho_1")
    rho_2 = model.particle("rho_2")
    h = model.add_scalar_boson("h; h^0", self_conjugate=True)
    heavy_h = model.add_scalar_boson("H0; H^0", self_conjugate=True)
    model.rotate_fields(
        [rho_1, rho_2],
        [h, heavy_h],
        [[-sin(alpha), cos(alpha)], [cos(alpha), sin(alpha)]],
        diagonalize_masses=True,
    )


def _replace_w_bosons(model: Model) -> None:
    """Express W_1, W_2 by W and the neutral gauge bosons by Z and A.

    This is the explicit form of the gauge breaking used by the gettingStarted
    port; the C++ code additionally handles the ghosts, which are not ported.
    """
    w_1 = model.particle("W_1")
    w_2 = model.particle("W_2")
    w = model.create_similar_particle("W", reference="W_1", self_conjugate=False)
    mu = Minkowski.index()
    nu = Minkowski.index()
    model.replace(w_1(mu), (w(mu) + w(mu).conjugate()) / sqrt(2))
    model.replace(
        w_1.field_strength()(mu, nu), (w(mu, nu) + w(mu, nu).conjugate()) / sqrt(2)
    )
    model.replace(w_2(mu), I * (w(mu) - w(mu).conjugate()) / sqrt(2))
    model.replace(
        w_2.field_strength()(mu, nu), I * (w(mu, nu) - w(mu, nu).conjugate()) / sqrt(2)
    )

    theta_w = constant("theta_W;\\theta_{W}")
    c_w = cos(theta_w)
    s_w = sin(theta_w)
    model.rotate_fields(["W_3", "B"], ["Z", "A"], [[c_w, s_w], [-s_w, c_w]])

    g_l = model.coupling("g_L")
    g_y = model.coupling("g_Y")
    e = constant("e_em")
    model.replace(g_l**2 + g_y**2, (g_l / c_w) ** 2)
    model.replace(g_y, e / c_w)
    model.replace(g_l, e / s_w)


def _adjust(model: Model) -> None:
    """Apply the substitutions of ``THDM_Model::adjust``."""
    m_w = constant("M_W")
    m_z = constant("M_Z")
    theta_w = constant("theta_W;\\theta_{W}")
    e = constant("e_em")
    v = constant("v")
    v1 = constant("v1")
    beta = constant("beta;\\beta")
    tan_beta = tan(beta)

    model.add_bosonic_mass(model.particle("W"), m_w)
    model.add_bosonic_mass(model.particle("Z"), m_z)
    model.promote_to_goldstone("Gp", "W")
    model.promote_to_goldstone("G0", "Z")

    model.replace(tan_beta + 1 / tan_beta, (1 + tan_beta * tan_beta) / tan_beta)
    model.replace(v1, v / sqrt(1 + tan_beta * tan_beta))
    model.replace(v, (2 * m_w * sin(theta_w)) / e)
    model.replace(
        1 / (1 + tan_beta * tan_beta) + tan_beta * tan_beta / (1 + tan_beta * tan_beta),
        Expr(1),
    )
    model.replace(1 + tan_beta**-2, 1 / sin(beta) ** 2)
    model.replace(1 + tan_beta**2, 1 / cos(beta) ** 2)
    model.replace(tan(beta), sin(beta) / cos(beta))
