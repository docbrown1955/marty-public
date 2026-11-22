"""Model constants shared by name, as the members of the C++ model classes."""

from pymarty.symbolic import Expr
from pymarty.symbolic import constant as _new_constant

_CONSTANTS: dict[str, Expr] = {}


def constant(name: str) -> Expr:
    """Return the constant ``name``, creating it on first use.

    ``pymarty.symbolic.constant`` creates a new symbol at each call, so a
    parameter used by several builder functions would not match. The C++
    models declare each parameter once; this helper does the same.
    """
    if name not in _CONSTANTS:
        _CONSTANTS[name] = _new_constant(name)
    return _CONSTANTS[name]
