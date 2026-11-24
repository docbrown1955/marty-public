"""Access to the GRAFED diagram viewer."""

from __future__ import annotations

import os
import shutil
import tempfile

from pymarty._pymarty import calculation as _core
from pymarty.calculation import Amplitude
from pymarty.errors import MartyStateError, MartyTypeError


def find(path: str | os.PathLike[str] | None = None) -> str | None:
    """Path of the ``grafed-gui`` executable, or ``None`` when not found.

    The search order is ``path`` (when given, it must be executable), the
    ``MARTY_GRAFED`` environment variable, the PATH, then the install prefix.
    """
    found = _core.visualization.find_grafed(os.fspath(path) if path else "")
    return found or None


def is_available() -> bool:
    """True if GRAFED can be found; see ``find``."""
    return find() is not None


def show(amplitude: Amplitude, *, wait: bool = False) -> int:
    """Opens the diagrams of an amplitude in GRAFED; see ``visualization.show``."""
    if not isinstance(amplitude, Amplitude):
        raise MartyTypeError(
            f"show takes a calculation.Amplitude, not {type(amplitude).__name__}"
        )
    executable = find()
    if executable is None:
        raise MartyStateError(
            "GRAFED not found: install grafed-gui, add it to the PATH, "
            "or set MARTY_GRAFED."
        )
    directory = tempfile.mkdtemp(prefix="pymarty-grafed-")
    path = os.path.join(directory, "diagrams.json")
    try:
        _core.visualization.save_diagrams(amplitude._native, path)
        pid = _core.visualization.launch_grafed(executable, path)
        if not wait:
            return pid
        return _core.visualization.wait_process(pid)
    finally:
        if wait:
            shutil.rmtree(directory, ignore_errors=True)
