"""Python interface to MARTY: symbolic expressions, configuration and errors.

Submodules:

- ``symbolic``: expressions, symbols, indices, spaces and tensors.
- ``diracology``: gamma matrices, traces and Fierz identities.
- ``config``: run-time discovery of MARTY paths and programs.
- ``group_theory``: simple groups, irreps, tensor products and Casimirs.
- ``modeling``: gauge groups, particles and symmetry breaking.
- ``calculation``: Feynman rules, amplitudes and squared amplitudes.
- ``visualization``: JSON export of diagrams and GRAFED availability.
- ``simulation``: generate, build and load standalone C libraries.
- ``errors``: the exception hierarchy, rooted at ``MartyError``.

The package is built from source; see the README for the build steps.
"""

import importlib
from types import ModuleType

from pymarty import config, diracology, errors, group_theory
from pymarty._pymarty import __version__

_LAZY_SUBMODULES = ("modeling", "calculation", "simulation", "visualization")

__all__ = [
    "__version__",
    "calculation",
    "config",
    "diracology",
    "errors",
    "group_theory",
    "modeling",
    "simulation",
    "visualization",
]


def __getattr__(name: str) -> ModuleType:
    """Load ``modeling`` on first access, keeping other imports light."""
    if name in _LAZY_SUBMODULES:
        return importlib.import_module(f"{__name__}.{name}")
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")
