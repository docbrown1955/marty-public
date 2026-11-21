"""Text and LaTeX output of the symbolic API."""

import pytest

from pymarty import symbolic
from pymarty.symbolic import Expr


@pytest.mark.unit
def test_latex_of_abs_and_common_functions() -> None:
    a = symbolic.variable("a")
    assert symbolic.abs(a).to_latex() == "\\left|a\\right|"
    assert symbolic.cos(a).to_latex() == "\\cos\\left(a\\right)"
    assert symbolic.exp(a).to_latex() == "e^{a}"


@pytest.mark.unit
def test_latex_of_coefficient_with_abs_and_denominator() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    c = symbolic.variable("c")
    expr = 2 * symbolic.abs(a) * symbolic.cos(b) / c
    assert expr.to_latex() == "\\frac{2\\left|a\\right|\\cos\\left(b\\right)}{c}"


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.LatexMatchesExistingCslOutput")
def test_latex_matches_existing_csl_output() -> None:
    # Expected strings: tests/units/csl/test_latex.cpp, as in the C++ test.
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    c = symbolic.variable("c")
    assert (a + symbolic.I * b).to_latex() == "a + i\\,b"
    assert (a - b / 2).to_latex() == "a - \\frac{b}{2}"
    assert (a * (b + c)).to_latex() == "a\\cdot\\left(b + c\\right)"
    assert (a**-1).to_latex() == "\\frac{1}{a}"
    assert ((a + b) ** 2).to_latex() == "\\left(a + b\\right)^{2}"
    assert symbolic.sqrt(a).to_latex() == "\\sqrt{a}"
    assert symbolic.exp(a + b).to_latex() == "e^{a + b}"
    assert symbolic.cos(a).to_latex() == "\\cos\\left(a\\right)"


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.CodeOutputStable")
def test_code_output_stable() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    e = (x + y) ** 2 - 3 * x
    assert str(e) == "(-3)*x + (x + y)^2"
    assert str(e) == str(e)
    assert e.to_latex() == "-3x + \\left(x + y\\right)^{2}"
    assert e.to_latex() == e.to_latex()


@pytest.mark.unit
def test_str_returns_text_output() -> None:
    x = symbolic.variable("x")
    assert isinstance(str(x), str)
    assert str(x) == "x"
    assert str(Expr(3)) == "3"


@pytest.mark.unit
def test_repr_wraps_text_output() -> None:
    x = symbolic.variable("x")
    assert repr(x) == "<Expr: x>"
    e = (x + symbolic.variable("y")) ** 2 - 3 * x
    assert repr(e) == "<Expr: (-3)*x + (x + y)^2>"
    assert "\n" not in repr(e)
    assert str(e) == "(-3)*x + (x + y)^2"


@pytest.mark.unit
def test_repr_latex_wraps_latex_in_dollars() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    e = x + y
    assert e._repr_latex_() == f"${e.to_latex()}$"
    assert e._repr_latex_() == "$x + y$"


@pytest.mark.unit
def test_latex_of_imaginary_unit_and_number() -> None:
    assert symbolic.I.to_latex() == "i"
    assert Expr(3).to_latex() == "3"
