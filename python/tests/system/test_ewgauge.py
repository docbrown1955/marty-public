"""Port of tests/system/src/EWGauge.cpp and libsrc/example_ewgauge.cpp.

Tree-level W Z -> W Z squared amplitudes: the total, the no-Higgs, the
triple-gauge (W or Gp mediated) and the quartic subsets. The four functions are
generated as one library built with quadruple precision inside its C code
(section 9 of the phase 6 spec); the Python interface stays double. The 2 -> 2
scan of example_ewgauge.cpp is reproduced and compared with EWgauge.txt at 1e-3.
"""

import inspect
import math
from pathlib import Path

import pytest

from pymarty import config
from pymarty._pymarty import simulation as _simulation
from pymarty.calculation import (
    Order,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.errors import MartyNotImplementedError
from pymarty.modeling.models.sm import SM
from pymarty.simulation.library import Library
from pymarty.symbolic import Expr
from pymarty.testing import compare

REFERENCE_DIR = Path(__file__).parents[3] / "tests/system/data/output"
TOLERANCE = 1e-3
E_CM_START, E_CM_STOP, E_CM_STEP = 500, 5000, 100
N_ANGLES = 100
M_W, M_Z, M_H = 80.379, 91.10, 125.1
W_LEGS = [incoming("W"), incoming("Z"), outgoing("W"), outgoing("Z")]


@pytest.fixture(scope="module")
def model() -> SM:
    return SM()


def test_ew_gauge_subset_squared_amplitudes(model: SM) -> None:
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("W"), incoming("Z"), outgoing("W"), outgoing("Z")],
    )
    assert amplitude.diagrams

    no_higgs = amplitude.filter(lambda d: "h" not in d.particles)
    triple_gauge = no_higgs.filter(lambda d: "W" in d.mediators or "Gp" in d.mediators)
    quartic = no_higgs.filter(
        lambda d: "W" not in d.mediators and "Gp" not in d.mediators
    )
    assert len(triple_gauge.diagrams) + len(quartic.diagrams) == len(no_higgs.diagrams)
    assert quartic.diagrams

    for subset in (amplitude, no_higgs, triple_gauge, quartic):
        assert isinstance(compute_squared_amplitude(model, subset), Expr)


@pytest.fixture(scope="module")
def ewgauge_library(model: SM, tmp_path_factory):
    amplitude = compute_amplitude(model, Order.TREE, W_LEGS)
    no_higgs = amplitude.filter(lambda d: "h" not in d.particles)
    triple_gauge = no_higgs.filter(lambda d: "W" in d.mediators or "Gp" in d.mediators)
    quartic = no_higgs.filter(
        lambda d: "W" not in d.mediators and "Gp" not in d.mediators
    )
    functions = {
        "M2_tot": compute_squared_amplitude(model, amplitude),
        "M2_V3": compute_squared_amplitude(model, quartic),
        "M2_V4": compute_squared_amplitude(model, triple_gauge),
        "M2_nH": compute_squared_amplitude(model, no_higgs),
    }

    generator = _simulation.Generator(
        "ewgauge", str(tmp_path_factory.mktemp("ewgauge"))
    )
    generator.add_include_path(config.resolve().include_path)
    try:
        generator.set_quadruple_precision(True)
    except MartyNotImplementedError:
        pytest.skip("MARTY was built without quadruple precision")
    for name, expr in functions.items():
        generator.add_function(name, expr)
    generator.print_sources()
    lib = Library("ewgauge", generator)
    lib.build(n_jobs=4)
    return lib.load()


def _kinematics(e_cm: float, theta: float) -> tuple[dict[str, float], float, float]:
    """Invariants and squared momenta of the 2 -> 2 process, as Kinematics<2, 2>.

    The masses are (M_W, M_Z, M_W, M_Z). The invariants are stored under the
    names used by example_ewgauge.cpp, which maps s_ij to the pair index (i, j)
    of the process with 1-based labels.
    """
    masses = (M_W, M_Z, M_W, M_Z)

    def energy_split(e: float, ma: float, mb: float) -> float:
        return (e * e + ma * ma - mb * mb) / (2 * e)

    e0 = energy_split(e_cm, masses[0], masses[1])
    e1 = energy_split(e_cm, masses[1], masses[0])
    e2 = energy_split(e_cm, masses[2], masses[3])
    e3 = energy_split(e_cm, masses[3], masses[2])
    pi2 = e0 * e0 - masses[0] ** 2
    pf2 = e2 * e2 - masses[2] ** 2
    cross = math.sqrt(pi2) * math.sqrt(pf2) * math.cos(theta)
    s_01 = e0 * e1 + pi2
    s_02 = e0 * e2 - cross
    s_03 = e0 * e3 + cross
    s_12 = e1 * e2 + cross
    s_13 = e1 * e3 - cross
    s_23 = e2 * e3 + pf2
    invariants = {
        "s_12": s_01,
        "s_13": s_02,
        "s_14": s_03,
        "s_23": s_12,
        "s_24": s_13,
        "s_34": s_23,
    }
    return invariants, pi2, pf2


def _sigma(func, e_cm: float) -> float:
    """Angular integral of the squared amplitude, as sigma() in EWGauge.cpp."""
    params = {
        "M_W": M_W,
        "M_Z": M_Z,
        "e_em": math.sqrt(4 * math.pi / 137.0),
        "m_h": M_H,
        "theta_W": math.acos(M_W / M_Z),
        "reg_prop": 0.0,
    }
    accepted = inspect.signature(func).parameters
    integral = 0.0
    for i in range(N_ANGLES):
        theta = i * math.pi / (N_ANGLES - 1)
        invariants, pi2, pf2 = _kinematics(e_cm, theta)
        kwargs = {k: v for k, v in {**params, **invariants}.items() if k in accepted}
        m2 = func(**kwargs).real
        integral += (
            1
            / (32 * math.pi * math.pi * e_cm * e_cm)
            * math.sqrt(abs(pf2 / pi2))
            * (math.sin(theta) * m2)
        )
    return integral


@pytest.mark.needs_compiler
@pytest.mark.slow
@pytest.mark.quad
def test_ewgauge_scan_matches_reference(ewgauge_library, tmp_path) -> None:
    lines = []
    e_cm = E_CM_START
    while e_cm <= E_CM_STOP:
        values = [
            _sigma(ewgauge_library.M2_V3, e_cm),
            _sigma(ewgauge_library.M2_V4, e_cm),
            _sigma(ewgauge_library.M2_nH, e_cm),
            _sigma(ewgauge_library.M2_tot, e_cm),
        ]
        lines.append("\t".join(f"{v:g}" for v in [e_cm, *values]))
        e_cm += E_CM_STEP
    result = tmp_path / "EWgauge.txt"
    result.write_text("\n".join(lines) + "\n")
    comparison = compare(REFERENCE_DIR / "EWgauge.txt", result, TOLERANCE)
    assert comparison.passed, (
        f"EWgauge.txt: max diff {comparison.maximum:.3e}, "
        f"average {comparison.average:.3e}"
    )
