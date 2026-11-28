"""Numerical inputs shared by the SM system tests."""

import cmath
import dataclasses
import math
import re

import pytest

from pymarty.symbolic import EvalMode


@pytest.fixture(scope="session")
def sm_wolfenstein() -> dict[str, complex | float]:
    """CKM entries and M_W baked into the C++ sm.h defaults (PDG 2021 Wolfenstein)."""
    lam, big_a, rho, eta = 0.22650, 0.790, 0.141, 0.357
    s12 = lam
    s23 = big_a * lam**2
    s13 = big_a * lam**3 * math.hypot(rho, eta)
    c12 = math.sqrt(1 - s12**2)
    c13 = math.sqrt(1 - s13**2)
    c23 = math.sqrt(1 - s23**2)
    phase = cmath.exp(1j * math.atan2(eta, rho))
    return {
        "M_W": 80.379,
        "V_ts": -c12 * s23 - s12 * c23 * s13 * phase,
        "V_tb": c23 * c13,
    }


@pytest.fixture
def assert_library_matches_symbolic():
    """Check a generated library function against its symbolic expression.

    Every free symbol of the expression gets a fixed nonzero value; the
    library and ``Expr.evaluate`` must then agree at relative 1e-10.
    """

    def check(library, function: str, expr, complex_values: bool = False) -> None:
        fields = {f.name: str(f.type) for f in dataclasses.fields(library.Params)}
        expanded = expr.evaluate(mode=EvalMode.Abbreviation)
        values = {}
        params_kwargs = {}
        for i, sym in enumerate(sorted(expanded.free_symbols, key=lambda s: s.name)):
            value = 0.3 + 0.07 * i
            field = _field_name(sym.name)
            if complex_values and "complex" in fields.get(field, ""):
                value = complex(0.3 + 0.07 * i, -0.11 + 0.05 * i)
            values[sym.name] = value
            if field in fields:
                params_kwargs[field] = value
        reference = complex(_substitute(expanded, values))
        assert reference != 0, f"{function}: symbolic reference vanishes"
        params = library.Params(**params_kwargs)
        result = complex(getattr(library, function)(params))
        assert abs(result - reference) <= 1e-10 * abs(reference), (
            f"{function}: library {result!r} vs symbolic {reference!r}"
        )

    return check


def _field_name(symbol: str) -> str:
    """Parameter name in the generated library (LaTeX braces and symbols removed)."""
    return re.sub(r"[^0-9A-Za-z_]", "", symbol)


def _substitute(expr, values: dict[str, complex | float]):
    """Substitute by name, conjugating the values of conjugated occurrences.

    Each free-symbol object is used as its own key; conjugated occurrences
    (``is_complex_conjugate``) receive the conjugate of the value.
    """
    for _ in range(4):
        if not expr.free_symbols:
            break
        mapping = {}
        for s in expr.free_symbols:
            if s.name not in values:
                continue
            value = values[s.name]
            mapping[s] = complex(value).conjugate() if s.is_complex_conjugate else value
        if not mapping:
            break
        expr = expr.evaluate(mapping)
    return expr
