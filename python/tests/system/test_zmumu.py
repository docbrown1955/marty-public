"""Port of testZmumu.cpp: e+e- -> mu+mu- with a custom U(1) Z boson.

The three squared amplitudes (Dirac, left- and right-handed electrons) are
generated as one library. The scans of libsrc/example_zmumu.cpp are
reproduced and compared with the reference files at 1e-5.
"""

import inspect
import math
from pathlib import Path

import pytest

from pymarty import simulation
from pymarty.calculation import (
    Field,
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    left,
    outgoing,
    right,
)
from pymarty.modeling.models.zmumu import Zmumu
from pymarty.testing import compare

REFERENCE_DIR = Path(__file__).parents[3] / "tests/system/data/output"
TOLERANCE = 1e-5
NANOBARN_CONVERSION = 2.56819e-6
DTHETA = math.pi / 100


@pytest.fixture(scope="module")
def zmumu_library(tmp_path_factory):
    model = Zmumu()
    functions = {}
    dirac = [incoming("e"), incoming(anti("e")), outgoing("mu"), outgoing(anti("mu"))]
    functions["Zmumu_full"] = compute_squared_amplitude(
        model, compute_amplitude(model, Order.TREE, dirac)
    )
    for name, electron in (("Zmumu_L", left("e")), ("Zmumu_R", right("e"))):
        chirality = "left" if name.endswith("L") else "right"
        legs = [
            incoming(electron),
            incoming(Field("e", antiparticle=True, chirality=chirality)),
            outgoing("mu"),
            outgoing(anti("mu")),
        ]
        functions[name] = compute_squared_amplitude(
            model, compute_amplitude(model, Order.TREE, legs)
        )

    lib = simulation.generate("zmumu", functions, path=tmp_path_factory.mktemp("zmumu"))
    lib.build(n_jobs=4)
    return lib.load()


def _parameters() -> dict[str, float]:
    e_em = math.sqrt(4 * math.pi / 137)
    mw, mz, gz = 80.379, 91.19, 2.49
    theta_w = math.acos(mw / mz)
    cw = math.cos(theta_w)
    s2w = math.sin(theta_w) ** 2
    g_l = e_em / math.sin(theta_w)
    return {
        "g_Y": e_em,
        "aL": g_l / cw * (-1.0 / 2 + s2w),
        "aR": g_l / cw * s2w,
        "M_Z": mz,
        "G_Z": gz,
        "reg_prop": 0.0,
    }


def _dsigma(
    func, params: dict[str, float], e_cm: float, theta: float, forward: bool
) -> float:
    """Differential cross section in nb for massless 2 -> 2 kinematics.

    Mirrors dsigmaCalc() of example_zmumu.cpp, where pf/pi = 1 for massless
    final states.
    """
    energy = e_cm / 2
    momentum_2 = energy * energy
    sign = 1.0 if forward else -1.0
    cross = sign * momentum_2 * math.cos(theta)
    invariants = {
        "s_12": energy * energy + momentum_2,
        "s_13": energy * energy - cross,
        "s_14": energy * energy + cross,
        "s_23": energy * energy + cross,
        "s_24": energy * energy - cross,
    }
    kwargs = {**params, **invariants}
    accepted = inspect.signature(func).parameters
    m2 = func(**{k: v for k, v in kwargs.items() if k in accepted}).real
    return m2 / (64 * math.pi**2 * e_cm * e_cm) / NANOBARN_CONVERSION


def _compare_scan(tmp_path, reference_name: str, lines: list[str]) -> None:
    result = tmp_path / reference_name
    result.write_text("\n".join(lines) + "\n")
    comparison = compare(REFERENCE_DIR / reference_name, result, TOLERANCE)
    assert comparison.passed, (
        f"{reference_name}: max diff {comparison.maximum:.3e}, "
        f"average {comparison.average:.3e}"
    )


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_total_cross_section(zmumu_library, tmp_path) -> None:
    params = _parameters()
    func = zmumu_library.Zmumu_full
    lines = []
    e_cm = 4.0
    while e_cm < 150:
        sigma = 0.0
        theta = 0.0
        while theta < math.pi:
            factor = DTHETA * math.sin(theta) * 2 * math.pi
            sigma += factor * _dsigma(func, params, e_cm, theta, forward=True)
            theta += DTHETA
        lines.append(f"{e_cm} {sigma}")
        e_cm += 0.1
    _compare_scan(tmp_path, "ee_mumu_tot.txt", lines)


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_forward_backward_asymmetry(zmumu_library, tmp_path) -> None:
    params = _parameters()
    func = zmumu_library.Zmumu_full
    lines = []
    e_cm = 55.0
    while e_cm < 185:
        theta = math.pi / 3
        sigma_f = _dsigma(func, params, e_cm, theta, forward=True)
        sigma_b = _dsigma(func, params, e_cm, theta, forward=False)
        lines.append(f"{e_cm} {(sigma_f - sigma_b) / (sigma_b + sigma_f)}")
        e_cm += 0.1
    _compare_scan(tmp_path, "ee_mumu_AFB.txt", lines)


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_left_right_asymmetry(zmumu_library, tmp_path) -> None:
    params = _parameters()
    e_cm = 87.0
    max_theta = math.acos(0.9)
    lines = []
    theta = max_theta
    while theta < math.pi - max_theta:
        m2_left = (
            2
            * math.pi
            * _dsigma(
                zmumu_library.Zmumu_L, params, e_cm, math.pi - theta, forward=True
            )
        )
        m2_right = (
            2
            * math.pi
            * _dsigma(
                zmumu_library.Zmumu_R, params, e_cm, math.pi - theta, forward=True
            )
        )
        lines.append(f"{e_cm} {theta} {m2_left} {m2_right}")
        theta += DTHETA
    _compare_scan(tmp_path, "ee_mumu_LR.txt", lines)
