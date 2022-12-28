from . import Expr
from typing import Optional, Any
from IPython.display import display, Latex


def _get_latex_display(latex_str: str) -> Latex:
    return Latex(
        data=f"\\begin{{equation*}}\\begin{{split}}{latex_str}\\end{{split}}\\end{{equation*}}"
    )


def print_latex(lhs: str, rhs: Optional[str] = None) -> None:
    if rhs is None:
        return display(_get_latex_display(lhs))
    else:
        return display(_get_latex_display(f"{lhs}\\ =\\ {rhs}"))
