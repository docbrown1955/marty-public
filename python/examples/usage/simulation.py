"""Usage example: standalone C libraries with ``pymarty.simulation``.

Writes the C sources of a library from symbolic expressions, compiles it,
imports its ctypes binding, and calls the functions with plain keyword
arguments.

Run it with ``python simulation.py``. It is also run by the test suite
(marked ``needs_compiler`` and ``slow``).
"""

import tempfile

from pymarty import simulation
from pymarty.symbolic import constant

# 1. Describe the functions. Each name maps to a symbolic expression of the
#    parameters; nothing is compiled yet.
x = constant("x")
y = constant("y")
functions = {
    "f": x * y + 2 * x,  # the function f(x, y)
    "g": x * x * y,  # a second function of the same parameters
}

with tempfile.TemporaryDirectory() as work:
    # 2. Write the sources in <work>/toy and get a Library describing them.
    lib = simulation.generate("toy", functions, path=work)

    # 3. Compile it. build() runs make on the generated Makefile.
    lib.build(n_jobs=4)

    # 4. Import the compiled binding and call the functions.
    toy = lib.load()
    print("f(2, 3) =", toy.f(x=2.0, y=3.0))  # 2*3 + 2*2 = 10
    print("g(2, 3) =", toy.g(x=2.0, y=3.0))  # 2*2*3 = 12

    # The generated binding is standalone and raises ValueError.
    try:
        toy.f(x=2.0)
    except ValueError as error:
        print(f"caught: {error}")
