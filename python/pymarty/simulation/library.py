"""Generate, build and load standalone MARTY C libraries.

The C++ layer (``pymarty._pymarty.simulation``) writes the sources and compiles
them. This module adds the path and configuration handling, and loads the
ctypes binding that the generator writes next to the sources.
"""

from __future__ import annotations

import importlib.util
import os
from collections.abc import Mapping
from pathlib import Path
from types import ModuleType

from pymarty import config
from pymarty._pymarty import Expr
from pymarty._pymarty import simulation as _simulation
from pymarty.errors import MartyStateError

__all__ = ["Library", "generate"]


class Library:
    """A generated C library: its sources, its shared object, and its binding.

    Created by :func:`generate`. Call :meth:`build` to compile it, then
    :meth:`load` to import the ctypes binding.
    """

    def __init__(self, name: str, generator: _simulation.Generator) -> None:
        self._name = name
        self._generator = generator
        self._built = False
        self._module: ModuleType | None = None

    @property
    def name(self) -> str:
        """Name of the library, used for the file names."""
        return self._name

    @property
    def directory(self) -> Path:
        """Directory holding the generated sources and build tree."""
        return Path(self._generator.library_directory)

    @property
    def shared_object(self) -> Path:
        """Path of the compiled shared library ``lib<name>.so``."""
        return self.directory / "build" / "lib" / f"lib{self._name}.so"

    def build(self, n_jobs: int = 1) -> None:
        """Compile the library with the ``make`` found by :mod:`pymarty.config`."""
        make = config.resolve().make
        self._generator.build(n_jobs, make)
        self._built = True
        self._module = None

    def load(self) -> ModuleType:
        """Import the generated ctypes binding of the built library.

        Raises:
            MartyStateError: if the library has not been built yet.
        """
        if not self._built:
            raise MartyStateError(
                f"library {self._name!r} is not built; call build() first"
            )
        if self._module is None:
            binding = self.directory / "src" / "py" / f"{self._name}.py"
            spec = importlib.util.spec_from_file_location(self._name, binding)
            if spec is None or spec.loader is None:
                raise MartyStateError(f"no Python binding found at {binding}")
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            self._module = module
        return self._module


def generate(
    name: str,
    functions: Mapping[str, Expr],
    *,
    path: str | Path,
    include_path: str | None = None,
) -> Library:
    """Write the sources of a library computing ``functions``.

    Each entry of ``functions`` maps a function name to its expression. The
    sources go in ``<path>/<name>``. Nothing is compiled until
    :meth:`Library.build`.

    Args:
        name: C identifier used for the library and its functions.
        functions: Function name to expression, built with ``pymarty.symbolic``.
        path: Directory that receives the library directory.
        include_path: MARTY include directory. Defaults to the value resolved
            by :func:`pymarty.config.resolve`.
    """
    generator = _simulation.Generator(name, str(Path(path)))
    resolved = config.resolve(include_path=include_path)
    generator.add_include_path(resolved.include_path)
    for library_dir in filter(None, resolved.library_path.split(os.pathsep)):
        generator.add_library_path(library_dir)
    for function_name, expr in functions.items():
        generator.add_function(function_name, expr)
    generator.print_sources()
    return Library(name, generator)
