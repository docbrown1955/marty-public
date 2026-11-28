"""Run-time discovery of MARTY include/library paths and programs.

Resolution order for every item: explicit argument, environment
(``MARTY_INCLUDE_PATH``, ``MARTY_LIBRARY_PATH``, ``MARTY_CXX`` then ``CXX``,
``MARTY_GRAFED``), package data (``pymarty/_data``), system locations. Programs
are looked up by scanning ``PATH``; no shell is involved.
"""

from __future__ import annotations

from pathlib import Path

from pymarty._pymarty import Config

__all__ = ["Config", "resolve"]

_PACKAGE_DATA = Path(__file__).resolve().parent / "_data"


def resolve(
    *,
    include_path: str | None = None,
    library_path: str | None = None,
    cxx: str | None = None,
    make: str | None = None,
    grafed: str | None = None,
) -> Config:
    """Resolve the configuration now, from the current environment.

    Items that cannot be found raise :class:`pymarty.errors.MartyBuildError` when they
    are read, not here.
    """
    return Config(
        include_path=include_path or "",
        library_path=library_path or "",
        cxx=cxx or "",
        make=make or "",
        grafed=grafed or "",
        package_data=str(_PACKAGE_DATA),
    )
