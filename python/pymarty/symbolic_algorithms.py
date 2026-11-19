"""Python duplicates of the CSL callback algorithms.

CSL algorithms such as ``AnyOfLeafs`` or ``FindIfNode`` take C++ callbacks, which
pybind11 does not expose. The functions below are written in Python with the same
traversal order and depth semantics as the CSL versions. ``tests/api`` and
``python/tests/symbolic/test_algorithms.py`` check them against the same expressions.

A negative ``depth`` (the default) means no limit. ``depth=0`` looks at the root only.
"""

from __future__ import annotations

import operator
from collections.abc import Callable, Iterable, Sequence
from functools import reduce

from pymarty._pymarty import Expr, ExprType

__all__ = [
    "all_of_leafs",
    "all_of_nodes",
    "any_of_leafs",
    "any_of_nodes",
    "distribute_if",
    "expand_if",
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


ONE = Expr(1)
ZERO = Expr(0)


def _prod(factors: Iterable[Expr]) -> Expr:
    """Product of the factors, built with the CSL operator."""
    return reduce(operator.mul, factors, ONE)


def _sum(terms: Iterable[Expr]) -> Expr:
    """Sum of the terms, built with the CSL operator."""
    return reduce(operator.add, terms, ZERO)


def distribute_if(expr: Expr, predicate: Predicate, *, full: bool = True) -> Expr:
    """Distributes the factors satisfying the predicate over the sums they multiply.

    Mirrors CSL ``DistributedIf``: for a product, each factor that satisfies the
    predicate is multiplied into every other sum factor, and the resulting terms
    are added. With ``full=True`` the result is distributed recursively.

    >>> from pymarty.symbolic import variable
    >>> x, y, z = variable("x"), variable("y"), variable("z")
    >>> is_z = lambda e: e.type == ExprType.Variable and e.name == "z"
    >>> print(distribute_if((x + y) * z, is_z, full=False))
    x*z + y*z
    """
    if expr.type == ExprType.Prod:
        for i, factor in enumerate(expr.args):
            if not predicate(factor):
                continue
            results: list[Expr] = []
            for j, other in enumerate(expr.args):
                if i != j and other.type == ExprType.Sum:
                    args = list(expr.args)
                    args[i] = ONE
                    args[j] = (factor * other).expand(full=True)
                    results.append(expr.rebuilt(args))
            if results:
                total = _sum(results)
                if total.type == ExprType.Sum:
                    return total.rebuilt(
                        [distribute_if(a, predicate, full=full) for a in total.args]
                    )
                if full:
                    return distribute_if(total, predicate, full=True)
                return total
    if full and expr.args:
        return expr.rebuilt([distribute_if(a, predicate, full=full) for a in expr.args])
    return expr


def expand_if(
    expr: Expr, predicate: Predicate, *, full: bool = True, inplace: bool = False
) -> Expr:
    """Expands the products and integer powers of sums, restricted by the predicate.

    Mirrors CSL ``ExpandedIf``. A sum factor is developed only if it satisfies the
    predicate, and then only the terms satisfying it are developed. With
    ``inplace=True`` the factors failing the predicate are kept outside the
    expanded sum. Sums and leaves are returned unchanged. Other function nodes
    with arguments are not ported and raise ``NotImplementedError``.

    >>> from pymarty.symbolic import variable
    >>> x, y, z = variable("x"), variable("y"), variable("z")
    >>> is_sum_or_x = lambda e: e.type == ExprType.Sum or (
    ...     e.type == ExprType.Variable and e.name == "x"
    ... )
    >>> print(expand_if((x + y) * z, is_sum_or_x, full=False))
    x*z + y*z
    """
    result = _expand_or_none(expr, predicate, full=full, inplace=inplace)
    return expr if result is None else result


def _expand_or_none(
    expr: Expr, predicate: Predicate, *, full: bool, inplace: bool
) -> Expr | None:
    """Expanded expression, or None when the node is unchanged."""
    if expr.type == ExprType.Prod:
        return _expand_factors(expr.args, predicate, full=full, inplace=inplace)
    if expr.type == ExprType.Pow:
        return _expand_pow(expr, predicate, full=full, inplace=inplace)
    if expr.type == ExprType.Sum or not expr.args:
        return None
    raise NotImplementedError(
        f"expand_if is not ported for {expr.type.name} nodes with arguments"
    )


def _expand_factors(
    factors: Sequence[Expr], predicate: Predicate, *, full: bool, inplace: bool
) -> Expr | None:
    """CSL ``Prod::expand_if`` on the factors; negative powers form the denominator."""
    args = list(factors)
    if full:
        args = [expand_if(a, predicate, full=True, inplace=inplace) for a in args]
    expanded = full
    in_place_factors: list[Expr] = []
    numerator: list[Expr] = []
    denom: list[Expr] = []
    for arg in args:
        exponent = arg.argument(1) if arg.type == ExprType.Pow else None
        if exponent is not None and exponent.is_integer and exponent.value < 0:
            denom.append(
                arg.argument(0)
                if exponent.value == -1
                else arg.argument(0) ** -exponent
            )
        else:
            numerator.append(arg)

    new_abstracts: list[list[Expr]] = [[ONE]]
    for arg in numerator:
        if arg.type == ExprType.Sum and predicate(arg):
            expanded = True
            to_develop = [a for a in arg.args if predicate(a)]
            kept = [a for a in arg.args if not predicate(a)]
            if kept:
                to_develop.append(_sum(kept))
            new_abstracts = [
                abstract + [term] for term in to_develop for abstract in new_abstracts
            ]
        elif not inplace or predicate(arg):
            new_abstracts = [abstract + [arg] for abstract in new_abstracts]
        else:
            in_place_factors.append(arg)

    expanded_denom: Expr | None = None
    if denom:
        expanded_denom = _expand_factors(denom, predicate, full=full, inplace=inplace)
        if not expanded and expanded_denom is None:
            return None
        if expanded_denom is None:
            expanded_denom = _prod(denom)

    terms = []
    for abstract in new_abstracts:
        if expanded_denom is not None:
            abstract = abstract + [ONE / expanded_denom]
        terms.append(_prod(abstract))
    if not inplace:
        return _sum(terms)
    return _prod(in_place_factors + [_sum(terms)])


def _expand_pow(
    expr: Expr, predicate: Predicate, *, full: bool, inplace: bool
) -> Expr | None:
    """CSL ``Pow::expand_if``: integer powers of a sum satisfying the predicate."""
    if not full:
        return None
    base = expand_if(expr.argument(0), predicate, full=True, inplace=inplace)
    exponent = expand_if(expr.argument(1), predicate, full=True, inplace=inplace)
    if (
        exponent.is_integer
        and exponent.value > 0
        and base.type == ExprType.Sum
        and predicate(base)
    ):
        result = ONE
        for _ in range(round(abs(exponent.value))):
            result = result * base
        return result.expand(full=True)
    return base**exponent
