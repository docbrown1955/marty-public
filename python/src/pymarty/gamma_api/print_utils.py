from . import Expr
from typing import Union
from IPython.display import display, Latex


def _get_latex_display(latex_str: str) -> Latex:
    return Latex(
        data=f"\\begin{{equation*}}\\begin{{split}}{latex_str}\\end{{split}}\\end{{equation*}}"
    )


def _print_latex(latex_str: str) -> None:
    display(_get_latex_display(latex_str=latex_str))


def _latex_from_expr(expr: Union[str, Expr]) -> str:
    if isinstance(expr, Expr):
        return expr.to_latex()
    if isinstance(expr, str):
        return expr
    raise TypeError(
        f"Cannot generate latex string from object {expr!r} of type {type(expr)!r}."
        "Only Expr and str objects are supported."
    )


def _process_str(s: str, split) -> str:
    if not split:
        return s.replace("&", "").replace("\\\\", "")
    return s


def print_latex(*args, split=True, **kwargs) -> None:
    if len(args) == 1:
        arg = args[0]
        if isinstance(arg, dict):
            print_latex(**arg, split=split)
        else:
            _print_latex(_process_str(_latex_from_expr(arg.to_latex()), split=split))
    elif len(args) == 2:
        _print_latex(
            f"{_process_str(_latex_from_expr(args[0]), split=False)}"
            "\\ =\\ "
            f"{_process_str(_latex_from_expr(args[1]), split=split)}"
        )
    elif "lhs" in kwargs and "rhs" in kwargs:
        print_latex(kwargs["lhs"], kwargs["rhs"], split=split)
    else:
        raise ValueError(
            f"Wrong arguments given to print_latex(), please read the help"
            + help(print_latex)
        )
