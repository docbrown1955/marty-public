"""Tests of pymarty.testing.compare, the port of the comparedata.h rule."""

import pytest

from pymarty.testing import Comparison, ComparisonError, compare


def _write(path, text: str):
    path.write_text(text)
    return path


def test_identical_files_agree_exactly(tmp_path) -> None:
    ref = _write(tmp_path / "ref.txt", "1 2\n3 4\n")
    res = _write(tmp_path / "res.txt", "1 2\n3 4\n")
    result = compare(ref, res, 1e-5)
    assert isinstance(result, Comparison)
    assert result.maximum == 0.0
    assert result.passed


def test_relative_differences_give_average_and_maximum(tmp_path) -> None:
    ref = _write(tmp_path / "ref.txt", "2.0 4.0\n")
    res = _write(tmp_path / "res.txt", "2.2 4.0\n")
    result = compare(ref, res, 0.05)
    assert result.maximum == pytest.approx(0.1)
    assert result.average == pytest.approx(0.05)
    assert not result.passed


def test_tiny_reference_values_use_absolute_difference(tmp_path) -> None:
    ref = _write(tmp_path / "ref.txt", "0.0\n")
    res = _write(tmp_path / "res.txt", "3e-6\n")
    result = compare(ref, res, 1e-5)
    assert result.maximum == pytest.approx(3e-6)
    assert result.passed


def test_size_mismatch_is_an_error(tmp_path) -> None:
    ref = _write(tmp_path / "ref.txt", "1 2\n3 4\n")
    res = _write(tmp_path / "res.txt", "1 2\n")
    with pytest.raises(ComparisonError):
        compare(ref, res, 1e-5)


def test_missing_result_file_is_an_error(tmp_path) -> None:
    ref = _write(tmp_path / "ref.txt", "1\n")
    with pytest.raises(FileNotFoundError):
        compare(ref, tmp_path / "absent.txt", 1e-5)
