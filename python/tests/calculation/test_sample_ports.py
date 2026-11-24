"""Tree-level ports of the C++ sample programs, for ``pymarty.calculation``.

Each test mirrors one sample: the model is built with the Python modeling
API and the diagram content is checked against the C++ run.
"""

from pymarty.calculation import (
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.group_theory import SU, U
from pymarty.modeling import (
    FlavorGroup,
    GaugeGroup,
    Model,
    dirac_gamma,
    dirac_P_L,
    dirac_P_R,
)
from pymarty.modeling.models.sm import SM
from pymarty.symbolic import constant


def _zmumu() -> Model:
    # Port of tests/system/src/Zmumu.cpp: a Z boson coupling e and mu through
    # P_L / P_R, next to the U(1) photon-like boson of the gauge group Y.
    model = Model(gauge=[GaugeGroup(U(1), name="Y")])
    electron = model.add_dirac_fermion("e", gauge_rep={"Y": -1})
    muon = model.add_dirac_fermion("mu ; \\mu", gauge_rep={"Y": -1})
    z = model.add_vector_boson(
        "Z", mass=constant("M_Z"), width=constant("G_Z"), self_conjugate=True
    )
    a0, a1, a2 = (model.generate_dirac_index() for _ in range(3))
    m = model.generate_minkowski_index()
    aL, aR = constant("aL"), constant("aR")
    # The Z index is upper in the C++ Lagrangian (Z(+m)); here it is ~m.
    for fermion in (electron, muon):
        model.add_term(
            aL
            * dirac_gamma(m, a0, a1)
            * dirac_P_L(a1, a2)
            * fermion(a0).conjugate()
            * z(~m)
            * fermion(a2)
        )
        model.add_term(
            aR
            * dirac_gamma(m, a0, a1)
            * dirac_P_R(a1, a2)
            * fermion(a0).conjugate()
            * z(~m)
            * fermion(a2)
        )
    model.finalize()
    return model


def test_zmumu_scattering_exchanges_the_z_and_the_gauge_boson() -> None:
    model = _zmumu()
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("e"), incoming(anti("e")), outgoing("mu"), outgoing(anti("mu"))],
    )
    particles = sorted(tuple(d.particles) for d in amplitude.diagrams)
    assert particles == [("e", "mu", "A_Y"), ("e", "mu", "Z")]
    assert compute_squared_amplitude(model, amplitude) is not None


def _qcd_with_sextet() -> Model:
    # Port of examples/sampleQCDSextet.cpp: colour triplets u, d and a sextet X.
    model = Model(gauge=[GaugeGroup(SU(3), name="C", boson_name="G", coupling="g")])
    for name in ("u", "d"):
        model.add_dirac_fermion(
            name, gauge_rep={"C": [1, 0]}, mass=constant(f"m_{name}")
        )
    model.add_dirac_fermion("X", gauge_rep={"C": [2, 0]}, mass=constant("m_X"))
    model.finalize()
    return model


def test_qcd_sextet_scattering_has_a_squared_amplitude() -> None:
    model = _qcd_with_sextet()
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("X"), incoming(anti("X")), outgoing("u"), outgoing(anti("u"))],
    )
    assert len(amplitude.diagrams) == 1
    assert compute_squared_amplitude(model, amplitude) is not None


def test_qcd_triplet_scattering_keeps_the_gluon_diagram() -> None:
    model = _qcd_with_sextet()
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("u"), incoming(anti("u")), outgoing("d"), outgoing(anti("d"))],
    )
    assert len(amplitude.diagrams) == 1
    assert "G" in amplitude.diagrams[0].particles


def test_gg_to_tt_tree_has_three_diagrams_and_ghost_channels_are_separate() -> None:
    # Port of tests/system/src/gg_to_tt.cpp (tree level, ghosts included).
    model = SM()
    gg = compute_amplitude(
        model,
        Order.TREE,
        [incoming("G"), incoming("G"), outgoing("t"), outgoing(anti("t"))],
    )
    assert len(gg.diagrams) == 3
    assert compute_squared_amplitude(model, gg) is not None

    ghost = compute_amplitude(
        model,
        Order.TREE,
        [incoming("c_G"), incoming(anti("c_G")), outgoing("t"), outgoing(anti("t"))],
    )
    assert len(ghost.diagrams) == 1


def test_nf_flavour_qcd_quark_scattering_at_tree_level() -> None:
    # Quark of colour triplet with an N_f = 2 flavour index.
    model = Model(
        gauge=[GaugeGroup(SU(3), name="C", boson_name="G", coupling="g")],
        flavor=[FlavorGroup("F", 2)],
    )
    model.add_dirac_fermion(
        "q", gauge_rep={"C": [1, 0]}, flavor_rep="F", mass=constant("m_q")
    )
    model.finalize()
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("q"), incoming(anti("q")), outgoing("q"), outgoing(anti("q"))],
    )
    assert len(amplitude.diagrams) == 2
    assert compute_squared_amplitude(model, amplitude) is not None
