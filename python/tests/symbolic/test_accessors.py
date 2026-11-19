"""Structure accessors of Expr: args, argument, operand, factors, term, exponents."""

from fractions import Fraction

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError
from pymarty.symbolic import Expr


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ArgumentsOfSumAreTerms")
def test_args_of_sum_are_its_terms() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    args = (x + y).args
    assert len(args) == 2
    assert x in args
    assert y in args


@pytest.mark.unit
def test_leaf_has_no_args() -> None:
    x = symbolic.variable("x")
    assert x.args == ()


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ArgumentOutOfRangeThrows")
def test_argument_out_of_range_raises_lookup_error() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    with pytest.raises(LookupError):
        (x * y).argument(2)
    with pytest.raises(LookupError):
        x.argument(0)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.OperandOfNonOperatorThrows")
def test_operand_of_non_operator_raises_type_error() -> None:
    x = symbolic.variable("x")
    with pytest.raises(MartyTypeError):
        _ = x.operand


@pytest.mark.unit
def test_operand_of_derivative_is_the_argument() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert y.diff(x).operand == y


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.NumericalFactorAndTerm")
def test_numerical_factor_and_term_split_the_coefficient() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert (3 * x).numerical_factor == 3
    assert (x + y).numerical_factor == 1
    assert (3 * x * y).term == x * y


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.FactorsAndExponentsOfProduct")
def test_factors_and_exponents_of_product() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    z = symbolic.variable("z")
    expr = x**2 * y
    assert len(expr.factors) == 3
    exps = expr.exponents([x, y, z])
    assert exps[0] == 2
    assert exps[1] == 1
    assert exps[2] == 0


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.PredicatesOfNumbersAndSymbols")
def test_predicates_of_numbers_and_symbols() -> None:
    x = symbolic.variable("x")
    assert Expr(3).is_integer
    assert Expr(3).is_real
    assert not Expr(Fraction(1, 2)).is_integer
    assert Expr(1.5).is_real
    assert not Expr(1.5).is_integer
    assert not Expr(3).is_indexed
    assert not Expr(3).is_an_operator
    assert not x.is_an_operator
    assert not x.is_indexed


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.PredicatesOfOperators")
def test_predicates_of_operators() -> None:
    z = symbolic.variable("z", complex_property=symbolic.ComplexProperty.Complex)
    assert z.conjugate().is_complex_conjugate
    assert not z.is_complex_conjugate
    assert z.real_part().is_an_operator
    assert not z.is_an_operator
