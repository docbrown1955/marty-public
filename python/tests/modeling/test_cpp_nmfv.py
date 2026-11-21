import pytest
from cpp_models.nmfv import build

# Particle names of MSSM_Model::initContent as printed by the C++ NMFV model
# (src/marty/models/nmfv.cpp, scratch probe of the pre-breaking field content).
CPP_PARTICLE_NAMES = sorted(
    [
        "G",
        "W",
        "B",
        "c_G",
        "c_W",
        "c_B",
        "sB",
        "sW",
        "sG",
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
    ]
)


@pytest.mark.unit
def test_nmfv_particle_names_match_cpp() -> None:
    model = build()
    assert sorted(p.name for p in model.particles) == CPP_PARTICLE_NAMES


@pytest.mark.unit
def test_nmfv_gaugino_and_higgs_content() -> None:
    model = build()
    names = {p.name for p in model.particles}
    assert {"sW", "sB", "sG", "sHu", "sHd", "Hu", "Hd"} <= names
    assert len(model.particles) == len(CPP_PARTICLE_NAMES)
