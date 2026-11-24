"""Toy model without gauge group, from tests/system/src/testConjugation.cpp.

Fermions psi and lambda (self-conjugate), a scalar phi and a vector A, with
Yukawa and vector couplings that mix psi and lambda. It is used to check the
relative signs of diagrams with fermion-number violation (Denner, eq. 3.4a,
3.5a and 3.6a).
"""

from pymarty.modeling import Model, dirac_gamma
from pymarty.symbolic import Expr, constant


class Conjugation(Model):
    """Gauge-less toy model with fermion-number violation (testConjugation.cpp).

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(gauge=[])
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:
    model.add_dirac_fermion("\\psi")
    model.add_dirac_fermion("\\lambda", self_conjugate=True)
    model.add_scalar_boson("\\phi")
    model.add_vector_boson("A")

    psi = model.particle("\\psi")
    lam = model.particle("\\lambda")
    phi = model.particle("\\phi")
    A = model.particle("A")
    a = model.generate_dirac_index()
    b = model.generate_dirac_index()
    mu = model.generate_minkowski_index()
    g = constant("g")

    model.add_term(constant("a") * phi() * psi(a).conjugate() * psi(a))
    model.add_term(g * phi() * psi(a).conjugate() * lam(a), add_hermitic=True)
    model.add_term(Expr(1) / 2 * constant("c") * phi() * lam(a).conjugate() * lam(a))

    model.add_term(
        constant("a") * A(~mu) * psi(a).conjugate() * dirac_gamma(mu, a, b) * psi(b)
    )
    model.add_term(
        g * A(~mu) * psi(a).conjugate() * dirac_gamma(mu, a, b) * lam(b),
        add_hermitic=True,
    )
    model.add_term(
        Expr(1)
        / 2
        * constant("c")
        * A(~mu)
        * lam(a).conjugate()
        * dirac_gamma(mu, a, b)
        * lam(b)
    )
