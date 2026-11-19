"""Abbreviations: named sub-expressions expanded by ``evaluate``."""

import pytest

from pymarty import symbolic
from pymarty.symbolic import EvalMode


@pytest.mark.unit
def test_abbreviation_is_replaced_by_its_definition() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    ab = symbolic.make_abbreviation("Cab", a * b)
    assert (3 * ab).evaluate(mode=EvalMode.Abbreviation) == 3 * a * b


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EvaluateAbbreviationModeExpandsNestedAbbreviations")
def test_nested_abbreviations_are_expanded_fully() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    inner = symbolic.make_abbreviation("Cin", a + b)
    outer = symbolic.make_abbreviation("Cout", 2 * inner)
    assert outer.evaluate(mode=EvalMode.Abbreviation) == 2 * (a + b)


@pytest.mark.unit
def test_expression_without_abbreviations_is_unchanged() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    assert (a + b).evaluate(mode=EvalMode.Abbreviation) == a + b


@pytest.mark.unit
def test_expansion_does_not_modify_the_abbreviation() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    ab = symbolic.make_abbreviation("Cab", a * b)
    expanded = ab.evaluate(mode=EvalMode.Abbreviation)
    assert expanded == a * b
    assert ab != a * b
    assert ab.evaluate(mode=EvalMode.Abbreviation) == a * b


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EvaluateAllExpandsAbbreviations")
def test_default_evaluate_expands_abbreviations() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    ab = symbolic.make_abbreviation("Cdef", a * b)
    assert (3 * ab).evaluate() == 3 * a * b


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EvaluateBaseModeKeepsAbbreviations")
def test_base_mode_keeps_abbreviations() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    ab = symbolic.make_abbreviation("Cbase", a * b)
    assert (3 * ab).evaluate(mode=EvalMode.Base) == 3 * ab


@pytest.mark.unit
def test_eval_modes_combine_as_flags() -> None:
    combined = (
        EvalMode.Literal
        | EvalMode.Numerical
        | EvalMode.Indicial
        | EvalMode.Abbreviation
    )
    assert combined == EvalMode.All


@pytest.mark.unit
def test_combined_mode_without_abbreviation_keeps_abbreviations() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    ab = symbolic.make_abbreviation("Ccomb", a * b)
    mode = EvalMode.Literal | EvalMode.Numerical
    assert (3 * ab).evaluate(mode=mode) == 3 * ab


@pytest.mark.unit
def test_combined_mode_with_abbreviation_expands_abbreviations() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    ab = symbolic.make_abbreviation("Cflag", a * b)
    mode = EvalMode.Base | EvalMode.Abbreviation
    assert (3 * ab).evaluate(mode=mode) == 3 * a * b


@pytest.mark.unit
def test_eval_mode_and_tests_membership() -> None:
    mode = EvalMode.Literal | EvalMode.Abbreviation
    assert mode & EvalMode.Abbreviation == EvalMode.Abbreviation
    assert mode & EvalMode.Numerical == EvalMode.Base
