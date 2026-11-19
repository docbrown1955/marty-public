"""Python duplicates of the CSL callback algorithms.

CSL algorithms such as ``AnyOfLeafs`` or ``FindIfNode`` take C++ callbacks, which
pybind11 does not expose. The functions below are written in Python with the same
traversal order and depth semantics as the CSL versions. ``tests/api`` and
``python/tests/symbolic/test_algorithms.py`` check them against the same expressions.

A negative ``depth`` (the default) means no limit. ``depth=0`` looks at the root only.
"""

from __future__ import annotations

from collections.abc import Callable

from pymarty._pymarty import Expr

__all__ = [
    "all_of_leafs",
    "all_of_nodes",
    "any_of_leafs",
    "any_of_nodes",
    "find_if_leaf",
    "find_if_node",
    "visit_each_node_cut",
    "visit_first_of_leaf",
    "visit_first_of_node",
]

Predicate = Callable[[Expr], bool]


def _child_depth(depth: int) -> int:
    """Depth left for the children of a node, keeping -1 as unlimited."""
    return depth - 1 if depth > 0 else depth


def any_of_leafs(expr: Expr, predicate: Predicate, *, depth: int = -1) -> bool:
    """Whether some leaf satisfies the predicate. Returns a bool.

    A leaf is an expression without arguments. Below ``depth`` levels, non-leaf
    expressions are not descended into.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> any_of_leafs(x + y, lambda e: e.type == ExprType.Variable and e.name == "y")
    True
    """
    if not expr.args:
        return predicate(expr)
    if depth == 0:
        return False
    return any(any_of_leafs(a, predicate, depth=_child_depth(depth)) for a in expr.args)


def all_of_leafs(expr: Expr, predicate: Predicate, *, depth: int = -1) -> bool:
    """Whether every leaf satisfies the predicate. Returns a bool.

    With ``depth=0`` a non-leaf expression is not descended into, so the result is
    True (vacuous), as in CSL.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> all_of_leafs(x + y, lambda e: e.type == ExprType.Variable)
    True
    """
    if not expr.args:
        return predicate(expr)
    if depth == 0:
        return True
    return all(all_of_leafs(a, predicate, depth=_child_depth(depth)) for a in expr.args)


def any_of_nodes(expr: Expr, predicate: Predicate, *, depth: int = -1) -> bool:
    """Whether some node, the root included, satisfies the predicate. Returns a bool.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> any_of_nodes(x * y, lambda e: e.type == ExprType.Prod, depth=0)
    True
    """
    if predicate(expr):
        return True
    if depth == 0:
        return False
    return any(any_of_nodes(a, predicate, depth=_child_depth(depth)) for a in expr.args)


def all_of_nodes(expr: Expr, predicate: Predicate, *, depth: int = -1) -> bool:
    """Whether every node, the root included, satisfies the predicate. Returns a bool.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> all_of_nodes(x + y, lambda e: e.type == ExprType.Sum, depth=0)
    True
    """
    if not predicate(expr):
        return False
    if depth == 0:
        return True
    return all(all_of_nodes(a, predicate, depth=_child_depth(depth)) for a in expr.args)


def find_if_leaf(expr: Expr, predicate: Predicate, *, depth: int = -1) -> Expr | None:
    """First leaf, in depth-first order, satisfying the predicate, or None.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> is_y = lambda e: e.type == ExprType.Variable and e.name == "y"
    >>> print(find_if_leaf(x + y, is_y))
    y
    """
    if not expr.args:
        return expr if predicate(expr) else None
    if depth == 0:
        return None
    for arg in expr.args:
        found = find_if_leaf(arg, predicate, depth=_child_depth(depth))
        if found is not None:
            return found
    return None


def find_if_node(expr: Expr, predicate: Predicate, *, depth: int = -1) -> Expr | None:
    """First node, in pre-order (the root first), satisfying the predicate, or None.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> print(find_if_node(x * y + 1, lambda e: e.type == ExprType.Prod))
    x*y
    """
    if predicate(expr):
        return expr
    if depth == 0:
        return None
    for arg in expr.args:
        found = find_if_node(arg, predicate, depth=_child_depth(depth))
        if found is not None:
            return found
    return None


def visit_each_node_cut(
    expr: Expr, visitor: Callable[[Expr], bool], *, depth: int = -1
) -> None:
    """Calls the visitor on each node in pre-order and does not descend below a node
    for which it returns True.

    >>> from pymarty.symbolic import ExprType, variable
    >>> x, y = variable("x"), variable("y")
    >>> seen = []
    >>> visit_each_node_cut(
    ...     x * y + 1, lambda e: seen.append(e) or e.type == ExprType.Prod
    ... )
    >>> len(seen)
    3
    """
    if visitor(expr) or depth == 0:
        return
    for arg in expr.args:
        visit_each_node_cut(arg, visitor, depth=_child_depth(depth))


def visit_first_of_node(
    expr: Expr, visitor: Callable[[Expr], bool], *, depth: int = -1
) -> bool:
    """Calls the visitor on nodes in pre-order until it returns True.

    Returns True if a node was accepted by the visitor, False otherwise.

    >>> from pymarty.symbolic import variable
    >>> x, y = variable("x"), variable("y")
    >>> visit_first_of_node(x + y, lambda e: e == y)
    True
    """
    if visitor(expr):
        return True
    if depth == 0:
        return False
    return any(
        visit_first_of_node(a, visitor, depth=_child_depth(depth)) for a in expr.args
    )


def visit_first_of_leaf(
    expr: Expr, visitor: Callable[[Expr], bool], *, depth: int = -1
) -> bool:
    """Calls the visitor on leaves in depth-first order until it returns True.

    Returns True if a leaf was accepted by the visitor, False otherwise.

    >>> from pymarty.symbolic import variable
    >>> x, y = variable("x"), variable("y")
    >>> visit_first_of_leaf(x + y, lambda e: e == y)
    True
    """
    if not expr.args:
        return visitor(expr)
    if depth == 0:
        return False
    return any(
        visit_first_of_leaf(a, visitor, depth=_child_depth(depth)) for a in expr.args
    )
