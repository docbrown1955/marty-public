"""Port of tests/system/src/simpleSpectrum.cpp and libsrc/example_simplespectrum.cpp.

The toy model with a dark SU(2) group is generated as a spectrum library. Its
six masses, computed with the parameters of the C++ program, are compared with
the reference file at 1e-10.
"""

from pathlib import Path

import pytest

from pymarty import config, simulation
from pymarty._pymarty import simulation as _simulation
from pymarty.modeling.models.simple_spectrum import SimpleSpectrum
from pymarty.testing import compare

REFERENCE_DIR = Path(__file__).parents[3] / "tests/system/data/output"
TOLERANCE = 1e-10
PARAMS = {
    "g_D": 0.05,
    "lSH": 0.25,
    "lSS": -0.33,
    "vph": 0.76,
    "MPsi": 10.0,
    "mhH2": 54.0,
}


@pytest.fixture(scope="module")
def spectrum_binding(tmp_path_factory):
    path = tmp_path_factory.mktemp("simplespectrum")
    generator = _simulation.Generator("simpleSpectrum", str(path))
    generator.add_include_path(config.resolve().include_path)
    generator.add_library_path(config.resolve().library_path)
    generator.add_spectrum(SimpleSpectrum())
    # The generator writes the binding and library files in lower case.
    lib = simulation.Library("simplespectrum", generator)
    generator.print_sources()
    lib.build(n_jobs=4)
    return lib.load()


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_simple_spectrum_masses_match_reference(spectrum_binding, tmp_path) -> None:
    # As in the C++ program, the reference holds the real part of each mass.
    masses = [
        spectrum_binding.m_H(lSH=PARAMS["lSH"], vph=PARAMS["vph"], mhH2=PARAMS["mhH2"]),
        spectrum_binding.m_VW(g_D=PARAMS["g_D"], vph=PARAMS["vph"]),
        spectrum_binding.m_VZ(g_D=PARAMS["g_D"], vph=PARAMS["vph"]),
        spectrum_binding.m_phR(lSS=PARAMS["lSS"], vph=PARAMS["vph"]),
        spectrum_binding.m_Psi_1(MPsi=PARAMS["MPsi"]),
        spectrum_binding.m_Psi_2(MPsi=PARAMS["MPsi"]),
    ]
    result = tmp_path / "simplespectrum.txt"
    result.write_text("".join(f"{m.real:.15g}\n" for m in masses), encoding="utf-8")

    comparison = compare(REFERENCE_DIR / "simplespectrum.txt", result, TOLERANCE)
    assert comparison.passed, comparison
