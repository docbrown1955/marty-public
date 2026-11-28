"""Port of tests/system/src/testModelWidth.cpp and libsrc/example_modelwidthlib.cpp.

The tree-level SM widths are generated as one library and evaluated at the
default parameters of src/marty/models/sm.h, with the Wolfenstein CKM matrix
(useWolfensteinCKM = true). The twelve widths are compared with
data/output/modelwidthlib.txt at 1e-5.
"""

import cmath
import math
from pathlib import Path

import pytest

from pymarty import config
from pymarty._pymarty import simulation as _simulation
from pymarty.calculation import Order
from pymarty.modeling.models.sm import SM
from pymarty.simulation import Library
from pymarty.testing import compare

REFERENCE = Path(__file__).parents[3] / "tests/system/data/output/modelwidthlib.txt"
TOLERANCE = 1e-5

# Order of f_G in tests/system/libs/modelwidthLib/src/cpp/group_g.cpp.
WIDTH_ORDER = [
    "Gamma_W",
    "Gamma_h",
    "Gamma_Z",
    "Gamma_u",
    "Gamma_c",
    "Gamma_t",
    "Gamma_d",
    "Gamma_s",
    "Gamma_b",
    "Gamma_e",
    "Gamma_mu",
    "Gamma_tau",
]


def _sm_inputs() -> dict[str, float | complex]:
    """Default inputs of sm.h, with the Wolfenstein CKM elements."""
    alpha_em = 1 / 137.035999139
    s2_theta_w = 0.23057
    a, lam, rho, eta = 0.790, 0.22650, 0.141, 0.357

    s12 = lam
    s23 = a * lam * lam
    s13 = a * lam**3 * math.sqrt(rho * rho + eta * eta)
    c12, c13, c23 = (math.sqrt(1 - s * s) for s in (s12, s13, s23))
    delta = math.atan2(eta, rho)
    phase = cmath.exp(1j * delta)

    return {
        "M_W": 80.379,
        "M_Z": 91.1876,
        "m_b": 4.18,
        "m_c": 1.275,
        "m_d": 4.7e-3,
        "m_e": 5.109989461e-4,
        "m_h": 125.18,
        "m_s": 0.095,
        "m_t": 173.0,
        "m_u": 2.2e-3,
        "m_mu": 0.105658375,
        "m_tau": 1.77686,
        "e_em": math.sqrt(4 * math.pi * alpha_em),
        "theta_W": math.asin(math.sqrt(s2_theta_w)),
        "V_ud": c12 * c13,
        "V_us": s12 * c13,
        "V_cb": s23 * c13,
        "V_tb": c23 * c13,
        "V_ub": s13 * cmath.exp(-1j * delta),
        "V_cd": -s12 * c23 - c12 * s23 * s13 * phase,
        "V_cs": c12 * c23 - s12 * s23 * s13 * phase,
        "V_td": s12 * s23 - c12 * c23 * s13 * phase,
        "V_ts": -c12 * s23 - s12 * c23 * s13 * phase,
    }


@pytest.fixture(scope="module")
def width_library(tmp_path_factory) -> Library:
    resolved = config.resolve()
    if not resolved.library_path:
        pytest.skip("no LoopTools library path configured")
    model = SM()
    generator = _simulation.Generator(
        "modelwidthLib", str(tmp_path_factory.mktemp("modelwidthLib"))
    )
    generator.add_include_path(resolved.include_path)
    generator.add_library_path(resolved.library_path)
    generator.compute_model_widths(model, Order.TREE)
    generator.add_spectrum(model)
    generator.print_sources()
    lib = Library("modelwidthlib", generator)
    lib.build(n_jobs=4)
    return lib


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_model_widths_match_reference(width_library: Library, tmp_path) -> None:
    module = width_library.load()
    params = module.Params(**_sm_inputs())
    widths = [getattr(module, name)(params).real for name in WIDTH_ORDER]

    result = tmp_path / "modelwidthlib.txt"
    result.write_text("\t".join(f"{w:.10g}" for w in widths) + "\n")

    comparison = compare(REFERENCE, result, TOLERANCE)
    assert comparison.passed, (
        f"max diff {comparison.maximum:.3e}, average {comparison.average:.3e}"
    )
