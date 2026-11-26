"""Generation, compilation and calls of standalone MARTY libraries.

- ``generate(name, functions, path=...)`` writes the C sources of a library.
- ``Library.build()`` compiles it, and ``Library.load()`` imports its binding.
"""

from pymarty.simulation.library import Library, generate

__all__ = ["Library", "generate"]
