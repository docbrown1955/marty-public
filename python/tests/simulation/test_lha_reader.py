"""Tests of the LesHouches file reader."""

from __future__ import annotations

import pytest

from pymarty.errors import MartyLookupError
from pymarty.simulation import read_lha

LHA_TEXT = """\
BLOCK STANDARD
  0  1.0 # first
  1  2.5 # second
BLOCK SINGLE
  0  5.67e-3
BLOCK MATRIX
  0 0  1.0
  0 1  2.0
  1 0  3.0e5
  1 1  4.0
"""


def test_read_lha_returns_values_by_block_and_index(tmp_path) -> None:
    path = tmp_path / "input.lha"
    path.write_text(LHA_TEXT)

    blocks = read_lha(path)

    assert blocks["STANDARD"] == {(0,): 1.0, (1,): 2.5}
    assert blocks["SINGLE"] == {(0,): pytest.approx(5.67e-3)}
    assert blocks["MATRIX"] == {
        (0, 0): 1.0,
        (0, 1): 2.0,
        (1, 0): pytest.approx(3.0e5),
        (1, 1): 4.0,
    }


def test_read_lha_missing_file_raises_lookup_error(tmp_path) -> None:
    with pytest.raises(MartyLookupError):
        read_lha(tmp_path / "absent.lha")
