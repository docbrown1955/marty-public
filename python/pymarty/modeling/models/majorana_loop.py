"""Toy U(1) model with a Majorana-like fermion loop (majoranaLoop.cpp).

A charged complex scalar pair (S, P) couples to a Dirac fermion psi and to a
self-conjugate Dirac fermion lambda, which also couples to the photon. It is
used to check the simplification of fermion loops with a self-conjugate
fermion.
"""

from pymarty.group_theory import U
from pymarty.modeling import GaugeGroup, Model, dirac_gamma, dirac_gamma5
from pymarty.symbolic import Expr, constant, delta

CHARGE = 1


class MajoranaLoop(Model):
    """U(1) toy model with a self-conjugate fermion (src/tests/majoranaLoop.cpp).

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(gauge=[GaugeGroup(U(1), name="\\gamma", boson_name="\\gamma")])
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:
    mass = constant("m")
    model.add_dirac_fermion("\\psi", gauge_rep={"\\gamma": CHARGE}, mass=mass)
    model.add_dirac_fermion("\\lambda", self_conjugate=True)
    model.add_scalar_boson(
        "S", gauge_rep={"\\gamma": CHARGE}, mass=mass, self_conjugate=False
    )
    model.add_scalar_boson(
        "P", gauge_rep={"\\gamma": CHARGE}, mass=mass, self_conjugate=False
    )

    S = model.particle("S")
    P = model.particle("P")
    psi = model.particle("\\psi")
    lam = model.particle("\\lambda")
    photon = model.particle("\\gamma")
    a1 = model.generate_dirac_index()
    a2 = model.generate_dirac_index()
    mu = model.generate_minkowski_index()

    scalar_term = (
        Expr(1)
        / 2
        * CHARGE
        * CHARGE
        * (S().conjugate() * P() - P().conjugate() * S()) ** 2
    )
    model.add_term(scalar_term)

    yukawa = (
        1j
        * CHARGE
        * psi(a1).conjugate()
        * (S() * delta(a1, a2) + 1j * dirac_gamma5(a1, a2) * P())
        * lam(a2)
    )
    model.add_term(yukawa, add_hermitic=True)

    gauge_term = (
        Expr(1)
        / 2
        * lam(a1).conjugate()
        * photon(~mu)
        * dirac_gamma(mu, a1, a2)
        * lam(a2)
    )
    model.add_term(gauge_term)
