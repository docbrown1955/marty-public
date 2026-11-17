"""Exceptions raised by pymarty.

Every exception derives from :class:`MartyError`. The specific ones also derive
from the matching built-in exception, so code that only knows the built-ins
(``except ValueError``) keeps working.

>>> from pymarty.errors import MartyError, MartyValueError
>>> issubclass(MartyValueError, (MartyError, ValueError))
True
"""

from pymarty._pymarty import (
    MartyBuildError,
    MartyError,
    MartyLookupError,
    MartyNotImplementedError,
    MartyStateError,
    MartyTypeError,
    MartyValueError,
)

__all__ = [
    "MartyBuildError",
    "MartyError",
    "MartyLookupError",
    "MartyNotImplementedError",
    "MartyStateError",
    "MartyTypeError",
    "MartyValueError",
]
