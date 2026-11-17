import pytest

import pymarty  # noqa: F401  (import check: the package must load)
from pymarty._pymarty import _debug


@pytest.mark.unit
def test_cout_is_redirected_to_sys_stdout(capsys: pytest.CaptureFixture[str]) -> None:
    _debug.print_cout("hello from C++")
    assert capsys.readouterr().out == "hello from C++\n"


@pytest.mark.unit
def test_consecutive_calls_each_reach_sys_stdout(
    capsys: pytest.CaptureFixture[str],
) -> None:
    _debug.print_cout("first")
    _debug.print_cout("second")
    assert capsys.readouterr().out == "first\nsecond\n"
