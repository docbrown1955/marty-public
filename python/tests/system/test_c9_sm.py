"""Port of tests/system/src/C9_SM.cpp: the b -> s mu mu semileptonic coefficient.

Three one-loop amplitudes give the Wilson coefficients C9_BZ (no photon), C9_A
(photon only) and C9_Tot (all diagrams), in quadruple precision. The box and Z
penguin pieces and the analytic photon piece are the closed forms of
libsrc/example_c9_sm.cpp, computed here in double precision. The slow test
compares the m_t scan with data/output/C9_SM.txt at 1e-5.
"""

import dataclasses
import math
from pathlib import Path
from typing import Any

import pytest

from pymarty import config, simulation, symbolic
from pymarty.calculation import (
    Amplitude,
    FeynOptions,
    Order,
    ParticleFilter,
    anti,
    compute_amplitude,
    dimension6_coefficient,
    disable_particles,
    force_particle,
    incoming,
    outgoing,
    set_options,
    wilson_coefficients,
)
from pymarty.modeling.models.sm import SM, ckm_matrix
from pymarty.symbolic import Expr, constant
from pymarty.testing import compare

REFERENCE_DIR = Path(__file__).parents[3] / "tests/system/data/output"
TOLERANCE = 1e-5
M_W = 80.379
M_Z = 91.2
M_B = 4.18
M_S = 0.148
E_EM = math.sqrt(4 * math.pi / 137)
MU_MASS = 0.0
REG_PROP = 1e-3
DECOUPLED = ("h", "u", "c")
M_T_RANGE = range(120, 400)


def _operator_factor() -> Expr:
    ckm = ckm_matrix()
    v_ts = ckm[2][1]
    v_tb = ckm[2][2]
    e_em = constant("e_em")
    m_w = constant("M_W;M_{W}")
    theta_w = constant("theta_W")
    return (
        -1
        * e_em**4
        * v_ts.conjugate()
        * v_tb
        / (32 * symbolic.pi**2 * m_w**2 * symbolic.sin(theta_w) ** 2)
    )


def _amplitude(model: SM, filters: list[ParticleFilter]) -> Amplitude:
    options = FeynOptions(
        filters=[disable_particles(*DECOUPLED), *filters],
        fermion_order=[1, 0, 2, 3],
    )
    return compute_amplitude(
        model,
        Order.ONE_LOOP,
        [
            incoming("b"),
            outgoing("s"),
            outgoing("mu"),
            outgoing(anti("mu")),
        ],
        options=options,
    )


def _c9_functions() -> dict[str, Expr]:
    """C9_BZ, C9_A and C9_Tot, each with the options of its C++ getter."""
    model = SM()
    functions: dict[str, Expr] = {}
    # C++ getC9BZ keeps only the first mass in loops and drops the photon.
    with set_options(keep_only_first_mass_in_loop=True):
        amplitude = _amplitude(model, [disable_particles("A")])
        wilsons = wilson_coefficients(model, amplitude, _operator_factor())
        functions["C9_BZ"] = dimension6_coefficient(model, wilsons, "VL", "V")
    with set_options(keep_only_first_mass_in_loop=False):
        amplitude = _amplitude(model, [force_particle("A")])
        wilsons = wilson_coefficients(model, amplitude, _operator_factor())
        functions["C9_A"] = dimension6_coefficient(model, wilsons, "VL", "V")
        amplitude = _amplitude(model, [])
        wilsons = wilson_coefficients(model, amplitude, _operator_factor())
        functions["C9_Tot"] = dimension6_coefficient(model, wilsons, "VL", "V")
    return functions


def _b0(x: float) -> float:
    return x / (4 * (x - 1) ** 2) * math.log(x) + 1 / (4 * (1 - x))


def _c0(x: float) -> float:
    x2 = x * x
    return (3 * x2 + 2 * x) / (8 * (1 - x) ** 2) * math.log(x) + (-x2 + 6 * x) / (
        8 * (1 - x)
    )


def _d0(x: float) -> float:
    x2 = x * x
    x3 = x2 * x
    x4 = x2 * x2
    return (-3 * x4 + 30 * x3 - 54 * x2 + 32 * x - 8) / (18 * (1 - x) ** 4) * math.log(
        x
    ) + (-47 * x3 + 237 * x2 - 312 * x + 104) / (108 * (1 - x) ** 3)


@pytest.fixture(scope="module")
def c9_library(tmp_path_factory: pytest.TempPathFactory) -> Any:
    lib = simulation.generate(
        "c9_sm",
        _c9_functions(),
        path=tmp_path_factory.mktemp("c9_sm"),
        quadruple_precision=True,
    )
    lib.build(n_jobs=4)
    return lib.load()


@pytest.mark.needs_compiler
@pytest.mark.slow
@pytest.mark.skipif(
    not config.Config.quad_available,
    reason="MARTY was built without quadruple precision",
)
def test_c9_sm_m_t_scan_matches_reference(
    c9_library: Any, sm_wolfenstein: dict[str, Any], tmp_path: Path
) -> None:
    fields = {f.name for f in dataclasses.fields(c9_library.Params)}
    theta_w = math.acos(M_W / M_Z)
    sin2_w = math.sin(theta_w) ** 2
    inputs = {
        "M_W": sm_wolfenstein["M_W"],
        "V_ts": sm_wolfenstein["V_ts"],
        "V_tb": sm_wolfenstein["V_tb"],
        "m_b": M_B,
        "m_s": M_S,
        "e_em": E_EM,
        "theta_W": theta_w,
        "s_12": (M_B * M_B + M_S * M_S) / 2,
        "s_34": -MU_MASS * MU_MASS,
        "m_mu": MU_MASS,
        "reg_prop": REG_PROP,
        "Finite": 1,
        "mu": M_W,
    }
    lines = []
    for m_t in M_T_RANGE:
        params = c9_library.Params(
            **{k: v for k, v in {**inputs, "m_t": float(m_t)}.items() if k in fields}
        )
        x = (m_t / M_W) ** 2
        box = -_b0(x) / sin2_w
        z_penguin = _c0(x) * (1 - 4 * sin2_w) / sin2_w
        photon = -_d0(x)
        values = [
            m_t,
            complex(c9_library.C9_BZ(params)).real,
            box + z_penguin,
            complex(c9_library.C9_A(params)).real,
            complex(c9_library.C9_Tot(params)).real,
            photon,
        ]
        lines.append("\t".join(repr(float(v)) for v in values))
    result = tmp_path / "C9_SM.txt"
    result.write_text("\n".join(lines) + "\n")
    comparison = compare(REFERENCE_DIR / "C9_SM.txt", result, TOLERANCE)
    assert comparison.passed, (
        f"C9_SM.txt: max diff {comparison.maximum:.3e}, "
        f"average {comparison.average:.3e}"
    )
