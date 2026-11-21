import pytest
from cpp_models.qcd import build

# Reference output of the C++ QCD_Model (qcd.cpp), obtained from a scratch program.
CPP_PARTICLES = ["G", "c_G", "u", "u_L", "u_R", "d", "d_L", "d_R"]


@pytest.mark.unit
def test_qcd_particles_match_cpp() -> None:
    model = build()
    assert sorted(p.name for p in model.particles) == sorted(CPP_PARTICLES)
