"""Checks of the partial pymarty Standard Model port against the C++ SM model."""

import pytest
from cpp_models.sm import build

from pymarty import models

CPP_SM_FEYNMAN_RULES = 190


@pytest.mark.unit
def test_particle_names_match_cpp_sm() -> None:
    cpp_names = {p.name for p in models.SM().particles}
    py_names = {p.name for p in build().particles}
    assert py_names == cpp_names


@pytest.mark.unit
@pytest.mark.xfail(
    strict=True,
    reason="W/Z mass setting and gauge-fixing terms (C++ adjust) are not ported: "
    "176 rules instead of 190",
)
def test_feynman_rule_count_matches_cpp_sm() -> None:
    assert len(build().feynman_rules) == CPP_SM_FEYNMAN_RULES
    assert len(models.SM().feynman_rules) == CPP_SM_FEYNMAN_RULES
