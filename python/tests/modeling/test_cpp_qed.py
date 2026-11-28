import pytest

from pymarty.modeling.models.qed import QED

# Particle names of the C++ QED model, as printed by the core.
CPP_QED_PARTICLES = {"A", "c_A", "\\psi", "\\psi_L", "\\psi_R"}


@pytest.mark.unit
def test_qed_particles_match_cpp() -> None:
    assert {p.name for p in QED().particles} == CPP_QED_PARTICLES
