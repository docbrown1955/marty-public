"""Check that C++ API tests and Python tests stay in parity.

Rules (see docs/architecture/pymarty/testing-strategy.md, section 2):

* every ``TEST(<Module>Api, <Behaviour>)`` in ``tests/api/**/*.cpp`` (plus the
  files listed in ``python/tests/parity_sources.txt``) has a Python test marked
  ``@pytest.mark.parity("<Module>Api.<Behaviour>")``, or an exemption in
  ``python/tests/parity_exemptions.txt``;
* every parity marker references an existing C++ test;
* every exemption matches at least one C++ test and gives a reason.

Exemption lines have the form ``<Module>Api.<Behaviour> | <reason>``; the id
may contain ``*`` wildcards. Lines starting with ``#`` are comments.
"""

from __future__ import annotations

import argparse
import fnmatch
import re
import sys
from pathlib import Path

PYTHON_DIR = Path(__file__).resolve().parent.parent
REPO_ROOT = PYTHON_DIR.parent

API_TEST = re.compile(r"^\s*TEST(?:_F)?\(\s*(\w+Api)\s*,\s*(\w+)\s*\)", re.MULTILINE)
ANY_TEST = re.compile(r"^\s*TEST(?:_F)?\(\s*(\w+)\s*,\s*(\w+)\s*\)", re.MULTILINE)
PARITY_MARKER = re.compile(r"""pytest\.mark\.parity\(\s*["']([^"']+)["']\s*\)""")


def read_list(path: Path) -> list[str]:
    """Non-empty, non-comment lines of a text file (empty if it is missing)."""
    if not path.exists():
        return []
    lines = (line.strip() for line in path.read_text().splitlines())
    return [line for line in lines if line and not line.startswith("#")]


def cpp_tests(root: Path, extra_sources: list[str]) -> set[str]:
    found: set[str] = set()
    for source in sorted((root / "tests" / "api").rglob("*.cpp")):
        found.update(f"{s}.{n}" for s, n in API_TEST.findall(source.read_text()))
    for relative in extra_sources:
        source = root / relative
        if source.exists():
            found.update(f"{s}.{n}" for s, n in ANY_TEST.findall(source.read_text()))
    return found


def python_markers(tests_dir: Path) -> dict[str, list[Path]]:
    markers: dict[str, list[Path]] = {}
    for source in sorted(tests_dir.rglob("*.py")):
        for ident in PARITY_MARKER.findall(source.read_text()):
            markers.setdefault(ident, []).append(source)
    return markers


def parse_exemptions(lines: list[str]) -> tuple[dict[str, str], list[str]]:
    exemptions: dict[str, str] = {}
    problems: list[str] = []
    for line in lines:
        ident, sep, reason = (part.strip() for part in line.partition("|"))
        if not sep or not reason:
            problems.append(f"exemption without a reason: {line!r}")
            continue
        exemptions[ident] = reason
    return exemptions, problems


def check(root: Path, tests_dir: Path) -> tuple[list[str], str]:
    extra = read_list(tests_dir / "parity_sources.txt")
    cpp = cpp_tests(root, extra)
    markers = python_markers(tests_dir)
    exemptions, problems = parse_exemptions(
        read_list(tests_dir / "parity_exemptions.txt")
    )

    exempt_ids: set[str] = set()
    for pattern in exemptions:
        matched = {ident for ident in cpp if fnmatch.fnmatchcase(ident, pattern)}
        if not matched:
            problems.append(f"exemption matches no C++ test: {pattern}")
        exempt_ids |= matched

    for ident in sorted(cpp - set(markers) - exempt_ids):
        problems.append(f"C++ test without Python counterpart or exemption: {ident}")
    for ident in sorted(set(markers) - cpp):
        files = ", ".join(p.name for p in markers[ident])
        problems.append(f"parity marker references no C++ test: {ident} ({files})")

    matched_count = len(cpp & set(markers))
    summary = (
        f"{len(cpp)} C++ API tests: {matched_count} with a Python counterpart, "
        f"{len(exempt_ids - set(markers))} exempted"
    )
    return problems, summary


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", type=Path, default=REPO_ROOT)
    parser.add_argument("--tests", type=Path, default=PYTHON_DIR / "tests")
    args = parser.parse_args(argv)

    problems, summary = check(args.root, args.tests)
    for problem in problems:
        print(f"parity: {problem}", file=sys.stderr)
    if problems:
        print(f"parity: FAILED ({len(problems)} problems)", file=sys.stderr)
        return 1
    print(f"parity: OK, {summary}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
