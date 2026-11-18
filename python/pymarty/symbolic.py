"""Symbolic expressions: numbers, symbols, operators and elementary functions.

Build expressions with ``variable()`` and ``constant()``, with the operators and
with the functions of this module (``sqrt``, ``exp``, ``log``, ``sin``, ...).
The ``Expr`` methods transform expressions: ``expand()``, ``factor()``,
``collect()``, ``diff()``, ``replace()`` and ``evaluate()``.

Indices and tensors: ``Space`` creates indices, ``tensor()`` creates tensors,
and ``delta``, ``metric`` and ``epsilon`` are the built-in tensors.
``relabel()`` renames the indices of an expression. ``Minkowski`` and
``Euclid_R2``, ``Euclid_R3``, ``Euclid_R4`` are predefined spaces.

Constants: ``I`` (imaginary unit), ``pi``, ``e`` and ``infinity``.
Errors are described in ``pymarty.errors``.

>>> from pymarty import symbolic
>>> x = symbolic.variable("x")
>>> symbolic.sqrt(4) == 2
True
"""

from pymarty._pymarty import (
    DEFAULT_INDEX_NAMES,
    Euclid_R2,
    Euclid_R3,
    Euclid_R4,
    Expr,
    I,
    Index,
    IndexRenaming,
    IndexType,
    Minkowski,
    Space,
    Tensor,
    abs,
    acos,
    acosh,
    apply_renaming,
    argument,
    asin,
    asinh,
    atan,
    atanh,
    constant,
    cos,
    cosh,
    delta,
    e,
    epsilon,
    exp,
    infinity,
    log,
    metric,
    modulus,
    pi,
    relabel,
    relabel_with_renaming,
    sin,
    sinh,
    sqrt,
    tan,
    tanh,
    tensor,
    variable,
)

__all__ = [
    "DEFAULT_INDEX_NAMES",
    "Euclid_R2",
    "Euclid_R3",
    "Euclid_R4",
    "I",
    "Expr",
    "Index",
    "IndexRenaming",
    "IndexType",
    "Minkowski",
    "Space",
    "Tensor",
    "abs",
    "acos",
    "acosh",
    "apply_renaming",
    "argument",
    "asin",
    "asinh",
    "atan",
    "atanh",
    "constant",
    "cos",
    "cosh",
    "delta",
    "e",
    "epsilon",
    "exp",
    "infinity",
    "log",
    "metric",
    "modulus",
    "pi",
    "relabel",
    "relabel_with_renaming",
    "sin",
    "sinh",
    "sqrt",
    "tan",
    "tanh",
    "tensor",
    "variable",
]
