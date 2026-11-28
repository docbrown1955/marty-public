"""Port of tests/system/src/widthsSM.cpp: squared amplitudes behind the SM widths.

The squared amplitudes are generated as one library, built and loaded. The
widths are evaluated on the 1 -> 2 kinematics of libsrc/example_widths_lib.cpp
and compared with data/output/widths_with_hBB_1L.txt at 1e-5. As in the C++
test, the hAA_W amplitude is in the library but not in the reference.
"""

import math
from pathlib import Path

import pytest

from pymarty import config
from pymarty._pymarty import simulation as _simulation
from pymarty.calculation import (
    FeynOptions,
    Insertion,
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    force_particle,
    incoming,
    outgoing,
)
from pymarty.modeling.models.sm import SM
from pymarty.simulation import Library
from pymarty.symbolic import Expr
from pymarty.testing import compare

REFERENCE = (
    Path(__file__).parents[3] / "tests/system/data/output/widths_with_hBB_1L.txt"
)
TOLERANCE = 1e-5

M_T, M_H, M_W, M_Z = 173.0, 125.0, 80.0, 90.0
M_B, M_C, M_S, M_MU = 4.0, 0.0, 0.0, 0.0

SQUARED_AMPLITUDES = {
    "M2_hWW": ([incoming("h"), outgoing("W"), outgoing(anti("W"))], Order.TREE, None),
    "M2_hZZ": ([incoming("h"), outgoing("Z"), outgoing("Z")], Order.TREE, None),
    "M2_hbb": ([incoming("h"), outgoing("b"), outgoing(anti("b"))], Order.TREE, None),
    "M2_Wlnu": (
        [incoming("W"), outgoing("nu_mu"), outgoing(anti("mu"))],
        Order.TREE,
        None,
    ),
    "M2_Wcb": ([incoming("W"), outgoing("c"), outgoing(anti("b"))], Order.TREE, None),
    "M2_Zss": ([incoming("Z"), outgoing("s"), outgoing(anti("s"))], Order.TREE, None),
    "M2_hAA_t": (
        [incoming("h"), outgoing("A"), outgoing(anti("A"))],
        Order.ONE_LOOP,
        FeynOptions(filters=[force_particle("t")]),
    ),
    "M2_hGG": (
        [incoming("h"), outgoing("G"), outgoing(anti("G"))],
        Order.ONE_LOOP,
        FeynOptions(filters=[force_particle("t")]),
    ),
    "M2_hAA_W": (
        [incoming("h"), outgoing("A"), outgoing(anti("A"))],
        Order.ONE_LOOP,
        FeynOptions(filters=[force_particle("W", "Gp")]),
    ),
}

# Decay kinematics (M0, M1, M2) and m_b in effect, in the order of widthsSM.cpp.
# m_b is 4 until the W -> cb evaluation, which sets it to 0.
WIDTH_KINEMATICS = [
    ("M2_hbb", (M_H, M_B, M_B), M_B),
    ("M2_hWW", (M_H, M_W, M_W), M_B),
    ("M2_hZZ", (M_H, M_Z, M_Z), M_B),
    ("M2_Wlnu", (M_W, 0.0, M_MU), M_B),
    ("M2_Wcb", (M_W, M_C, 0.0), 0.0),
    ("M2_Zss", (M_Z, M_S, M_S), 0.0),
    ("M2_hAA_t", (M_H, 0.0, 0.0), 0.0),
    ("M2_hGG", (M_H, 0.0, 0.0), 0.0),
]
REFERENCE_ORDER = [
    "M2_hbb",
    "M2_hWW",
    "M2_hZZ",
    "M2_Wlnu",
    "M2_Wcb",
    "M2_Zss",
    "M2_hAA_t",
    "M2_hGG",
]


def _kinematics(m0: float, m1: float, m2: float) -> tuple[float, float, float, float]:
    """Return (s_12, s_13, s_23, p2) as set by fillKinematics_1to2()."""
    m0_2, m1_2, m2_2 = m0 * m0, m1 * m1, m2 * m2
    e1 = (m0_2 + m1_2 - m2_2) / (2 * m0)
    e2 = (m0_2 - m1_2 + m2_2) / (2 * m0)
    p2 = e2 * e2 - m2_2
    return e1 * m0, e2 * m0, e1 * e2 + p2, p2


def _width(m2: complex, m0: float, p2: float) -> float:
    return m2.real * math.sqrt(abs(p2)) / (8 * math.pi * m0 * m0)


@pytest.fixture(scope="module")
def sm() -> SM:
    return SM()


@pytest.mark.parametrize(
    "legs",
    [
        [incoming("h"), outgoing("W"), outgoing(anti("W"))],
        [incoming("h"), outgoing("Z"), outgoing("Z")],
        [incoming("h"), outgoing("b"), outgoing(anti("b"))],
        [incoming("W"), outgoing("nu_mu"), outgoing(anti("mu"))],
        [incoming("W"), outgoing("c"), outgoing(anti("b"))],
        [incoming("Z"), outgoing("s"), outgoing(anti("s"))],
    ],
    ids=["hWW", "hZZ", "hbb", "Wlnu", "Wcb", "Zss"],
)
def test_tree_level_squared_amplitude_of_a_width(sm: SM, legs: list[Insertion]) -> None:
    amplitude = compute_amplitude(sm, Order.TREE, legs)
    assert amplitude.diagrams
    assert isinstance(compute_squared_amplitude(sm, amplitude), Expr)


@pytest.mark.slow
def test_one_loop_h_to_photons_through_top_loop(sm: SM) -> None:
    amplitude = compute_amplitude(
        sm,
        Order.ONE_LOOP,
        [incoming("h"), outgoing("A"), outgoing(anti("A"))],
        options=FeynOptions(filters=[force_particle("t")]),
    )
    assert amplitude.diagrams
    assert all("t" in d.particles for d in amplitude.diagrams)
    assert isinstance(compute_squared_amplitude(sm, amplitude), Expr)


@pytest.fixture(scope="module")
def widths_library(tmp_path_factory) -> Library:
    sm = SM()
    functions = {}
    for name, (legs, order, options) in SQUARED_AMPLITUDES.items():
        if options is None:
            amplitude = compute_amplitude(sm, order, legs)
        else:
            amplitude = compute_amplitude(sm, order, legs, options=options)
        functions[name] = compute_squared_amplitude(sm, amplitude)

    generator = _simulation.Generator(
        "widthssm", str(tmp_path_factory.mktemp("widthsSM"))
    )
    generator.add_include_path(config.resolve().include_path)
    generator.add_library_path(config.resolve().library_path)
    for function_name, expr in functions.items():
        generator.add_function(function_name, expr)
    generator.print_sources()
    lib = Library("widthssm", generator)
    lib.build(n_jobs=4)
    return lib


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_widths_match_reference(widths_library: Library, tmp_path) -> None:
    module = widths_library.load()
    couplings = {
        "e_em": math.sqrt(4 * math.pi / 137),
        "M_W": M_W,
        "M_Z": M_Z,
        "theta_W": math.acos(M_W / M_Z),
        "V_cb": 0.0412,
        "m_s": M_S,
        "m_h": M_H,
        "m_t": M_T,
        "g_s": math.sqrt(4 * math.pi * 0.01),
        "Finite": 1.0,
    }
    widths = {}
    for name, (m0, m1, m2), m_b in WIDTH_KINEMATICS:
        s_12, s_13, s_23, p2 = _kinematics(m0, m1, m2)
        params = module.Params(**couplings, m_b=m_b, s_12=s_12, s_13=s_13, s_23=s_23)
        widths[name] = _width(getattr(module, name)(params), m0, p2)

    lines = []
    for name in REFERENCE_ORDER:
        lines.append(f"{widths[name]:.10g}")
        lines.append("")
    result = tmp_path / "widths_with_hBB_1L.txt"
    result.write_text("\n".join(lines) + "\n")

    comparison = compare(REFERENCE, result, TOLERANCE)
    assert comparison.passed, (
        f"max diff {comparison.maximum:.3e}, average {comparison.average:.3e}"
    )
