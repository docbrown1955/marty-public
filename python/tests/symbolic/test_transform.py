"""Transformations, evaluation and numeric conversion of the symbolic API."""

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError, MartyValueError
from pymarty.symbolic import Expr, sin


@pytest.mark.unit
def test_copy_returns_an_equal_new_expression() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    expr = (x + y) ** 2
    assert expr.copy() == expr
    assert expr.copy() is not expr
    assert expr.copy(deep=True) == expr


@pytest.mark.unit
def test_refresh_returns_an_equal_new_expression() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    expr = x * y + sin(x)
    assert expr.refresh() == expr
    assert expr.refresh() is not expr
    assert expr.refresh(deep=True) == expr


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ExpandBinomial")
def test_expand_binomial_square() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert ((x + y) ** 2).expand() == x**2 + 2 * x * y + y**2


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.FactorCommonFactor")
def test_factor_common_factor() -> None:
    a = symbolic.variable("a")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (a * x + a * y).factor() == a * (x + y)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.FactorNoCommonFactorIsIdentity")
def test_factor_without_common_factor_is_identity() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (x + y).factor() == x + y


@pytest.mark.unit
def test_factor_by_given_factor() -> None:
    a = symbolic.variable("a")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (a * x + a * y).factor(by=a) == a * (x + y)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.CollectBySymbol")
def test_collect_by_symbol() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (a * x + b * x + y).collect([x]) == (a + b) * x + y


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.DiffPolynomial")
def test_diff_polynomial() -> None:
    # y is a constant: a plain variable may depend on other variables in CSL.
    x = symbolic.variable("x")
    y = symbolic.constant("y")
    assert (x**3 + 2 * x * y).diff(x) == 3 * x**2 + 2 * y


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.DiffChainRule")
def test_diff_chain_rule() -> None:
    x = symbolic.variable("x")
    assert symbolic.sin(x**2).diff(x) == symbolic.cos(x**2) * 2 * x


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.SubsSymbolSwap")
def test_replace_swaps_symbols_simultaneously() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (x + 2 * y).replace({x: y, y: x}) == y + 2 * x


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.SubsDoesNotMutateOriginal")
def test_replace_does_not_mutate_original() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    z = symbolic.variable("z")
    e = x + y
    before = str(e)
    result = e.replace({x: z})
    assert result == z + y
    assert e == x + y
    assert str(e) == before


@pytest.mark.unit
def test_no_transform_mutates_its_input() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    e = (x + y) ** 2 + symbolic.I * x
    before_key = e.structural_key()
    before_text = str(e)
    before_latex = e.to_latex()
    free_before = e.free_symbols

    e.expand()
    e.factor()
    e.collect([x])
    e.diff(x)
    e.replace({x: y})
    e.evaluate({x: 2})
    e.conjugate()
    e.real_part()
    e.imag_part()

    assert e.structural_key() == before_key
    assert str(e) == before_text
    assert e.to_latex() == before_latex
    assert e.free_symbols == free_before


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EvaluateWithValues")
def test_evaluate_with_values() -> None:
    x = symbolic.variable("x")
    assert (x**2 + 1).evaluate({x: 3}) == 10


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EvaluateWithFreeSymbolsKeepsThem")
def test_evaluate_keeps_free_symbols() -> None:
    x = symbolic.variable("x")
    y = symbolic.constant("y")
    result = (x**2 + y).evaluate({x: 2})
    assert result == 4 + y
    assert result.depends_on(y)
    assert not result.depends_on(x)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.EvaluateAmbiguousNameRaises")
def test_evaluate_ambiguous_name_raises() -> None:
    x1 = symbolic.variable("x")
    x2 = symbolic.variable("x")
    y = symbolic.variable("y")
    with pytest.raises(MartyValueError):
        (x1 + x2).evaluate(x=2)
    assert (x1 + y).evaluate(x=2) == 2 + y


@pytest.mark.unit
def test_evaluate_rejects_two_positional_mappings() -> None:
    x = symbolic.variable("x")
    with pytest.raises(TypeError):
        x.evaluate({x: 1}, {x: 2})  # type: ignore[call-arg,arg-type]


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ConjugateInvolution")
def test_conjugate_is_an_involution() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    e = a + symbolic.I * b
    assert e.conjugate().conjugate() == e


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ConjugateOfImaginaryUnit")
def test_conjugate_of_imaginary_unit() -> None:
    assert symbolic.I.conjugate() == -symbolic.I


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.RealPartOfRealSymbol")
def test_real_part_of_real_symbol() -> None:
    x = symbolic.variable("x")
    assert x.real_part() == x


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ImagPartOfRealSymbol")
def test_imag_part_of_real_symbol() -> None:
    x = symbolic.variable("x")
    assert x.imag_part() == 0


@pytest.mark.unit
def test_depends_on_and_free_symbols() -> None:
    x = symbolic.variable("x")
    y = symbolic.constant("y")
    e = 2 * x * y + 1
    assert isinstance(e.free_symbols, frozenset)
    assert e.free_symbols == frozenset({x, y})
    assert e.depends_on(x)
    assert not Expr(3).depends_on(x)


@pytest.mark.unit
def test_name_of_symbol() -> None:
    x = symbolic.variable("x")
    assert x.name == "x"
    assert symbolic.constant("gamma").name == "gamma"


@pytest.mark.unit
def test_name_of_compound_expression_raises() -> None:
    x = symbolic.variable("x")
    with pytest.raises(MartyTypeError):
        _ = (x + 1).name


@pytest.mark.unit
def test_collect_rejects_string_factor() -> None:
    x = symbolic.variable("x")
    with pytest.raises(TypeError):
        x.collect(["x"])


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.CollectIndexedFactorThrows")
def test_collect_indexed_factor_raises() -> None:
    V = symbolic.Space("V", 3)
    y = symbolic.tensor("y", [V])
    yi = y(V.index("i"))
    x = symbolic.variable("x")
    z = symbolic.variable("z")
    with pytest.raises(MartyTypeError):
        (x * yi + z * yi).collect([yi])


@pytest.mark.unit
def test_float_of_real_number() -> None:
    assert float(Expr(3)) == 3.0
    assert float(Expr(0.5)) == 0.5


@pytest.mark.unit
def test_int_of_integer_only() -> None:
    assert int(Expr(3)) == 3
    with pytest.raises(ValueError):
        int(Expr(0.5))


@pytest.mark.unit
def test_complex_of_numbers() -> None:
    assert complex(Expr(2)) == 2 + 0j
    assert complex(symbolic.I) == 1j


@pytest.mark.unit
def test_float_of_complex_with_imaginary_part_raises() -> None:
    with pytest.raises(ValueError):
        float(symbolic.I)


@pytest.mark.unit
def test_numeric_conversion_of_symbol_raises() -> None:
    x = symbolic.variable("x")
    with pytest.raises(ValueError):
        float(x)
    with pytest.raises(ValueError):
        complex(x)
    with pytest.raises(ValueError):
        int(x)
