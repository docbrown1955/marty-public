"""Checks of the partial MSSM port against the C++ MSSM_HEM model.

The C++ reference was obtained by building ``MSSM_HEM`` from
src/marty/models/mssm.cpp in the build directory and printing its particles
and Feynman rules. It has 29 particles and 148 Feynman rules. The Python port
has the same 29 particles, but fewer Feynman rules because the soft masses,
gaugino Yukawas and D-terms are blocked (see cpp_models/mssm.py).
"""

import pytest
from cpp_models.mssm import build

# Particle names printed by the C++ MSSM_HEM model
CPP_MSSM_HEM_PARTICLES = {
    "G",
    "W",
    "B",
    "c_G",
    "c_W",
    "c_B",
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
    "sB",
    "sB_L",
    "sB_R",
    "sW",
    "sW_L",
    "sW_R",
    "sG",
    "sG_L",
    "sG_R",
}


@pytest.mark.unit
def test_mssm_particles_match_cpp() -> None:
    model = build()
    names = [particle.name for particle in model.particles]
    assert len(names) == len(set(names))
    assert set(names) == CPP_MSSM_HEM_PARTICLES
