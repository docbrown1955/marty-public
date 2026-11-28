"""Custom U(1) model with e, mu and a Z boson, from tests/system/src/Zmumu.cpp.

Two Dirac leptons of charge -1 couple to a vector boson Z through left- and
right-handed couplings ``aL`` and ``aR``, which gives the e+e- -> mu+mu-
squared amplitude through Z exchange.
"""

from pymarty.group_theory import U
from pymarty.modeling import GaugeGroup, Model, dirac_gamma, dirac_P_L, dirac_P_R
from pymarty.modeling.models._symbols import constant

MU_NAME = "mu ; \\mu"


class Zmumu(Model):
    """U(1) toy model with the leptons e and mu and the vector boson Z.

    With ``finalize=False`` the model is left open for further additions.
    """

    def __init__(self, *, finalize: bool = True) -> None:
        super().__init__(gauge=[GaugeGroup(U(1), name="Y")])
        _add_content(self)
        if finalize:
            self.finalize()


def _add_content(model: Model) -> None:
    electron = model.add_dirac_fermion("e", gauge_rep={"Y": -1})
    muon = model.add_dirac_fermion(MU_NAME, gauge_rep={"Y": -1})
    boson = model.add_vector_boson(
        "Z",
        mass=constant("M_Z"),
        width=constant("G_Z"),
        self_conjugate=True,
    )

    aL = constant("aL")
    aR = constant("aR")
    a0 = model.generate_dirac_index()
    a1 = model.generate_dirac_index()
    a2 = model.generate_dirac_index()
    mu = model.generate_minkowski_index()

    for fermion in (electron, muon):
        model.add_term(
            aL
            * dirac_gamma(mu, a0, a1)
            * dirac_P_L(a1, a2)
            * fermion(a0).conjugate()
            * boson(~mu)
            * fermion(a2)
        )
        model.add_term(
            aR
            * dirac_gamma(mu, a0, a1)
            * dirac_P_R(a1, a2)
            * fermion(a0).conjugate()
            * boson(~mu)
            * fermion(a2)
        )
