"""Python ports of the model parts of the C++ examples.

``gettingStarted``, ``sampleSMFeynmanRulesLatex`` and the model part of
``simpleSpectrum`` are ported here. The expected values come from running the
C++ examples in the same build: the 12 particles of the toy model before the
symmetry breaking, the 13 particles after the full toy model, 6 diagrams for
the amplitude, and the 190 Feynman rules of the Standard Model. The C++
examples also display, draw and build libraries; those steps are not ported.
"""

from fractions import Fraction

from pymarty import models
from pymarty.group_theory import SU, U
from pymarty.modeling import (
    FlavorGroup,
    GaugeGroup,
    Leg,
    Model,
    Order,
    dirac_index,
    renamed_indices,
)
from pymarty.symbolic import Expr, I, Minkowski, constant, cos, sin, sqrt

# Particle names printed by examples/gettingStarted.cpp (first half)
CPP_TOY_PARTICLES = {
    "B",
    "c_B",
    "u_R",
    "d_R",
    "W_1",
    "W_2",
    "W_3",
    "c_W_1",
    "c_W_2",
    "c_W_3",
    "u_L",
    "d_L",
}

# Particle names after the full gettingStarted model (13 particles)
CPP_GETTING_STARTED_PARTICLES = {
    "A",
    "W",
    "Z",
    "c_B",
    "c_W_1",
    "c_W_2",
    "c_W_3",
    "d",
    "d_L",
    "d_R",
    "u",
    "u_L",
    "u_R",
}

# Number of diagrams of the u ubar -> d dbar tree-level amplitude
CPP_GETTING_STARTED_DIAGRAMS = 6

# Number of Feynman rules printed by examples/sampleSMFeynmanRulesLatex.cpp
CPP_SM_FEYNMAN_RULES = 190

# Particle names after the model part of simpleSpectrum.cpp (before generation)
CPP_SIMPLE_SPECTRUM_PARTICLES = {
    "H",
    "Psi_1",
    "Psi_1_L",
    "Psi_1_R",
    "Psi_2",
    "Psi_2_L",
    "Psi_2_R",
    "VW",
    "VZ",
    "W",
    "c_VD_1",
    "c_VD_2",
    "c_VZ",
    "c_W",
    "phI",
    "phR",
    "php",
}


def make_toy_model() -> Model:
    """Build the toy model of gettingStarted.cpp up to the broken particles."""
    toy = Model(
        gauge=[
            GaugeGroup(SU(2), name="L", boson_name="W"),
            GaugeGroup(U(1), name="Y", boson_name="B"),
        ]
    )
    Q_L = toy.weyl_fermion("Q_L", chirality="L")
    u_R = toy.weyl_fermion("u_R", chirality="R")
    d_R = toy.weyl_fermion("d_R", chirality="R")
    Q_L.set_rep("L", 1)
    Q_L.set_rep("Y", Fraction(1, 6))
    u_R.set_rep("Y", Fraction(2, 3))
    d_R.set_rep("Y", Fraction(-1, 3))
    toy.add_particle(Q_L)
    toy.add_particle(u_R)
    toy.add_particle(d_R)
    toy.break_gauge_symmetry("Y")
    toy.break_gauge_symmetry("L", renaming={"Q_L": ["u_L", "d_L"]})
    return toy


def test_getting_started_first_half_particles_match_cpp() -> None:
    toy = make_toy_model()
    names = [p.name for p in toy.particles]
    assert len(names) == len(CPP_TOY_PARTICLES)
    assert set(names) == CPP_TOY_PARTICLES


def test_getting_started_renaming_is_explicit_and_partial() -> None:
    toy = make_toy_model()
    assert "Q_L" not in toy
    assert "u_L" in toy and "d_L" in toy
    # Partial renaming: the gauge bosons of the unrenamed group keep defaults
    assert {"W_1", "W_2", "W_3"} <= {p.name for p in toy.particles}


def make_unbroken_model() -> Model:
    """Toy model before any symmetry breaking, so gauge irreps are meaningful."""
    toy = Model(
        gauge=[
            GaugeGroup(SU(2), name="L", boson_name="W"),
            GaugeGroup(U(1), name="Y", boson_name="B"),
        ]
    )
    Q_L = toy.weyl_fermion("Q_L", chirality="L")
    Q_L.set_rep("L", 1)
    Q_L.set_rep("Y", Fraction(1, 6))
    toy.add_particle(Q_L)
    return toy


def test_spin_follows_the_lorentz_representation() -> None:
    assert make_toy_model().particle("W_1").spin == 1
    toy = make_unbroken_model()
    assert toy.particle("Q_L").spin == Fraction(1, 2)
    assert toy.scalar_boson("H").spin == 0


def test_irreps_are_read_per_gauge_group() -> None:
    toy = make_unbroken_model()
    Q_L = toy.particle("Q_L")
    assert Q_L.irrep("L").highest_weight == (1,)
    assert Q_L.irrep("L").dim == 2
    assert Q_L.irrep("Y").charge == Fraction(1, 6)


def test_irreps_of_broken_groups_are_trivial() -> None:
    toy = make_toy_model()
    u_L = toy.particle("u_L")
    assert u_L.irrep("L").dim == 1
    assert u_L.irrep("L").highest_weight == (0,)


def test_built_in_models_expose_group_irreps() -> None:
    e_L = models.SM().particle("e_L")
    assert e_L.irrep("L").dim == 1
    assert e_L.irrep("Y").charge == Fraction(-1, 2)


def test_model_irreps_compare_with_group_irreps() -> None:
    toy = make_unbroken_model()
    assert toy.particle("Q_L").irrep("L") == SU(2).fundamental()
    assert toy.particle("Q_L").irrep("L") != SU(2).adjoint()


def test_str_model_lists_particles_with_spin_irrep_and_lagrangian() -> None:
    toy = make_toy_model()
    text = str(toy)
    count = len(toy.particles)
    assert text.startswith("Gauge = ")
    assert f"Particle list, {count} particles:" in text
    assert text.count("\n  -> ") == count
    assert text.count("\n    spin: ") == count
    assert text.count("\n    irrep: ") == count
    assert "lagrangian" in text


def test_sm_latex_document_has_one_entry_per_cpp_feynman_rule() -> None:
    rules = models.SM().feynman_rules
    assert len(rules) == CPP_SM_FEYNMAN_RULES
    document = rules.latex_document()
    headers = [
        line for line in document.splitlines() if line.startswith("Feynman vertex for ")
    ]
    assert len(headers) == CPP_SM_FEYNMAN_RULES


def make_getting_started_model() -> Model:
    """Build the full toy model of gettingStarted.cpp, ready for amplitudes."""
    toy = make_toy_model()
    W_1 = toy.particle("W_1")
    W_2 = toy.particle("W_2")
    F_W_1 = W_1.field_strength()
    F_W_2 = W_2.field_strength()
    W = W_1.similar("W")
    W.set_self_conjugate(False)
    mu = Minkowski.index()
    nu = Minkowski.index()
    toy.replace(W_1(mu), (W(mu) + W(mu).conjugate()) / sqrt(2))
    toy.replace(F_W_1(mu, nu), (W(mu, nu) + W(mu, nu).conjugate()) / sqrt(2))
    toy.replace(W_2(mu), I * (W(mu) - W(mu).conjugate()) / sqrt(2))
    toy.replace(F_W_2(mu, nu), I * (W(mu, nu) - W(mu, nu).conjugate()) / sqrt(2))

    theta_w = constant("thetaW")
    c_w = cos(theta_w)
    s_w = sin(theta_w)
    toy.rotate_fields(
        ["W_3", "B"],
        ["Z", "A"],
        [[c_w, s_w], [-s_w, c_w]],
    )
    e = constant("e")
    toy.replace(toy.coupling("g_Y"), e / c_w)
    toy.replace(toy.coupling("g_L"), e / s_w)

    toy.add_bosonic_mass(toy.particle("W"), constant("M_W"))
    toy.add_fermionic_mass(toy.particle("u_L"), toy.particle("u_R"), constant("m_u"))
    toy.add_fermionic_mass(toy.particle("d_L"), toy.particle("d_R"), constant("m_d"))
    toy.refresh()
    return toy


def test_getting_started_model_particles_match_cpp() -> None:
    toy = make_getting_started_model()
    names = {p.name for p in toy.particles}
    assert names == CPP_GETTING_STARTED_PARTICLES


def test_getting_started_tree_amplitude_matches_cpp_diagram_count() -> None:
    toy = make_getting_started_model()
    u = toy.particle("u")
    d = toy.particle("d")
    amplitude = toy.compute_amplitude(
        Order.TreeLevel,
        [Leg(u, True), Leg(u, True, True), Leg(d, False), Leg(d, False, True)],
    )
    assert amplitude.diagram_count == CPP_GETTING_STARTED_DIAGRAMS
    squared = toy.compute_squared_amplitude(amplitude)
    assert isinstance(squared, Expr)
    assert squared != Expr(0)


def make_simple_spectrum_model() -> Model:
    """Build the model part of simpleSpectrum.cpp, before the spectrum step."""
    toy = Model(
        gauge=[
            GaugeGroup(SU(2), name="D", boson_name="VD"),
            GaugeGroup(SU(2), name="L", boson_name="W"),
        ],
        flavor=[FlavorGroup("dark_flavor", 10)],
    )
    spinor = dirac_index()
    H = toy.scalar_boson("H")
    H.set_rep("L", 1)
    toy.add_particle(H)
    phi = toy.scalar_boson("phi")
    phi.set_rep("D", 1)
    toy.add_particle(phi)
    Psi = toy.dirac_fermion("Psi")
    Psi.set_rep("D", 1)
    Psi.set_fundamental_flavor_rep("dark_flavor")
    toy.add_particle(Psi)

    flavor_i = toy.generate_index("dark_flavor")
    i = toy.generate_index("L", "H")
    i_d = toy.generate_index("D", "phi")

    def cc(expr: Expr) -> Expr:
        return expr.conjugate()

    toy.add_term(
        -constant("MPsi") * cc(Psi(flavor_i, i_d, spinor)) * Psi(flavor_i, i_d, spinor),
        True,
    )
    prod = cc(phi(i_d)) * phi(i_d)
    toy.add_term(constant("lSS") * prod * renamed_indices(prod))
    toy.add_term(constant("mhH2") * cc(H(i)) * H(i))
    toy.add_term(constant("lSH") * cc(phi(i_d)) * phi(i_d) * cc(H(i)) * H(i))

    toy.break_gauge_symmetry(
        "D",
        renaming={
            "Psi": ["Psi_1 ; \\Psi_1", "Psi_2 ; \\Psi_2"],
            "phi": ["ph0", "php"],
            "VD": ["VD_1", "VD_2", "VZ"],
        },
    )
    VD_1 = toy.particle("VD_1")
    VD_2 = toy.particle("VD_2")
    F_VD_1 = VD_1.field_strength()
    F_VD_2 = VD_2.field_strength()
    VW = VD_1.similar("VW")
    VW.set_self_conjugate(False)
    mu = Minkowski.index()
    nu = Minkowski.index()
    ph0 = toy.particle("ph0")
    phR = ph0.similar("phR")
    phR.set_self_conjugate(True)
    phI = ph0.similar("phI")
    phI.set_self_conjugate(True)
    toy.replace(
        ph0,
        (constant("vph") + phR.expr() + I * phI.expr()) / sqrt(2),
    )
    toy.replace(VD_1(mu), (VW(mu) + cc(VW(mu))) / sqrt(2))
    toy.replace(F_VD_1(mu, nu), (VW(mu, nu) + cc(VW(mu, nu))) / sqrt(2))
    toy.replace(VD_2(mu), I * (VW(mu) - cc(VW(mu))) / sqrt(2))
    toy.replace(F_VD_2(mu, nu), I * (VW(mu, nu) - cc(VW(mu, nu))) / sqrt(2))
    toy.promote_to_goldstone("php", "VW")
    toy.promote_to_goldstone("phI", "VZ")
    toy.refresh()
    return toy


def test_simple_spectrum_model_particles_match_cpp() -> None:
    toy = make_simple_spectrum_model()
    assert {p.name for p in toy.particles} == CPP_SIMPLE_SPECTRUM_PARTICLES
