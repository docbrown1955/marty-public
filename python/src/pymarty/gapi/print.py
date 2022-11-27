from typing import Union

from .expr import Expr


def print_latex(expr: Expr):
    """Prints out the latex equation for the expression in a ipython session."""
    from IPython.display import Latex

    latex_str = f"\\begin{{equation}}\\begin{{split}}{expr.to_latex()}\\end{{split}}\\end{{equation}}"
    return Latex(latex_str)


def print_equation(lhs: Union[str, Expr], rhs: Expr):
    """Prints out the latex equation for the expression in a ipython session."""
    from IPython.display import Latex

    if isinstance(lhs, Expr):
        lhs = lhs.to_latex()
    latex_str = f"\\begin{{equation}}\\begin{{split}}{lhs}\ =\ {rhs.to_latex()}\\end{{split}}\\end{{equation}}"
    return Latex(latex_str)
