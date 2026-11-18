"""Python interface to MARTY: symbolic expressions, configuration and errors.

Submodules:

- ``symbolic``: expressions, symbols, indices, spaces and tensors.
- ``config``: run-time discovery of MARTY paths and programs.
- ``errors``: the exception hierarchy, rooted at ``MartyError``.

The package is built from source; see the README for the build steps.
"""

from pymarty import config, errors
from pymarty._pymarty import __version__

__all__ = ["__version__", "config", "errors"]
