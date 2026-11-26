"""Port of testGgToTt.cpp (gg -> ttbar reinterpretation) with the custom SU(3) model.

The C++ test uses the built-in SM. Here the minimal custom model of
gg_to_tt.cpp (USE_SM 0) is used, which gives the same tree-level result. The
gluon and ghost squared amplitudes are compared with the PDG 2020 formula
against reinterpretation.txt at 1e-5.
"""

import inspect
import math
from pathlib import Path

import pytest

from pymarty import simulation
from pymarty.calculation import (
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.modeling.models.gg_to_tt import GgToTt
from pymarty.testing import compare

REFERENCE_DIR = Path(__file__).parents[3] / "tests/system/data/output"
TOLERANCE = 1e-5
PB_CONVERSION = 2.56819e-9
MT = 172.76
ALPHAS = 0.1179


def _dsigma_pdg(alphas: float, mq: float, s: float, t: float, u: float) -> float:
    """PDG 2020 p. 259 formula for the differential cross section, in GeV^-2."""
    mq2 = mq * mq
    lt = (mq2 - t) * (mq2 - u)
    return (
        alphas
        * alphas
        / (32 * s)
        * math.sqrt(1 - 4 * mq2 / s)
        * (
            6 * lt / (s * s)
            - mq2 * (s - 4 * mq2) / (3 * lt)
            + 4.0 / 3 * (lt - 2 * mq2 * (mq2 + t)) / (mq2 - t) ** 2
            + 4.0 / 3 * (lt - 2 * mq2 * (mq2 + u)) / (mq2 - u) ** 2
            - 3 * (lt + mq2 * (u - t)) / (s * (mq2 - t))
            - 3 * (lt + mq2 * (t - u)) / (s * (mq2 - u))
        )
    )


@pytest.fixture(scope="module")
def gg_to_tt_library(tmp_path_factory):
    model = GgToTt()
    gluons = [incoming("G"), incoming("G"), outgoing("t"), outgoing(anti("t"))]
    ghost = [incoming("c_G"), incoming(anti("c_G")), outgoing("t"), outgoing(anti("t"))]
    ghost_c = [
        incoming(anti("c_G")),
        incoming("c_G"),
        outgoing("t"),
        outgoing(anti("t")),
    ]
    functions = {
        "M2": compute_squared_amplitude(
            model, compute_amplitude(model, Order.TREE, gluons)
        ),
        "M2_ghost": compute_squared_amplitude(
            model, compute_amplitude(model, Order.TREE, ghost)
        ),
        "M2_ghostc": compute_squared_amplitude(
            model, compute_amplitude(model, Order.TREE, ghost_c)
        ),
    }
    lib = simulation.generate(
        "gg_to_tt", functions, path=tmp_path_factory.mktemp("gg_to_tt")
    )
    lib.build(n_jobs=4)
    return lib.load()


@pytest.mark.needs_compiler
def test_reinterpretation_scan(gg_to_tt_library, tmp_path) -> None:
    lines = []
    e_cm = 400.0
    while e_cm < 5000:
        theta = math.pi / 20
        while theta < math.pi:
            ct = math.cos(theta)
            gs = math.sqrt(4 * math.pi * ALPHAS)
            mt2 = MT * MT
            s = e_cm * e_cm
            k = math.sqrt(1 - 4 * mt2 / s)
            s13 = s / 4 * (1 - k * ct)
            s14 = s / 4 * (1 + k * ct)
            params = {
                "g_s": gs,
                "m_t": MT,
                "s_12": s / 2,
                "s_13": s13,
                "s_14": s14,
                "s_23": s14,
                "s_24": s13,
                "s_34": s / 2 - mt2,
                "reg_prop": 0.0,
            }
            t = mt2 - 2 * s13
            u = mt2 - 2 * s14

            m2 = _evaluate(gg_to_tt_library.M2, params)
            m2g = _evaluate(gg_to_tt_library.M2_ghost, params)
            m2gc = _evaluate(gg_to_tt_library.M2_ghostc, params)
            m2 = m2 - m2g - m2gc

            dsigma = k * m2 / (64 * math.pi**2 * s) / PB_CONVERSION
            dsigma_pdg = _dsigma_pdg(ALPHAS, MT, s, t, u) / PB_CONVERSION
            lines.append(f"{e_cm} {theta} {dsigma} {dsigma_pdg}")
            theta += math.pi / 20
        e_cm += 25

    result = tmp_path / "reinterpretation.txt"
    result.write_text("\n".join(lines) + "\n")
    comparison = compare(REFERENCE_DIR / "reinterpretation.txt", result, TOLERANCE)
    assert comparison.passed, (
        f"max diff {comparison.maximum:.3e}, average {comparison.average:.3e}"
    )


def _evaluate(func, params: dict[str, float]) -> float:
    """Real part of a generated squared amplitude, given only its own parameters."""
    accepted = inspect.signature(func).parameters
    return func(**{k: v for k, v in params.items() if k in accepted}).real
