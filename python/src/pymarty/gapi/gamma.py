from typing import Optional, List

from .._internal import pymartyc

from .expr import Expr


def set_symbolic_dimension(symbolic: bool) -> None:
    pymartyc.set_symbolic_dimension(symbolic)


def gamma(mu: int, nu: Optional[int] = None) -> Expr:
    return Expr(pymartyc.gamma(mu) if nu is None else pymartyc.gamma(mu, nu))


def g(mu: int, nu: int) -> Expr:
    return Expr(pymartyc.g(mu, nu))


def eps(mu: int, nu: int, rho: int, sigma: int) -> Expr:
    return Expr(pymartyc.eps(mu, nu, rho, sigma))


def gamma5() -> Expr:
    return Expr(pymartyc.gamma5())


def P_L() -> Expr:
    return Expr(pymartyc.P_L())


def P_R() -> Expr:
    return Expr(pymartyc.P_R())


def C() -> Expr:
    return Expr(pymartyc.C())


def chain(gammas: List[int], left_current_index: int, right_current_index: int) -> Expr:
    return Expr(
        pymartyc.chain(
            [expr._expr for expr in gammas], left_current_index, right_current_index
        )
    )


def simplify(expr: Expr) -> Expr:
    return Expr(pymartyc.simplify(expr._expr))


def single_fierz(left_current: Expr, right_current: Expr) -> Expr:
    return Expr(pymartyc.single_fierz(left_current._expr, right_current._expr))


def double_fierz(left_current: Expr, right_current: Expr) -> Expr:
    return Expr(pymartyc.double_fierz(left_current._expr, right_current._expr))
