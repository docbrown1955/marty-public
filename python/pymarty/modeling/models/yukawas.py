"""Yukawa terms of the two Higgs doublet models, shared by THDM and GTHDM.

``add_yukawas`` ports ``THDM_Model::initYukawas`` and ``replace_yukawas`` ports
``THDM_Model::replaceYukawas`` for type II. The GTHDM port uses ``add_yukawas``
and overrides the replacement, as ``GTHDM_Model::replaceYukawas`` does in C++.
"""

from pymarty.modeling import Model
from pymarty.modeling.models.sm import ckm_matrix
from pymarty.symbolic import (
    ComplexProperty,
    Expr,
    Tensor,
    constant,
    diagonal_tensor,
    epsilon,
    sqrt,
    tan,
    tensor,
    unitary_tensor,
)

FLAVOR = "SM_flavor"


def add_yukawas(model: Model) -> dict[str, Tensor]:
    """Add the Yukawa terms of ``THDM_Model::initYukawas``, one set per doublet.

    Returns the Yukawa tensors by name, for ``replace_yukawas``.
    """
    flavor = model.vector_space(FLAVOR)
    tensors = {
        name: tensor(name, [flavor, flavor], complex_property=ComplexProperty.Complex)
        for name in ("Y1u", "Y1d", "Y1e", "Y2u", "Y2d", "Y2e")
    }
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

    for doublet, Yu, Yd, Ye in (
        ("Phi1", tensors["Y1u"], tensors["Y1d"], tensors["Y1e"]),
        ("Phi2", tensors["Y2u"], tensors["Y2d"], tensors["Y2e"]),
    ):
        phi = model.particle(doublet)
        model.add_term(
            Yu(fI, fJ)
            * phi(il).conjugate()
            * epsilon(il, jl)
            * Q(fI, a, jl, al).conjugate()
            * U_R(fJ, a, al),
            add_hermitic=True,
        )
        model.add_term(
            -Yd(fI, fJ) * phi(il) * Q(fI, a, il, al).conjugate() * D_R(fJ, a, al),
            add_hermitic=True,
        )
        model.add_term(
            -Ye(fI, fJ) * phi(il) * L(fI, il, al).conjugate() * E_R(fJ, al),
            add_hermitic=True,
        )
    for name in ("Y1e", "Y1u", "Y1d", "Y2e", "Y2u", "Y2d"):
        model.add_tensor_coupling(tensors[name])
    return tensors


def replace_yukawas(model: Model, yukawas: dict[str, Tensor]) -> None:
    """Apply the Z2 coefficients, the Yukawa mass matrices and the CKM rotation.

    Port of ``THDM_Model::replaceYukawas`` for type II. The substitutions are
    applied in the C++ order, because a later one acts on the terms created by an
    earlier one (e.g. ``Y2u`` is first set to ``Y1u``, then ``Y1u`` is replaced).
    """
    flavor = model.vector_space(FLAVOR)
    fi = flavor.index("fi")
    fj = flavor.index("fj")
    fk = flavor.index("fk")
    fl = flavor.index("fl")
    v1 = constant("v1")
    tan_beta = tan(constant("beta;\\beta"))
    v2 = v1 * tan_beta
    v = constant("v")

    def z2_coefs(charge: int) -> tuple[Expr, Expr]:
        # Z2_coef in src/marty/models/thdm.cpp
        return (Expr(0), Expr(1)) if charge == 1 else (Expr(1), Expr(0))

    def z2_mass_coef(charge: int) -> Expr:
        # Z2_mass_coef in src/marty/models/thdm.cpp
        return sqrt(2) / (v2 if charge == 1 else v1)

    # Type II charges (Z2_charges<2>): u = 1, d = -1, e = -1
    charges = {"e": -1, "u": 1, "d": -1}
    for suffix in ("e", "u", "d"):
        first, second = z2_coefs(charges[suffix])
        y1 = yukawas[f"Y1{suffix}"]
        y2 = yukawas[f"Y2{suffix}"]
        # Bare tensors on the left, as in the C++ Replaced calls: an indexed
        # left-hand side cannot be replaced by zero.
        model.replace(y1, first * y1(fi, fj))
        model.replace(y2, second * y1(fi, fj))

    m_e = [
        constant("m_e;m_{e}"),
        constant("m_mu;m_{\\mu}"),
        constant("m_tau;m_{\\tau}"),
    ]
    m_u = [constant("m_u;m_{u}"), constant("m_c;m_{c}"), constant("m_t;m_{t}")]
    m_d = [constant("m_d;m_{d}"), constant("m_s;m_{s}"), constant("m_b;m_{b}")]
    M_e = diagonal_tensor("M_e", flavor, m_e)
    M_u = diagonal_tensor("M_u", flavor, m_u)
    M_d = diagonal_tensor("M_d", flavor, m_d)
    V = unitary_tensor("V_CKM", flavor, ckm_matrix())

    model.replace(yukawas["Y1e"](fi, fj), z2_mass_coef(charges["e"]) * M_e(fi, fj))
    model.replace(yukawas["Y1u"](fi, fj), z2_mass_coef(charges["u"]) * M_u(fi, fj))
    # Y_d = V M_d V^dagger, as in the C++ Hermitian conjugate of V_CKM.
    model.replace(
        yukawas["Y1d"](fi, fj),
        z2_mass_coef(charges["d"]) * V(fi, fk) * M_d(fk, fl) * V(fj, fl).conjugate(),
    )

    D_L = model.particle("D_L")
    D_R = model.particle("D_R")
    dirac = model.generate_dirac_index()
    colour = model.generate_index("C", "D_L")
    model.replace(D_L(fj, colour, dirac), V(fj, fk) * D_L(fk, colour, dirac))
    model.replace(D_R(fi, colour, dirac), V(fi, fj) * D_R(fj, colour, dirac))
    model.replace(1 + tan_beta**-2, v * v / (v2 * v2))
