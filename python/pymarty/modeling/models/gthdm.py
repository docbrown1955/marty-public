"""Partial pure-pymarty port of the generic two Higgs doublet model.

Ported from src/marty/models/gthdm.cpp. The content, gauge symmetry breaking,
Higgs expansion, rotations, W and ghost bosons, flavour symmetry breaking and
adjust steps are shared with THDM (``add_thdm_content``). Only the Yukawa
replacement differs: it is ``replace_gthdm_yukawas``, with the xi
parametrisation of the second doublet couplings and the generic CKM matrix.

Not ported, as for THDM: ``getMass``/``setMass`` and ``mergeTerms``.
"""

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model
from pymarty.modeling.models.sm import ckm_matrix
from pymarty.modeling.models.thdm import add_thdm_content
from pymarty.symbolic import (
    Expr,
    Space,
    Tensor,
    constant,
    cos,
    diagonal_tensor,
    matrix_tensor,
    sin,
    sqrt,
    tan,
    unitary_tensor,
)

FLAVOR_GROUP = "SM_flavor"


class GTHDM(Model):
    """Ported generic two Higgs doublet model (src/marty/models/gthdm.cpp).

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(
            gauge=[
                GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
                GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
                GaugeGroup(U(1), name="Y", boson_name="B", coupling="g_Y"),
            ],
            flavor=[FlavorGroup(FLAVOR_GROUP, 3)],
        )
        add_thdm_content(self, replace_gthdm_yukawas)
        if finalize:
            self.finalize()


def _xi_matrix(name: str, flavor: Space, a: Expr, b: Expr, c: Expr) -> Tensor:
    """Symmetric xi matrix with a zero first row and column, as in the C++ model."""
    zero = Expr(0)
    return matrix_tensor(name, flavor, [[zero, zero, zero], [zero, a, b], [zero, b, c]])


def replace_gthdm_yukawas(model: Model, yukawas: dict[str, Tensor]) -> None:
    """Port of ``GTHDM_Model::replaceYukawas``.

    The first doublet couplings are expressed by the second ones, which are then
    expressed by the xi parameters and the mass matrices. The C++ order is kept.
    """
    flavor = model.vector_space(FLAVOR_GROUP)
    fi = flavor.index("fi")
    fj = flavor.index("fj")
    fk = flavor.index("fk")
    fl = flavor.index("fl")
    v = constant("v")
    beta = constant("beta;\\beta")
    cos_beta = cos(beta)
    sin_beta = sin(beta)
    tan_beta = tan(beta)
    sqrt2 = sqrt(2)

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

    D_L = model.particle("D_L")
    D_R = model.particle("D_R")
    dirac = model.generate_dirac_index()
    colour = model.generate_index("C", "D_L")
    model.replace(D_L(fj, colour, dirac), V(fj, fk) * D_L(fk, colour, dirac))
    model.replace(D_R(fi, colour, dirac), V(fi, fj) * D_R(fj, colour, dirac))

    Ye1, Yu1, Yd1 = yukawas["Y1e"], yukawas["Y1u"], yukawas["Y1d"]
    Ye2, Yu2, Yd2 = yukawas["Y2e"], yukawas["Y2u"], yukawas["Y2d"]
    model.replace(
        Ye1(fi, fj),
        sqrt2 / (v * cos_beta) * M_e(fi, fj) - tan_beta * Ye2(fi, fj),
    )
    model.replace(
        Yd1(fi, fj),
        V(fi, fk) * (sqrt2 / (v * cos_beta) * M_d(fk, fl)) * V(fj, fl).conjugate()
        - tan_beta * Yd2(fi, fj),
    )
    model.replace(
        Yu1(fi, fj),
        sqrt2 / (v * cos_beta) * M_u(fi, fj) - tan_beta * Yu2(fi, fj),
    )

    xil = _xi_matrix(
        "xil",
        flavor,
        constant("xil_mm;\\xi^{l}_{mm}"),
        constant("xil_mt;\\xi^{l}_{mt}"),
        constant("xil_tt;\\xi^{l}_{tt}"),
    )
    xiu = _xi_matrix(
        "xiu",
        flavor,
        constant("xiu_cc;\\xi^{u}_{cc}"),
        constant("xiu_ct;\\xi^{u}_{ct}"),
        constant("xiu_tt;\\xi^{u}_{tt}"),
    )
    xid = _xi_matrix(
        "xid",
        flavor,
        constant("xid_ss;\\xi^{d}_{ss}"),
        constant("xid_sb;\\xi^{d}_{sb}"),
        constant("xid_bb;\\xi^{d}_{bb}"),
    )
    model.replace(
        Ye2(fi, fj),
        cos_beta * xil(fi, fj) + sqrt2 / v * sin_beta * M_e(fi, fj),
    )
    model.replace(
        Yu2(fi, fj),
        cos_beta * xiu(fi, fj) + sqrt2 / v * sin_beta * M_u(fi, fj),
    )
    model.replace(
        Yd2(fi, fj),
        cos_beta * xid(fi, fj)
        + V(fi, fk) * (sqrt2 / v * sin_beta * M_d(fk, fl)) * V(fj, fl).conjugate(),
    )
