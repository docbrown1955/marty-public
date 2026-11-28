"""Port of tests/system/src/C7_SM.cpp: the b -> s gamma chromomagnetic coefficient.

The one-loop amplitude in Feynman gauge is reduced to the Wilson coefficients
of the chromomagnetic operators, with the same prefactor as the C++ test. The
slow test generates, builds and loads a library from these coefficients and
compares the m_t scan of libsrc/example_c7_sm.cpp with data/output/C7_SM.txt
at 1e-5.
"""

import dataclasses
from pathlib import Path

import pytest

from pymarty import simulation, symbolic
from pymarty.calculation import (
    FeynOptions,
    Order,
    chromo_magnetic_coefficient,
    compute_amplitude,
    force_particle,
    incoming,
    outgoing,
    wilson_coefficients,
)
from pymarty.modeling import GaugeChoice
from pymarty.modeling.models.sm import SM, ckm_matrix
from pymarty.symbolic import Expr, constant
from pymarty.testing import compare

REFERENCE_DIR = Path(__file__).parents[3] / "tests/system/data/output"
TOLERANCE = 1e-5
M_B = 4.18
M_S = 0.095
E_EM = 0.01
THETA_W = 2.0


def _amplitude():
    model = SM(finalize=False)
    model.particle("W").set_gauge_choice(GaugeChoice.FEYNMAN)
    model.finalize()
    options = FeynOptions(filters=[force_particle("t")])
    amplitude = compute_amplitude(
        model,
        Order.ONE_LOOP,
        [incoming("b"), outgoing("s"), outgoing("A")],
        options=options,
    )
    return model, amplitude


def _c7_prefactor() -> Expr:
    ckm = ckm_matrix()
    v_ts = ckm[2][1]
    v_tb = ckm[2][2]
    e_em = constant("e_em")
    m_b = constant("m_b;m_{b}")
    m_w = constant("M_W;M_{W}")
    theta_w = constant("theta_W")
    return (
        -1
        * e_em**3
        * v_ts.conjugate()
        * v_tb
        * m_b
        / (32 * symbolic.pi**2 * m_w**2 * symbolic.sin(theta_w) ** 2)
    )


@pytest.mark.loops
def test_c7_sm_feynman_gauge_wilson_coefficients() -> None:
    model, amplitude = _amplitude()
    diagrams = amplitude.diagrams
    assert diagrams
    assert all(
        "A" in d.particles and any(p.startswith("t_") for p in d.particles)
        for d in diagrams
    )

    wilsons = wilson_coefficients(model, amplitude, _c7_prefactor())
    assert isinstance(chromo_magnetic_coefficient(model, wilsons, "R"), Expr)
    assert isinstance(chromo_magnetic_coefficient(model, wilsons, "L"), Expr)


@pytest.fixture(scope="module")
def c7_library(tmp_path_factory):
    model, amplitude = _amplitude()
    wilsons = wilson_coefficients(model, amplitude, _c7_prefactor())
    functions = {
        "C7": chromo_magnetic_coefficient(model, wilsons, "R"),
        "C7_p": chromo_magnetic_coefficient(model, wilsons, "L"),
    }
    lib = simulation.generate("c7_sm", functions, path=tmp_path_factory.mktemp("c7_sm"))
    lib.build(n_jobs=4)
    return lib.load()


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_c7_sm_m_t_scan_matches_reference(c7_library, sm_wolfenstein, tmp_path) -> None:
    fields = {f.name for f in dataclasses.fields(c7_library.Params)}
    inputs = {
        "m_b": M_B,
        "m_s": M_S,
        "e_em": E_EM,
        "s_12": (M_B * M_B + M_S * M_S) / 2,
        "theta_W": THETA_W,
        "M_W": sm_wolfenstein["M_W"],
        "V_ts": sm_wolfenstein["V_ts"],
        "V_tb": sm_wolfenstein["V_tb"],
    }
    lines = []
    for m_t in range(50, 301):
        params = c7_library.Params(
            **{k: v for k, v in {**inputs, "m_t": float(m_t)}.items() if k in fields}
        )
        lines.append(repr(complex(c7_library.C7(params)).real))
    result = tmp_path / "C7_SM.txt"
    result.write_text("\n".join(lines) + "\n")
    comparison = compare(REFERENCE_DIR / "C7_SM.txt", result, TOLERANCE)
    assert comparison.passed, (
        f"C7_SM.txt: max diff {comparison.maximum:.3e}, "
        f"average {comparison.average:.3e}"
    )
