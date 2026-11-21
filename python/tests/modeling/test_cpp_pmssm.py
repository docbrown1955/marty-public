from fractions import Fraction

import pytest
from cpp_models.pmssm import build

# Particles of PMSSM_Model after initContent(), from a scratch C++ probe of
# src/marty/models/pmssm.h (subclass calling the protected initContent())
CPP_CONTENT_PARTICLES = {
    "G",
    "W",
    "B",
    "c_G",
    "c_W",
    "c_B",
    "sB",
    "sW",
    "sG",
    "L_L",
    "E_R",
    "sL_L",
    "sE_R",
    "Q_L",
    "U_R",
    "D_R",
    "sQ_L",
    "sU_R",
    "sD_R",
    "Hu",
    "Hd",
    "sHu",
    "sHd",
}

# Complete C++ PMSSM_Model (examples/samplePMSSM.cpp): 87 particles
CPP_FULL_PARTICLE_COUNT = 87


@pytest.mark.unit
def test_pmssm_content_particles_match_cpp() -> None:
    names = {p.name for p in build().particles}
    assert names == CPP_CONTENT_PARTICLES


@pytest.mark.unit
def test_pmssm_representations_match_cpp() -> None:
    model = build()
    assert model.particle("sG").irrep("C").dim == 8
    assert model.particle("sW").irrep("L").dim == 3
    assert model.particle("Q_L").irrep("C").dim == 3
    assert model.particle("Q_L").irrep("Y").charge == Fraction(1, 6)
    assert model.particle("Hd").irrep("Y").charge == Fraction(-1, 2)


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="interactions and the low-energy breaking chain of pmssm.cpp are not "
    "portable with the declarative API",
)
def test_pmssm_full_particle_count_matches_cpp() -> None:
    assert len(build().particles) == CPP_FULL_PARTICLE_COUNT
