"""Checks of the partial pymarty Standard Model port against the C++ SM model."""

import pytest
from cpp_models.sm import build

from pymarty import models

CPP_SM_FEYNMAN_RULES = 190


@pytest.mark.unit
def test_ported_particles_exist() -> None:
    py_names = {p.name for p in build().particles}
    assert {
        "G",
        "W_1",
        "W_2",
        "W_3",
        "B",
        "H_1",
        "H_2",
        "u_L",
        "c_L",
        "t_L",
        "d_L",
        "s_L",
        "b_L",
    } <= py_names


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="Electroweak breaking (Higgs VEV, W/Z/A and Goldstones) is not ported",
)
def test_particle_names_match_cpp_sm() -> None:
    cpp_names = {p.name for p in models.SM().particles}
    py_names = {p.name for p in build().particles}
    assert py_names == cpp_names


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="Ghost-boson and gauge-fixing steps are not ported",
)
def test_feynman_rule_count_matches_cpp_sm() -> None:
    assert len(build().feynman_rules) == CPP_SM_FEYNMAN_RULES
    assert len(models.SM().feynman_rules) == CPP_SM_FEYNMAN_RULES
