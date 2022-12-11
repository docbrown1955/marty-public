from __future__ import annotations

import sys
from .._internal import pymartyc
from typing import Union


class Expr:
    """Symbolic expression with gamma matrices"""

    def __init__(self, expr: pymartyc.GExpr) -> None:
        self._expr = expr

    def __str__(self) -> str:
        return pymartyc.generate_string(self._expr)

    def __repr__(self) -> str:
        return str(self)

    def to_latex(self) -> str:
        return pymartyc.generate_latex(self._expr)
