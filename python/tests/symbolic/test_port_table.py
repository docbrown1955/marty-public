"""Consistency checks for the WP5b port table of the C++ CSL unit tests."""

from __future__ import annotations

import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[3]
TABLE_PATH = REPO_ROOT / "docs/architecture/pymarty/phase-1/port-table.md"
CPP_DIR = REPO_ROOT / "tests/units/csl"
PY_DIR = Path(__file__).resolve().parent

# Verified totals of the WP5b pass; update together with port-table.md.
EXPECTED_ROWS = 133
EXPECTED_CLASSES = {"P": 54, "P+": 55, "L1": 13, "X": 11}
EXPECTED_STATUSES = {"ported": 61, "gap": 5, "deferred": 44, "covered": 10, "n/a": 13}
EXPECTED_FILES = {
    "test_equality.cpp": 5,
    "test_literals.cpp": 2,
    "test_numbers.cpp": 19,
    "test_latex.cpp": 63,
    "test_relabel_indices.cpp": 25,
    "test_signed_refresh.cpp": 5,
    "test_metric_delta_invariant.cpp": 7,
    "test_init_sanitizer.cpp": 7,
}
CRITERION_2_GATE = 0.90

ROW_RE = re.compile(
    r"^\| (\d+) \| (test_\w+\.cpp) \| (\w+)\.(\w+) \| (P\+?|L1|X) "
    r"\| ([^|]+?) \| ([^|]+?) \|$"
)
TEST_RE = re.compile(r"^TEST\(\s*(\w+)\s*,\s*(\w+)\s*\)", re.MULTILINE)
DEST_RE = re.compile(r"(\w+\.py)::(\w+)")
STATUS_KINDS = (
    ("ported", "ported"),
    ("gap", "gap"),
    ("deferred", "deferred"),
    ("covered by L1 only", "covered"),
    ("n/a", "n/a"),
)


@dataclass(frozen=True)
class Row:
    number: int
    cpp_file: str
    suite: str
    name: str
    cls: str
    status: str
    destination: str

    @property
    def status_kind(self) -> str:
        for prefix, kind in STATUS_KINDS:
            if self.status.startswith(prefix):
                return kind
        return "unknown"


def _load_rows() -> list[Row]:
    rows: list[Row] = []
    for line in TABLE_PATH.read_text(encoding="utf-8").splitlines():
        match = ROW_RE.match(line)
        if match is None:
            continue
        number, cpp_file, suite, name, cls, status, destination = match.groups()
        rows.append(Row(int(number), cpp_file, suite, name, cls, status, destination))
    return rows


def _cpp_tests() -> dict[str, set[tuple[str, str]]]:
    return {
        path.name: set(TEST_RE.findall(path.read_text(encoding="utf-8")))
        for path in CPP_DIR.glob("test_*.cpp")
    }


@pytest.fixture(scope="module")
def rows() -> list[Row]:
    return _load_rows()


@pytest.mark.unit
def test_table_lists_every_candidate_row_in_order(rows: list[Row]) -> None:
    assert [row.number for row in rows] == list(range(1, EXPECTED_ROWS + 1))


@pytest.mark.unit
def test_every_row_names_an_existing_cpp_test(rows: list[Row]) -> None:
    tests = _cpp_tests()
    missing = [
        f"{row.cpp_file} {row.suite}.{row.name}"
        for row in rows
        if (row.suite, row.name) not in tests.get(row.cpp_file, set())
    ]
    assert missing == []


@pytest.mark.unit
def test_per_file_counts_match_the_verified_totals(rows: list[Row]) -> None:
    assert Counter(row.cpp_file for row in rows) == Counter(EXPECTED_FILES)


@pytest.mark.unit
def test_class_and_status_totals_match_the_verified_counts(rows: list[Row]) -> None:
    assert Counter(row.cls for row in rows) == Counter(EXPECTED_CLASSES)
    assert Counter(row.status_kind for row in rows) == Counter(EXPECTED_STATUSES)


@pytest.mark.unit
def test_ported_rows_name_an_existing_python_test(rows: list[Row]) -> None:
    missing: list[str] = []
    for row in rows:
        if row.status_kind != "ported":
            continue
        references = DEST_RE.findall(row.destination)
        if not references:
            missing.append(f"row {row.number}: no Python destination")
        for file_name, func in references:
            path = PY_DIR / file_name
            text = path.read_text(encoding="utf-8") if path.exists() else ""
            if not re.search(rf"^def {func}\(", text, re.MULTILINE):
                missing.append(f"row {row.number}: {file_name}::{func}")
    assert missing == []


@pytest.mark.unit
def test_criterion_2_ratio_reaches_the_gate(rows: list[Row]) -> None:
    ported = sum(1 for row in rows if row.status_kind == "ported")
    gap = sum(1 for row in rows if row.status_kind == "gap")
    assert ported / (ported + gap) >= CRITERION_2_GATE
