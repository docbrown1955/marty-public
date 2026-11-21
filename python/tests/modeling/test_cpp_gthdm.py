"""Checks of the partial pymarty generic 2HDM port against the C++ GTHDM model."""

import pytest
from cpp_models.gthdm import build

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
CPP_GTHDM_FEYNMAN_RULES = 413


@pytest.mark.unit
def test_ported_particles_exist() -> None:
    py_names = {p.name for p in build().particles}
    assert {"G", "W", "B", "Nu_L", "E_L", "U_L", "D_L", "U_R", "D_R", "E_R"} <= py_names
    assert {"G0", "Gp", "h", "H0", "A0", "Hp"} <= py_names


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="BreakFlavorSymmetry, ghost bosons, Yukawa tensors and Z/A mixing "
    "are not bound",
)
def test_particle_names_match_cpp_gthdm() -> None:
    py_names = {p.name for p in build().particles}
    assert py_names == CPP_GTHDM_PARTICLES


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="Flavour splitting, ghost bosons, Yukawa and mass steps are not ported",
)
def test_feynman_rule_count_matches_cpp_gthdm() -> None:
    assert len(build().feynman_rules) == CPP_GTHDM_FEYNMAN_RULES
