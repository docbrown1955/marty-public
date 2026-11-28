"""Export and viewing of the diagrams of an amplitude or of the Feynman rules.

Diagrams are drawn by GRAFED (``show``), or rendered as LaTeX and PNG, for
example to display them in a notebook.
"""

from __future__ import annotations

from typing import Any

from pymarty._pymarty import calculation as _core
from pymarty.calculation import Amplitude, FeynmanRules
from pymarty.errors import MartyTypeError
from pymarty.visualization import grafed


def _native(obj: Amplitude | FeynmanRules) -> Any:
    if isinstance(obj, Amplitude):
        return obj._native
    if isinstance(obj, FeynmanRules):
        return obj
    raise MartyTypeError(
        f"expected a calculation.Amplitude or FeynmanRules, not {type(obj).__name__}"
    )


def _amplitude_native(amplitude: Amplitude) -> Any:
    if not isinstance(amplitude, Amplitude):
        raise MartyTypeError(
            f"expected a calculation.Amplitude, not {type(amplitude).__name__}"
        )
    return amplitude._native


def export_json(amplitude: Amplitude) -> str:
    """Diagram file of an amplitude, in the format read by GRAFED."""
    return _core.visualization.diagrams_json(_amplitude_native(amplitude))


def save(amplitude: Amplitude, path: str) -> None:
    """Writes the diagram file of an amplitude to ``path``, owner-readable only."""
    _core.visualization.save_diagrams(_amplitude_native(amplitude), str(path))


def latex(obj: Amplitude | FeynmanRules) -> str:
    """Standalone LaTeX document drawing the diagrams of an amplitude.

    For Feynman rules, the document holds one block per rule.
    """
    if isinstance(obj, FeynmanRules):
        return _core.visualization.rules_latex(obj)
    return _core.visualization.diagrams_latex(_native(obj))


def render_png(obj: Amplitude | FeynmanRules) -> bytes:
    """PNG bytes of the LaTeX document of ``obj``, e.g. for ``IPython.display.Image``.

    Requires ``latex`` and ``dvipng`` on the PATH. Raises ``MartyStateError``
    when one of them is missing or LaTeX fails.
    """
    return _core.visualization.render_png(latex(obj))


def show(amplitude: Amplitude, *, wait: bool = False) -> int:
    """Opens the diagrams of an amplitude in GRAFED.

    Without ``wait``, returns the process id and leaves the diagram file in a
    temporary directory that GRAFED reads. With ``wait``, returns the exit
    status and removes the file. Raises ``MartyStateError`` when GRAFED is
    not found.
    """
    return grafed.show(amplitude, wait=wait)


__all__ = ["export_json", "grafed", "latex", "render_png", "save", "show"]
