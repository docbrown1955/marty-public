"""Toy model of NLepto.cpp: heavy neutral leptons coupled to a scalar singlet.

Mirrors the model building, breaking and field rotations of
tests/system/src/NLepto.cpp. The library generation is left to phase 6.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model, renamed_indices
from pymarty.modeling.models._symbols import constant
from pymarty.symbolic import (
    ComplexProperty,
    cos,
    epsilon,
    sin,
    sqrt,
    tensor,
)

FLAVOR = "SM_flavor"


class NLepto(Model):
    """Toy model with heavy neutral leptons, broken as in NLepto.cpp.

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
        _add_content(self)
        _break_model(self)
        _rotate_heavy_neutrals(self)
        _set_couplings(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:
    """Add the particles and the Lagrangian of the toy model."""
    model.add_scalar_boson(
        "H", gauge_rep={"L": 1, "Y": Fraction(-1, 2)}, self_conjugate=False
    )
    model.add_weyl_fermion(
        "L_L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep=FLAVOR,
    )
    model.add_weyl_fermion("E_R", chirality="R", gauge_rep={"Y": -1}, flavor_rep=FLAVOR)
    model.add_dirac_fermion("NR", flavor_rep=FLAVOR)
    model.add_dirac_fermion("SL", flavor_rep=FLAVOR)
    model.add_weyl_fermion("X_L", chirality="L")
    model.add_weyl_fermion("X_R", chirality="R")
    model.add_scalar_boson("ss", self_conjugate=False)

    H = model.particle("H")
    Li = model.particle("L_L")
    Ei = model.particle("E_R")
    Ni = model.particle("NR")
    Si = model.particle("SL")
    XL = model.particle("X_L")
    XR = model.particle("X_R")
    sig = model.particle("ss")

    flavor_space = model.vector_space(FLAVOR)
    I_f = flavor_space.index()
    J_f = flavor_space.index()
    i = model.generate_index("L", "L_L")
    j = model.generate_index("L", "L_L")
    eps = epsilon(i, j)
    a = model.generate_dirac_index()

    Ye = tensor(
        "Ye", [flavor_space, flavor_space], complex_property=ComplexProperty.Complex
    )
    YN = tensor(
        "YN", [flavor_space, flavor_space], complex_property=ComplexProperty.Complex
    )
    MNs = tensor("MNS", [flavor_space, flavor_space])
    MS = tensor("MS", [flavor_space, flavor_space])
    YXL = tensor("YXL", [flavor_space], complex_property=ComplexProperty.Complex)
    YXR = tensor("YXR", [flavor_space], complex_property=ComplexProperty.Complex)

    muH2 = constant("muH2")
    lh = constant("lH")
    mus2 = constant("muS2")
    ls = constant("lS")
    lsh = constant("lHS")
    MX = constant("mX")

    model.add_term(
        -Li(I_f, i, a).conjugate() * eps * Ye(I_f, J_f) * Ei(J_f, a) * H(j).conjugate(),
        add_hermitic=True,
    )
    model.add_term(
        -H(i) * Li(I_f, i, a).conjugate() * YN(I_f, J_f) * Ni(J_f, a),
        add_hermitic=True,
    )
    model.add_term(
        -Si(I_f, a).conjugate() * MNs(I_f, J_f) * Ni(J_f, a),
        add_hermitic=True,
    )
    model.add_term(
        -sig() * XL(a).conjugate() * YXL(I_f) * Ni(I_f, a),
        add_hermitic=True,
    )
    model.add_term(
        -sig().conjugate() * Si(I_f, a).conjugate() * YXR(I_f) * XR(a),
        add_hermitic=True,
    )
    model.add_term(-Si(I_f, a) * MS(I_f, J_f) * Si(J_f, a), add_hermitic=True)
    model.add_term(-XL(a).conjugate() * MX * XR(a), add_hermitic=True)

    model.add_term(-muH2 * H(i).conjugate() * H(i))
    prod = H(i).conjugate() * H(i)
    model.add_term(lh * prod * renamed_indices(prod))
    model.add_term(-mus2 * sig().conjugate() * sig())
    prod1 = sig().conjugate() * sig()
    model.add_term(ls * prod1 * prod1)
    prod2 = sig().conjugate() * sig() * H(i).conjugate() * H(i)
    model.add_term(lsh * prod2)


def _break_model(model: Model) -> None:
    """Break the flavour symmetry and rename the states as in NLepto.cpp."""
    model.break_flavor_symmetry(FLAVOR)
    model.rename_particle("L_L_1", "L_e")
    model.rename_particle("L_L_2", "L_m")
    model.rename_particle("L_L_3", "L_t")
    model.rename_particle("E_R_1", "e_R")
    model.rename_particle("E_R_2", "m_R")
    model.rename_particle("E_R_3", "t_R")
    model.rename_particle("NR_1", "N_1")
    model.rename_particle("NR_2", "N_2")
    model.rename_particle("NR_3", "N_3")
    model.rename_particle("SL_1", "N_4")
    model.rename_particle("SL_2", "N_5")
    model.rename_particle("SL_3", "N_6")


def _rotate_heavy_neutrals(model: Model) -> None:
    """Build the Majorana-like heavy neutrinos N1..N6 and diagonalise their masses."""
    al = model.generate_dirac_index()
    heavy = []
    for k in range(1, 7):
        particle = model.add_dirac_fermion(f"N{k}")
        particle.set_self_conjugate(True)
        heavy.append(particle)
    for k, particle in enumerate(heavy, start=1):
        field = model.particle(f"N_{k}")
        model.replace(field(al), 1 / sqrt(2) * particle(al))

    model.rotate_fields(heavy, diagonalize_masses=True)
    for k, particle in enumerate(heavy, start=1):
        particle.width = constant(f"GN{k}")
    model.refresh()


def _set_couplings(model: Model) -> None:
    """Express the gauge couplings through the electric charge and Weinberg angle."""
    thetaW = constant("thetaW")
    cW = cos(thetaW)
    sW = sin(thetaW)
    e = constant("e")
    model.replace(model.coupling("g_Y"), e / cW)
    model.replace(model.coupling("g_L"), e / sW)
