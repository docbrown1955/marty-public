"""Check that every public method of the C++ modeling facade has a binding.

Criterion 2 of docs/architecture/pymarty/phase-4/03-spec.md: each public method
of the facade in src/marty/api/modeling/ needs a documented Python spelling.
The C++ names are read from the headers, and each one is mapped to its snake
case Python name, or to the alias in ALIASES. Run it from the repository root
with the build tree on PYTHONPATH::

    PYTHONPATH=<build>/python <venv>/bin/python python/tools/check_modeling_bindings.py

The script exits with status 1 if a method has no binding or no docstring.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path
from typing import Any, Final

from pymarty._pymarty import modeling as core

FACADE_DIR: Final = (
    Path(__file__).resolve().parents[2] / "src" / "marty" / "api" / "modeling"
)

# C++ class -> (header, Python object that carries the bindings).
CLASSES: Final[dict[str, tuple[str, Any]]] = {
    "Particle": ("modeling.h", core.Particle),
    "Model": ("modeling.h", core.Model),
}

# Per class: C++ method -> Python attribute that replaces it. An empty string
# means the method is internal (not part of the Python API) and needs no binding.
ALIASES: Final[dict[str, dict[str, str]]] = {
    "Particle": {
        "isU1": "set_rep",
        "setLabels": "set_rep",
        "setCharge": "set_rep",
        "fieldStrength": "field_strength",
        "createSimilarParticle": "generate_similar_particle",
        "ghostBoson": "ghost_boson",
        "raw": "",
    },
    "Model": {
        "contains": "__contains__",
        "describe": "__str__",
        "renameParticle": "rename_particle",
        "diracFermion": "add_dirac_fermion",
        "majoranaFermion": "add_majorana_fermion",
        "weylFermion": "add_weyl_fermion",
        "scalarBoson": "add_scalar_boson",
        "ghostBoson": "add_ghost_boson",
        "newCoupling": "add_coupling",
        "refresh": "",
        "standardModel": "",
        "qed": "",
        "feynmanRules": "",
        "computeAmplitude": "",
        "computeSquaredAmplitude": "",
    },
}

DECLARATION: Final = re.compile(
    r"^ {4}(?:static |explicit |virtual )*(?:[\w:<>&\*, ]+?[\s&\*])?(\w+)\("
)


def snake_case(name: str) -> str:
    return re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name).lower()


def public_methods(header: Path, class_name: str) -> list[str]:
    """Names of the public methods declared in the class body of the header."""
    text = header.read_text(encoding="utf-8")
    match = re.search(
        rf"^class {class_name}\b[^\n]*\{{(.*?)^\}};", text, re.MULTILINE | re.DOTALL
    )
    if match is None:
        raise SystemExit(f"class {class_name} not found in {header}")
    names: list[str] = []
    public = False
    for line in match.group(1).splitlines():
        stripped = line.strip()
        if stripped == "public:":
            public = True
        elif stripped in ("private:", "protected:"):
            public = False
        elif public:
            found = DECLARATION.match(line)
            if found and found.group(1) not in (class_name, "operator"):
                names.append(found.group(1))
    return names


def python_target(obj: Any, class_name: str, name: str) -> Any:
    """Binding of a C++ method, on the class or on the modeling module."""
    attribute = ALIASES[class_name].get(name, snake_case(name))
    if attribute == "":
        return None
    if hasattr(obj, attribute):
        return getattr(obj, attribute)
    if hasattr(core, attribute):
        return getattr(core, attribute)
    return None


def main() -> int:
    failures: list[str] = []
    for class_name, (header_name, obj) in CLASSES.items():
        for method in public_methods(FACADE_DIR / header_name, class_name):
            if ALIASES[class_name].get(method) == "":
                continue
            target = python_target(obj, class_name, method)
            label = f"{class_name}::{method}"
            if target is None:
                failures.append(f"{label}: no binding")
            elif not getattr(target, "__doc__", None):
                failures.append(f"{label}: binding has no docstring")
            else:
                print(f"ok  {label}")
    for failure in failures:
        print(f"FAIL {failure}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
