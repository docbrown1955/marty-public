"""Run the doctest examples embedded in the documentation of the Python API."""

import doctest
import inspect
import types
from collections.abc import Iterator
from typing import Any

import pytest

from pymarty import config, errors, symbolic


def _documented_objects() -> Iterator[tuple[str, Any]]:
    """Yield (name, object) pairs whose docstring contains a doctest example."""
    seen: set[int] = set()
    for module in (symbolic, errors, config):
        yield module.__name__, module
        for name, obj in vars(module).items():
            if name.startswith("_") or id(obj) in seen:
                continue
            seen.add(id(obj))
            yield f"{module.__name__}.{name}", obj
            if inspect.isclass(obj):
                for attr_name, attr in vars(obj).items():
                    if attr_name.startswith("_") and attr_name != "__init__":
                        continue
                    target = attr.fget if isinstance(attr, property) else attr
                    if isinstance(
                        target, (types.FunctionType, types.BuiltinFunctionType)
                    ) or callable(target):
                        yield f"{module.__name__}.{name}.{attr_name}", target


def _examples() -> list[tuple[str, Any]]:
    examples = []
    for name, obj in _documented_objects():
        doc = getattr(obj, "__doc__", None)
        if isinstance(doc, str) and ">>>" in doc:
            examples.append((name, obj))
    return examples


_EXAMPLES = _examples()


@pytest.mark.unit
def test_documented_examples_exist() -> None:
    assert len(_EXAMPLES) >= 40


@pytest.mark.unit
@pytest.mark.parametrize(("name", "obj"), _EXAMPLES, ids=[n for n, _ in _EXAMPLES])
def test_docstring_examples_pass(name: str, obj: Any) -> None:
    globs: dict[str, Any] = {"symbolic": symbolic}
    globs.update({n: getattr(symbolic, n) for n in symbolic.__all__})
    globs.update({"errors": errors, "config": config})
    parser = doctest.DocTestParser()
    test = parser.get_doctest(obj.__doc__ or "", globs, name, None, 0)
    runner = doctest.DocTestRunner(verbose=False)
    runner.run(test, clear_globs=True)
    assert runner.failures == 0, f"doctest failures in {name}"
