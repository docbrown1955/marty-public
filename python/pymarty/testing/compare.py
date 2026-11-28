"""Comparison of numerical result files, ported from tests/system/src/comparedata.h.

A result file holds one data line per point, each made of whitespace-separated
numbers. A value is compared to the reference value with the relative
difference ``|ref - res| / ref``, and the denominator is 1 when the reference
value is below ``1e-10``, as in the C++ header.
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from pymarty.errors import MartyValueError

_SMALL = 1e-10


class ComparisonError(MartyValueError):
    """The files cannot be compared: different line or value counts, or a bad value."""


@dataclass(frozen=True)
class Comparison:
    """Average and maximum relative differences between two result files.

    ``average`` is the mean over the data lines of the mean difference of each
    line; ``maximum`` is the largest difference of any value. ``passed`` tells
    whether the maximum is below ``tolerance``, as ``assert_equal`` does in C++.
    """

    average: float
    maximum: float
    tolerance: float

    @property
    def passed(self) -> bool:
        """True when every value agrees within the tolerance."""
        return self.maximum < self.tolerance


def _relative_difference(reference: float, result: float) -> float:
    denominator = 1.0 if reference < _SMALL else reference
    return abs(reference - result) / denominator


def _read_rows(path: Path) -> list[list[float]]:
    if not path.is_file():
        raise FileNotFoundError(f'result file "{path}" not found')
    rows: list[list[float]] = []
    with path.open(encoding="utf-8") as stream:
        for number, line in enumerate(stream, start=1):
            try:
                rows.append([float(token) for token in line.split()])
            except ValueError as error:
                raise ComparisonError(
                    f'unparsable value in "{path}" at line {number}: {line.strip()!r}'
                ) from error
    return rows


def compare(reference: str | Path, result: str | Path, tolerance: float) -> Comparison:
    """Compare a result file against its reference, value by value.

    Raises:
        FileNotFoundError: if either file is missing.
        ComparisonError: if the files have a different number of lines, a line
            has a different number of values, or a value cannot be parsed.
    """
    ref_rows = _read_rows(Path(reference))
    res_rows = _read_rows(Path(result))
    if len(ref_rows) != len(res_rows):
        raise ComparisonError(
            f"size mismatch: {len(ref_rows)} lines in {reference} "
            f"but {len(res_rows)} lines in {result}"
        )

    line_averages: list[float] = []
    maximum = 0.0
    for number, (ref_row, res_row) in enumerate(
        zip(ref_rows, res_rows, strict=True), start=1
    ):
        if len(ref_row) != len(res_row):
            raise ComparisonError(
                f"size mismatch at line {number}: {len(ref_row)} values in "
                f"{reference} but {len(res_row)} values in {result}"
            )
        if not ref_row:
            continue
        differences = [
            _relative_difference(ref, res)
            for ref, res in zip(ref_row, res_row, strict=True)
        ]
        line_averages.append(sum(differences) / len(differences))
        maximum = max(maximum, max(differences))

    average = sum(line_averages) / len(line_averages) if line_averages else 0.0
    return Comparison(average=average, maximum=maximum, tolerance=tolerance)
