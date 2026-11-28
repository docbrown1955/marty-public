"""Error reporting of the symbolic API."""

import pytest

from pymarty.errors import MartyError, MartyZeroDivisionError
from pymarty.symbolic import Expr


@pytest.mark.unit
def test_zero_division_error_derives_from_builtin_and_marty_error() -> None:
    assert issubclass(MartyZeroDivisionError, MartyError)
    assert issubclass(MartyZeroDivisionError, ZeroDivisionError)


@pytest.mark.unit
@pytest.mark.parity("SymbolicApi.CSLErrorMessageNotLost")
def test_error_message_names_operation_and_cause() -> None:
    with pytest.raises(MartyZeroDivisionError) as excinfo:
        Expr(0) ** -1
    message = str(excinfo.value)
    assert message
    assert "power" in message
    assert "division by zero" in message


@pytest.mark.unit
def test_zero_division_is_catchable_as_builtin() -> None:
    with pytest.raises(ZeroDivisionError):
        Expr(0) ** -1
