import pytest
from cpp_models.qed import build

from pymarty import models


@pytest.mark.unit
def test_qed_particles_match_cpp() -> None:
    cpp = models.QED()
    model = build()
    assert sorted(p.name for p in model.particles) == sorted(
        p.name for p in cpp.particles
    )


@pytest.mark.unit
def test_qed_feynman_rules_match_cpp() -> None:
    cpp = models.QED()
    model = build()
    assert len(model.feynman_rules) == len(cpp.feynman_rules)
