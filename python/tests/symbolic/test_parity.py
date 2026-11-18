"""Parity of the symbolic API with the C++ facade: golden printing and numerics.

Printing (`-k printing`): each line of the golden file
`tests/api/data/symbolic/expressions.txt` is mapped to one Python builder, in
the same order as `goldenExpressions()` in `tests/api/src/symbolic_golden.cpp`.
The golden file holds 50 expressions, one builder for each.

Numeric (`-k numeric`): `float(expr)` and `complex(expr)` are compared with
Python references at relative tolerance 1e-12. The reference is Python `math`
(and `fractions`, `complex`), not the C++ library: no numeric golden file
exists.
"""

import math
from collections.abc import Callable
from fractions import Fraction
from pathlib import Path

import pytest

from pymarty.symbolic import (
    Expr,
    I,
    Minkowski,
    Space,
    asin,
    cos,
    delta,
    exp,
    log,
    metric,
    relabel,
    sin,
    sqrt,
    tan,
    tensor,
    variable,
)
from pymarty.symbolic import abs as sym_abs

GOLDEN_PATH = (
    Path(__file__).resolve().parents[3]
    / "tests"
    / "api"
    / "data"
    / "symbolic"
    / "expressions.txt"
)

REL_TOL = 1e-12


def _chain_space() -> Space:
    return Space("V", 3, index_names=("i", "j"))


def _y_times_m() -> Expr:
    v = _chain_space()
    i, j = v.index("i"), v.index("j")
    return tensor("y", [v])(i) * tensor("M", [v, v])(i, j)


def _y_m1_x() -> Expr:
    v = _chain_space()
    i, j = v.index("i"), v.index("j")
    return tensor("y", [v])(i) * tensor("M1;M_{1}", [v, v])(i, j) * tensor("x", [v])(j)


def _y_m1() -> Expr:
    v = _chain_space()
    i, j = v.index("i"), v.index("j")
    return tensor("y", [v])(i) * tensor("M1;M_{1}", [v, v])(i, j)


def _m1_trace() -> Expr:
    v = _chain_space()
    i = v.index("i")
    return tensor("M1;M_{1}", [v, v])(i, i)


def _alpha_space() -> Space:
    return Space("S", 4, index_names=("alpha;\\alpha", "beta;\\beta"))


def _dummy_pairs() -> Expr:
    s = _alpha_space()
    x, y = tensor("x", [s]), tensor("y", [s])
    p1, p2, p3 = s.index("alpha"), s.index("alpha"), s.index("alpha")
    return relabel(x(p1) * y(p1) * x(p2) * y(p2) * x(p3) * y(p3))


def _free_names_skipped() -> Expr:
    s = _alpha_space()
    x, y, z = tensor("x", [s]), tensor("y", [s]), tensor("z", [s])
    p, f = s.index("alpha"), s.index("alpha")
    return relabel(x(p) * y(p) * z(f))


def _relabelled_product() -> Expr:
    s = _alpha_space()
    x, y = tensor("x", [s]), tensor("y", [s])
    p = s.index("alpha")
    return relabel(x(p) * y(p))


def _free_index_renamed() -> Expr:
    s = _alpha_space()
    return relabel(tensor("z", [s])(s.index("beta")))


def _relabelled_delta() -> Expr:
    s = _alpha_space()
    return relabel(delta(s.index("alpha"), s.index("beta")))


def _relabelled_metric() -> Expr:
    mu, nu = Minkowski.index("\\mu"), Minkowski.index("\\nu")
    return relabel(metric(mu, nu))


def _relabelled_delta_upper() -> Expr:
    mu, nu = Minkowski.index("\\mu"), Minkowski.index("\\nu")
    return relabel(delta(mu, nu.flipped()))


def _unsigned_subscripts() -> Expr:
    u = Space("U", 4, index_names=("alpha", "beta"))
    return tensor("x", [u])(u.index("alpha"))


# One builder per golden line, in golden order.
GOLDEN_BUILDERS: list[Callable[[], Expr]] = [
    lambda: Expr(3),
    lambda: Expr(-3),
    lambda: variable("x"),
    lambda: variable("M_W;M_{W}"),
    lambda: I,
    lambda: Expr(2) * I,
    lambda: variable("a") + I * variable("b"),
    lambda: variable("a") + variable("b"),
    lambda: variable("a") - variable("b"),
    lambda: variable("a") - Expr(2) * variable("b"),
    lambda: variable("a") - variable("b") * variable("c"),
    lambda: variable("a") - variable("b") / Expr(2),
    lambda: variable("a") - I * variable("b"),
    lambda: Expr(2) * variable("a") * variable("b"),
    lambda: Expr(-2) * variable("a"),
    lambda: Expr(-1) * variable("a"),
    lambda: variable("a") * (variable("b") + variable("c")),
    lambda: variable("a") ** -1,
    lambda: variable("a") / variable("b"),
    lambda: variable("a") / (variable("b") + variable("c")),
    lambda: Expr(2) * variable("a") / variable("b"),
    lambda: variable("a") * variable("c") / variable("b"),
    lambda: variable("a") / Expr(2),
    lambda: variable("a") / (Expr(2) * variable("b")),
    lambda: variable("x") * variable("y") ** -1,
    lambda: variable("a") ** 2,
    lambda: variable("a") ** (variable("b") ** variable("c")),
    lambda: (variable("a") + variable("b")) ** 2,
    lambda: variable("a") ** -2,
    lambda: sqrt(variable("a")),
    lambda: sqrt(variable("a") + variable("b")),
    lambda: sym_abs(variable("a")),
    lambda: cos(variable("a")),
    lambda: exp(variable("a") + variable("b")),
    _y_times_m,
    _y_m1_x,
    _y_m1,
    _m1_trace,
    lambda: (variable("a") + variable("b") ** -1) ** 2,
    lambda: Expr(2) * sym_abs(variable("a")) * cos(variable("b")) / variable("c"),
    lambda: variable("a") ** (variable("b") / variable("c")),
    _dummy_pairs,
    _free_names_skipped,
    _relabelled_product,
    _free_index_renamed,
    _relabelled_delta,
    _relabelled_metric,
    _relabelled_delta_upper,
    _unsigned_subscripts,
    lambda: (variable("a") + variable("b")) * (variable("c") + variable("d")),
]

REAL_NUMERIC_CASES: list[tuple[str, Callable[[], Expr], float]] = [
    ("sqrt(2)", lambda: sqrt(2), math.sqrt(2)),
    ("exp(1)", lambda: exp(1), math.exp(1)),
    ("sin(1)", lambda: sin(1), math.sin(1)),
    ("cos(2)", lambda: cos(2), math.cos(2)),
    ("tan(0.5)", lambda: tan(0.5), math.tan(0.5)),
    ("log(10)", lambda: log(10), math.log(10)),
    ("asin(0.5)", lambda: asin(0.5), math.asin(0.5)),
    (
        "sqrt(2) * sqrt(3)",
        lambda: sqrt(2) * sqrt(3),
        math.sqrt(2) * math.sqrt(3),
    ),
    (
        "exp(sqrt(2)) * sin(3)",
        lambda: exp(sqrt(2)) * sin(3),
        math.exp(math.sqrt(2)) * math.sin(3),
    ),
    (
        "rational 1/3 + 2/7",
        lambda: Expr(Fraction(1, 3)) + Expr(Fraction(2, 7)),
        float(Fraction(1, 3) + Fraction(2, 7)),
    ),
    (
        "rational (3/4)**5",
        lambda: Expr(Fraction(3, 4)) ** 5,
        float(Fraction(3, 4) ** 5),
    ),
    ("float 0.1 + 0.2", lambda: Expr(0.1) + Expr(0.2), 0.1 + 0.2),
]

COMPLEX_NUMERIC_CASES: list[tuple[str, Callable[[], Expr], complex]] = [
    ("i**2", lambda: I**2, complex(-1, 0)),
    (
        "(1+2i)*(3-i)",
        lambda: Expr(1 + 2j) * Expr(3 - 1j),
        (1 + 2j) * (3 - 1j),
    ),
    ("exp(i)", lambda: exp(I), complex(math.cos(1), math.sin(1))),
]


def read_golden() -> list[str]:
    """Golden LaTeX lines, without the header comments and blank lines."""
    lines = GOLDEN_PATH.read_text(encoding="utf-8").splitlines()
    return [line for line in lines if line and not line.startswith("#")]


@pytest.mark.unit
def test_golden_printing_builders_match_golden_lines() -> None:
    assert len(read_golden()) == len(GOLDEN_BUILDERS)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.GoldenExpressionsStable")
def test_golden_expressions_printing_matches_cpp() -> None:
    golden = read_golden()
    assert len(golden) == len(GOLDEN_BUILDERS)
    for number, (expected, build) in enumerate(
        zip(golden, GOLDEN_BUILDERS, strict=True), start=1
    ):
        assert build().to_latex() == expected, f"golden expression {number}"


@pytest.mark.unit
@pytest.mark.parametrize(
    ("label", "build", "reference"),
    REAL_NUMERIC_CASES,
    ids=[case[0] for case in REAL_NUMERIC_CASES],
)
def test_numeric_real_values_match_python_math(
    label: str, build: Callable[[], Expr], reference: float
) -> None:
    assert math.isclose(float(build()), reference, rel_tol=REL_TOL, abs_tol=0.0)


@pytest.mark.unit
@pytest.mark.parametrize(
    ("label", "build", "reference"),
    COMPLEX_NUMERIC_CASES,
    ids=[case[0] for case in COMPLEX_NUMERIC_CASES],
)
def test_numeric_complex_values_match_python_math(
    label: str, build: Callable[[], Expr], reference: complex
) -> None:
    assert abs(complex(build()) - reference) <= REL_TOL * abs(reference)
