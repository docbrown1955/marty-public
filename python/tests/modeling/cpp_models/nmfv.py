"""Partial pymarty port of the field content of ``src/marty/models/nmfv.cpp``.

NMFV derives from PMSSM and MSSM. This module ports the field content produced
by ``MSSM_Model::initContent`` (gauge groups, gauginos, leptons, squarks, Higgs
doublets and higgsinos) with the three-generation flavour group ``SM_flavor``,
and the Higgs boson masses set in ``MSSM_Model::initHiggs``.

Not ported (interactions and mixing are outside this field-content port):
  - the gaugino masses ``M_1``, ``M_2``, ``M_3`` and the higgsino ``mu`` term
    from ``initInteractions`` (the ``mu`` term is expressible with ``epsilon``
    and indexed ``add_term``, but it is not part of this port);
  - the sfermion soft-mass tensors ``M2_s_L``, ``M2_s_E``, ``M2_s_Q``, ``M2_s_U``,
    ``M2_s_D`` (complex Hermitian flavour tensors with Lagrangian terms);
  - every interaction, the symmetry breaking, the diagonalizations, the NMFV
    mixings, and the Feynman rules of the low-energy model.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model


def build() -> Model:
    """Build the pre-breaking MSSM field content used by NMFV."""
    model = Model(
        gauge=[
            GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g"),
            GaugeGroup(U(1), name="Y", boson_name="B", coupling="g'"),
        ],
        flavor=[FlavorGroup("SM_flavor", 3)],
    )
    _add_gauginos(model)
    _add_leptons(model)
    _add_quarks(model)
    _add_higgs(model)
    return model


def _add_gauginos(model: Model) -> None:
    model.add_weyl_fermion("sB", chirality="L", self_conjugate=True)
    model.add_weyl_fermion("sW", chirality="L", gauge_rep={"L": 2}, self_conjugate=True)
    model.add_weyl_fermion(
        "sG", chirality="L", gauge_rep={"C": (1, 1)}, self_conjugate=True
    )


def _add_leptons(model: Model) -> None:
    model.add_weyl_fermion(
        "L_L",
        chirality="L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep="SM_flavor",
    )
    model.add_weyl_fermion(
        "E_R", chirality="R", gauge_rep={"Y": -1}, flavor_rep="SM_flavor"
    )
    model.add_scalar_boson(
        "sL_L",
        gauge_rep={"L": 1, "Y": Fraction(-1, 2)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "sE_R", gauge_rep={"Y": -1}, flavor_rep="SM_flavor", self_conjugate=False
    )


def _add_quarks(model: Model) -> None:
    model.add_weyl_fermion(
        "Q_L",
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
    model.add_scalar_boson(
        "sQ_L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "sU_R",
        gauge_rep={"C": (1, 0), "Y": Fraction(2, 3)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )
    model.add_scalar_boson(
        "sD_R",
        gauge_rep={"C": (1, 0), "Y": Fraction(-1, 3)},
        flavor_rep="SM_flavor",
        self_conjugate=False,
    )


def _add_higgs(model: Model) -> None:
    Hu = model.add_scalar_boson(
        "Hu", gauge_rep={"L": 1, "Y": Fraction(1, 2)}, self_conjugate=False
    )
    Hd = model.add_scalar_boson(
        "Hd", gauge_rep={"L": 1, "Y": Fraction(-1, 2)}, self_conjugate=False
    )
    model.add_weyl_fermion(
        "sHu", chirality="L", gauge_rep={"L": 1, "Y": Fraction(1, 2)}
    )
    model.add_weyl_fermion(
        "sHd", chirality="L", gauge_rep={"L": 1, "Y": Fraction(-1, 2)}
    )
    model.add_bosonic_mass(Hu, model.add_coupling("m_sHu"))
    model.add_bosonic_mass(Hd, model.add_coupling("m_sHd"))
