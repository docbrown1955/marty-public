"""Reading of LesHouches Accord (LHA) input files."""

from __future__ import annotations

import os

from pymarty._pymarty import simulation as _simulation


def read_lha(path: str | os.PathLike[str]) -> dict[str, dict[tuple[int, ...], float]]:
    """Read a LesHouches file and return its values by block.

    The result maps each block name to a dictionary from the entry indices to
    the value. Single-valued entries use a one-element index tuple, matrix
    entries a two-element one.

    >>> import tempfile, pathlib
    >>> from pymarty.simulation import read_lha
    >>> text = "BLOCK MASS\\n  1  2.5 # m\\n"
    >>> with tempfile.TemporaryDirectory() as d:
    ...     p = pathlib.Path(d) / "in.lha"
    ...     _ = p.write_text(text)
    ...     read_lha(p)
    {'MASS': {(1,): 2.5}}

    Raises:
        MartyLookupError: If the file cannot be opened.
    """
    blocks: dict[str, dict[tuple[int, ...], float]] = {}
    for block, indices, value in _simulation.read_lha(os.fspath(path)):
        blocks.setdefault(block, {})[tuple(indices)] = value
    return blocks
