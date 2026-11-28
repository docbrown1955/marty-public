"""Toy model with a dark SU(2) group, from tests/system/src/simpleSpectrum.cpp.

The dark gauge group ``D`` (bosons ``VD``) is broken, which gives three dark
vectors. A scalar doublet ``phi``, a Dirac fermion ``Psi`` in the dark
flavour group, and a scalar doublet ``H`` of the SU(2) group ``L`` make up the
content. The dark Higgs mechanism is then simulated by replacing the broken
fields, and the Goldstone bosons are promoted before the spectrum is generated.
"""

from pymarty.group_theory import SU
from pymarty.modeling import FlavorGroup, GaugeGroup, Model, renamed_indices
from pymarty.modeling.models._symbols import constant
from pymarty.symbolic import I, sqrt

FLAVOR = "dark_flavor"


class SimpleSpectrum(Model):
    """Toy model of the spectrum test, with the replacements of the C++ program.

    The model is refreshed at the end, as the C++ program does before it
    generates the spectrum.
    """

    def __init__(self) -> None:
        super().__init__(
            gauge=[
                GaugeGroup(SU(2), name="D", boson_name="VD", coupling="g_D"),
                GaugeGroup(SU(2), name="L", boson_name="W"),
            ],
            flavor=[FlavorGroup(FLAVOR, 10)],
        )
        _add_content(self)
        self.refresh()


def _add_content(model: Model) -> None:
    H = model.add_scalar_boson("H", gauge_rep={"L": 1}, self_conjugate=False)
    Phi = model.add_scalar_boson("phi", gauge_rep={"D": 1}, self_conjugate=False)
    Psi = model.add_dirac_fermion("Psi", gauge_rep={"D": 1}, flavor_rep=FLAVOR)

    flavor_i = model.generate_index(FLAVOR)
    i = model.generate_index("L", "H")
    i_d = model.generate_index("D", "phi")
    dirac = model.generate_dirac_index()

    M_psi = constant("MPsi")
    mu_h2 = constant("mhH2")
    l_sh = constant("lSH")
    l_ss = constant("lSS")

    model.add_term(
        -M_psi * Psi(flavor_i, i_d, dirac).conjugate() * Psi(flavor_i, i_d, dirac),
        add_hermitic=True,
    )

    prod = Phi(i_d).conjugate() * Phi(i_d)
    model.add_term(l_ss * prod * renamed_indices(prod))
    model.add_term(mu_h2 * H(i).conjugate() * H(i))
    model.add_term(l_sh * Phi(i_d).conjugate() * Phi(i_d) * H(i).conjugate() * H(i))

    model.break_gauge_symmetry("D")

    model.rename_particle("Psi_1", "Psi_1 ; \\Psi_1")
    model.rename_particle("Psi_2", "Psi_2 ; \\Psi_2")
    model.rename_particle("phi_1", "ph0")
    model.rename_particle("phi_2", "php")

    VD_1 = model.particle("VD_1")
    VD_2 = model.particle("VD_2")
    F_VD_1 = VD_1.field_strength()
    F_VD_2 = VD_2.field_strength()
    VW = VD_1.generate_similar_particle("VW")
    VW.set_self_conjugate(False)

    model.rename_particle("VD_3", "VZ")

    mu = model.generate_minkowski_index()
    nu = model.generate_minkowski_index()

    ph0 = model.particle("ph0")
    phR = ph0.generate_similar_particle("phR")
    phR.set_self_conjugate(True)
    phI = ph0.generate_similar_particle("phI")
    phI.set_self_conjugate(True)

    vph = constant("vph")
    model.replace(ph0, (vph + phR() + I * phI()) / sqrt(2))

    model.replace(VD_1(mu), (VW(mu) + VW(mu).conjugate()) / sqrt(2))
    model.replace(
        F_VD_1(mu, nu),
        (VW(mu, nu) + VW(mu, nu).conjugate()) / sqrt(2),
    )
    model.replace(VD_2(mu), I * (VW(mu) - VW(mu).conjugate()) / sqrt(2))
    model.replace(F_VD_2(mu, nu), I * (VW(mu, nu) - VW(mu, nu).conjugate()) / sqrt(2))

    model.promote_to_goldstone("php", "VW")
    model.promote_to_goldstone("phI", "VZ")

    model.rename_particle("phR", "phR ; \\phi_R")
