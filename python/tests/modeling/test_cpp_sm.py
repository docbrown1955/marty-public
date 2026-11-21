"""Checks of the partial pymarty Standard Model port against the C++ SM model."""

import pytest
from cpp_models.sm import build

# Particle names of the C++ SM model (src/marty/models/sm.cpp), as printed by the core.
CPP_SM_PARTICLES = {
    "A", "G", "G0", "Gp", "W", "Z", "b", "b_L", "b_R", "c", "c_A", "c_G", "c_L",
    "c_R", "c_Wm", "c_Wp", "c_Z", "d", "d_L", "d_R", "e", "e_L", "e_R", "h", "mu",
    "mu_L", "mu_R", "nu_e", "nu_mu", "nu_tau", "s", "s_L", "s_R", "t", "t_L",
    "t_R", "tau", "tau_L", "tau_R", "u", "u_L", "u_R",
}  # fmt: skip


@pytest.mark.unit
def test_particle_names_match_cpp_sm() -> None:
    assert {p.name for p in build().particles} == CPP_SM_PARTICLES
