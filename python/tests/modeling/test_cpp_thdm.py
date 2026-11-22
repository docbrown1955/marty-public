"""Checks of the partial pymarty two Higgs doublet model port against C++ THDM."""

import pytest

from pymarty.modeling.models.thdm import THDM

CPP_THDM_PARTICLES = {
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
def test_ported_bosons_match_cpp_names() -> None:
    py_names = {p.name for p in THDM().particles}
    assert {"G", "c_G", "W", "A0", "Hp", "h", "H0", "A", "Z", "Gp", "G0"} <= py_names


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="flavour splitting, ghost replacements c_A/c_Z/c_Wm/c_Wp and "
    "replaceYukawas are not ported",
)
def test_particle_names_match_cpp_thdm() -> None:
    py_names = {p.name for p in THDM().particles}
    assert py_names == CPP_THDM_PARTICLES
