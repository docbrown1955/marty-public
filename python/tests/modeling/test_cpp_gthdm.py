"""Checks of the partial pymarty generic 2HDM port against the C++ GTHDM model."""

import pytest

from pymarty.modeling.models.gthdm import GTHDM

CPP_GTHDM_PARTICLES = {
    "G",
    "c_G",
    "W",
    "c_Wm",
    "c_Wp",
    "A0",
    "Hp",
    "h",
    "H0",
    "A",
    "Z",
    "c_A",
    "c_Z",
    "u_R",
    "c_R",
    "t_R",
    "u_L",
    "c_L",
    "t_L",
    "d_L",
    "s_L",
    "b_L",
    "d_R",
    "s_R",
    "b_R",
    "e_R",
    "mu_R",
    "tau_R",
    "e_L",
    "mu_L",
    "tau_L",
    "nu_e",
    "nu_mu",
    "nu_tau",
    "Gp",
    "G0",
    "u",
    "c",
    "t",
    "d",
    "s",
    "b",
    "e",
    "mu",
    "tau",
}


@pytest.mark.unit
def test_ported_particles_exist() -> None:
    py_names = {p.name for p in GTHDM().particles}
    assert {"G", "W", "A", "Z", "G0", "Gp", "h", "H0", "A0", "Hp"} <= py_names


@pytest.mark.unit
def test_particle_names_match_cpp_gthdm() -> None:
    py_names = {p.name for p in GTHDM().particles}
    assert py_names == CPP_GTHDM_PARTICLES
