"""Model constants shared by name, as the members of the C++ model classes."""

from pymarty.symbolic import ComplexProperty, Expr
from pymarty.symbolic import constant as _new_constant

_CONSTANTS: dict[str, Expr] = {}


def constant(
    name: str, *, complex_property: ComplexProperty = ComplexProperty.Real
) -> Expr:
    """Return the constant ``name``, creating it on first use.

    ``pymarty.symbolic.constant`` creates a new symbol at each call, so a
    parameter used by several builder functions would not match. The C++
    models declare each parameter once; this helper does the same.
    The complex property is only used when the constant is created.
    """
    if name not in _CONSTANTS:
        _CONSTANTS[name] = _new_constant(name, complex_property=complex_property)
    return _CONSTANTS[name]
