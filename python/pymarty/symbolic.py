"""Symbolic expressions: numbers, symbols, operators and elementary functions.

Build expressions with ``variable()`` and ``constant()``, with the operators and
with the functions of this module (``sqrt``, ``exp``, ``log``, ``sin``, ...).
The ``Expr`` methods transform expressions: ``expand()``, ``factor()``,
``collect()``, ``diff()``, ``replace()`` and ``evaluate()``.

Indices and tensors: ``Space`` creates indices, ``tensor()`` creates tensors,
and ``delta``, ``metric`` and ``epsilon`` are the built-in tensors.
``tensor_field()`` and ``tensor_derivative()`` build tensor fields and their
derivatives with respect to a point tensor.
``Expr.relabel_indices()`` renames the indices of an expression. ``Minkowski`` and
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
    ComplexProperty,
    Euclid_R2,
    Euclid_R3,
    Euclid_R4,
    Expr,
    I,
    Index,
    IndexType,
    Minkowski,
    Space,
    Tensor,
    TensorDerivative,
    TensorField,
    abs,
    acos,
    acosh,
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
    sin,
    sinh,
    sqrt,
    tan,
    tanh,
    tensor,
    tensor_derivative,
    tensor_field,
    variable,
)

__all__ = [
    "DEFAULT_INDEX_NAMES",
    "ComplexProperty",
    "Euclid_R2",
    "Euclid_R3",
    "Euclid_R4",
    "I",
    "Expr",
    "Index",
    "IndexType",
    "Minkowski",
    "Space",
    "Tensor",
    "TensorDerivative",
    "TensorField",
    "abs",
    "acos",
    "acosh",
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
    "sin",
    "sinh",
    "sqrt",
    "tan",
    "tanh",
    "tensor",
    "tensor_derivative",
    "tensor_field",
    "variable",
]
