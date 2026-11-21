"""Usage example: errors.

Everything pymarty raises lives in ``pymarty.errors`` and derives from
``MartyError``. The specific errors also derive from the matching built-in
exception, so code that only knows the built-ins keeps working.

Run it with ``python errors.py``. It is also run by the test suite.
"""

import sys

from pymarty import config
from pymarty.errors import (
    MartyBuildError,
    MartyError,
    MartyLookupError,
    MartyNotImplementedError,
    MartyStateError,
    MartyTypeError,
    MartyValueError,
)

# 1. One class to catch everything that comes from MARTY.
#    Asking for a compiler that does not exist is an error that needs no model.
try:
    _ = config.resolve(cxx="no-such-compiler").cxx
except MartyError as error:
    print(f"caught {type(error).__name__}: {error}")

# 2. Catch a specific class to react to a specific problem.
try:
    _ = config.resolve(cxx="no-such-compiler").cxx
except MartyBuildError:
    print("no usable C++ compiler: the generated libraries cannot be built")

# 3. The built-in exceptions work too (duck typing). A build error is a
#    RuntimeError, so generic code that handles RuntimeError needs no change.
try:
    _ = config.resolve(cxx="no-such-compiler").cxx
except RuntimeError as error:
    assert isinstance(error, MartyBuildError)
    print("also caught as a plain RuntimeError")

# 4. The hierarchy. Each line reads "<MARTY class> is also a <built-in>".
for marty_class, builtin in [
    (MartyValueError, ValueError),
    (MartyTypeError, TypeError),
    (MartyLookupError, LookupError),
    (MartyNotImplementedError, NotImplementedError),
    (MartyStateError, RuntimeError),
    (MartyBuildError, RuntimeError),
]:
    assert issubclass(marty_class, MartyError)
    assert issubclass(marty_class, builtin)
    print(f"{marty_class.__name__:<26} is also a {builtin.__name__}")

# 5. After an error the library is still usable: the next call works.
print("a compiler that exists resolves fine:", config.resolve(cxx=sys.executable).cxx)
