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
    sB = model.weyl_fermion("sB", chirality="L")
    sW = model.weyl_fermion("sW", chirality="L")
    sW.set_rep("L", 2)
    sG = model.weyl_fermion("sG", chirality="L")
    sG.set_rep("C", (1, 1))
    for gaugino in (sB, sW, sG):
        gaugino.set_self_conjugate(True)
        model.add_particle(gaugino)


def _add_leptons(model: Model) -> None:
    L_L = model.weyl_fermion("L_L", chirality="L")
    L_L.set_rep("L", 1)
    L_L.set_rep("Y", Fraction(-1, 2))
    L_L.set_fundamental_flavor_rep("SM_flavor")

    E_R = model.weyl_fermion("E_R", chirality="R")
    E_R.set_rep("Y", -1)
    E_R.set_fundamental_flavor_rep("SM_flavor")

    sL_L = model.scalar_boson("sL_L")
    sL_L.set_rep("L", 1)
    sL_L.set_rep("Y", Fraction(-1, 2))
    sL_L.set_fundamental_flavor_rep("SM_flavor")

    sE_R = model.scalar_boson("sE_R")
    sE_R.set_rep("Y", -1)
    sE_R.set_fundamental_flavor_rep("SM_flavor")

    for particle in (L_L, E_R, sL_L, sE_R):
        model.add_particle(particle)


def _add_quarks(model: Model) -> None:
    Q_L = model.weyl_fermion("Q_L", chirality="L")
    Q_L.set_rep("C", (1, 0))
    Q_L.set_rep("L", 1)
    Q_L.set_rep("Y", Fraction(1, 6))
    Q_L.set_fundamental_flavor_rep("SM_flavor")

    U_R = model.weyl_fermion("U_R", chirality="R")
    U_R.set_rep("C", (1, 0))
    U_R.set_rep("Y", Fraction(2, 3))
    U_R.set_fundamental_flavor_rep("SM_flavor")

    D_R = model.weyl_fermion("D_R", chirality="R")
    D_R.set_rep("C", (1, 0))
    D_R.set_rep("Y", Fraction(-1, 3))
    D_R.set_fundamental_flavor_rep("SM_flavor")

    sQ_L = model.scalar_boson("sQ_L")
    sQ_L.set_rep("C", (1, 0))
    sQ_L.set_rep("L", 1)
    sQ_L.set_rep("Y", Fraction(1, 6))
    sQ_L.set_fundamental_flavor_rep("SM_flavor")

    sU_R = model.scalar_boson("sU_R")
    sU_R.set_rep("C", (1, 0))
    sU_R.set_rep("Y", Fraction(2, 3))
    sU_R.set_fundamental_flavor_rep("SM_flavor")

    sD_R = model.scalar_boson("sD_R")
    sD_R.set_rep("C", (1, 0))
    sD_R.set_rep("Y", Fraction(-1, 3))
    sD_R.set_fundamental_flavor_rep("SM_flavor")

    for particle in (Q_L, U_R, D_R, sQ_L, sU_R, sD_R):
        model.add_particle(particle)


def _add_higgs(model: Model) -> None:
    Hu = model.scalar_boson("Hu")
    Hu.set_rep("L", 1)
    Hu.set_rep("Y", Fraction(1, 2))

    Hd = model.scalar_boson("Hd")
    Hd.set_rep("L", 1)
    Hd.set_rep("Y", Fraction(-1, 2))

    sHu = model.weyl_fermion("sHu", chirality="L")
    sHu.set_rep("L", 1)
    sHu.set_rep("Y", Fraction(1, 2))

    sHd = model.weyl_fermion("sHd", chirality="L")
    sHd.set_rep("L", 1)
    sHd.set_rep("Y", Fraction(-1, 2))

    for particle in (Hu, Hd, sHu, sHd):
        model.add_particle(particle)
    model.add_bosonic_mass(Hu, model.new_coupling("m_sHu"))
    model.add_bosonic_mass(Hd, model.new_coupling("m_sHd"))
