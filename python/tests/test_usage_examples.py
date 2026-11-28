"""Runs the usage examples of ``python/examples/usage``.

The examples are written for people (see the phase documents): they are
reviewed before the implementation of their module starts, and they are the
agreed Python API. Here they are only run, to keep them true.
"""

import pkgutil
import runpy
from pathlib import Path

import pytest

import pymarty

EXAMPLES = Path(__file__).resolve().parents[1] / "examples" / "usage"

# Modules that need an example once they exist in the package.
MODULES_WITH_EXAMPLE = [
    "errors",
    "symbolic",
    "group_theory",
    "diracology",
    "modeling",
    "calculation",
    "simulation",
]


def installed_modules() -> set[str]:
    return {info.name for info in pkgutil.iter_modules(pymarty.__path__)}


@pytest.mark.usage_example
@pytest.mark.parametrize("example", sorted(EXAMPLES.glob("*.py")), ids=lambda p: p.stem)
def test_usage_example_runs(example: Path, capsys: pytest.CaptureFixture[str]) -> None:
    if example.stem not in installed_modules():
        pytest.skip(f"pymarty.{example.stem} is not implemented yet")
    runpy.run_path(str(example), run_name="__main__")
    assert capsys.readouterr().out.strip(), "an example must show something"


@pytest.mark.usage_example
def test_every_module_has_a_usage_example() -> None:
    missing = [
        name
        for name in MODULES_WITH_EXAMPLE
        if name in installed_modules() and not (EXAMPLES / f"{name}.py").is_file()
    ]
    assert not missing, f"modules without python/examples/usage/<module>.py: {missing}"
