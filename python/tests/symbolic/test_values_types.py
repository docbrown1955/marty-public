"""Value, type, shape and latex name accessors of Expr."""

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ValueOfNumberAndValuedFlag")
def test_value_of_number_and_valued_flag() -> None:
    x = symbolic.variable("x")
    assert symbolic.Expr(3).valued is True
    assert symbolic.Expr(3).value == 3.0
    assert x.valued is False


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ValueOfSymbolThrows")
def test_value_of_symbol_raises_type_error() -> None:
    x = symbolic.variable("x")
    with pytest.raises(MartyTypeError):
        _ = x.value


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.TypeAndPrimaryTypeOfExpressions")
def test_type_and_primary_type_are_enums() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert x.type == symbolic.ExprType.Variable
    assert x.primary_type == symbolic.PrimaryType.Literal
    assert (x + y).type == symbolic.ExprType.Sum


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.ShapeOfScalarAndLatexName")
def test_shape_of_scalar_and_latex_name() -> None:
    x = symbolic.variable("x")
    y = symbolic.variable("y")
    assert x.shape == (1,)
    assert x.latex_name == "x"
    assert (x + y).latex_name == ""
