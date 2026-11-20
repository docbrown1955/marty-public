"""Python interface to MARTY: symbolic expressions, configuration and errors.

Submodules:

- ``symbolic``: expressions, symbols, indices, spaces and tensors.
- ``diracology``: gamma matrices, traces and Fierz identities.
- ``config``: run-time discovery of MARTY paths and programs.
- ``group_theory``: simple groups, irreps, tensor products and Casimirs.
- ``errors``: the exception hierarchy, rooted at ``MartyError``.

The package is built from source; see the README for the build steps.
"""

from pymarty import config, diracology, errors, group_theory
from pymarty._pymarty import __version__

__all__ = ["__version__", "config", "diracology", "errors", "group_theory"]
