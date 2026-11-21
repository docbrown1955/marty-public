"""Python ports of the model parts of the C++ examples.

``gettingStarted`` (first half) and ``sampleSMFeynmanRulesLatex`` are ported
here. The expected values come from running the C++ examples in the same
build: the 12 particles of the toy model and the 190 Feynman rules of the
Standard Model. ``simpleSpectrum`` is not ported: it needs flavour
representations, generated indices and Dirac indices, which the modeling
facade does not expose yet.
"""

from fractions import Fraction

from pymarty import models
from pymarty.group_theory import SU, U
from pymarty.modeling import GaugeGroup, Model

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

# Number of Feynman rules printed by examples/sampleSMFeynmanRulesLatex.cpp
CPP_SM_FEYNMAN_RULES = 190


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


def test_sm_latex_document_has_one_entry_per_cpp_feynman_rule() -> None:
    rules = models.SM().feynman_rules
    assert len(rules) == CPP_SM_FEYNMAN_RULES
    document = rules.latex_document()
    headers = [
        line for line in document.splitlines() if line.startswith("Feynman vertex for ")
    ]
    assert len(headers) == CPP_SM_FEYNMAN_RULES
