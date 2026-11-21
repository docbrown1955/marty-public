from pathlib import Path

import pytest
import state_guard

from pymarty._pymarty import _debug


@pytest.mark.unit
def test_snapshot_is_stable_and_restorable() -> None:
    before = _debug.state_snapshot()
    assert _debug.state_snapshot() == before
    _debug.leak_state()
    assert _debug.state_snapshot() != before
    _debug.restore_state(before)
    assert _debug.state_snapshot() == before


@pytest.mark.unit
def test_leaking_test_is_caught_by_clean_state(pytester: pytest.Pytester) -> None:
    pytester.makeconftest(Path(state_guard.__file__).read_text())
    pytester.makepyfile(
        """
        from pymarty._pymarty import _debug

        def test_well_behaved():
            pass

        def test_leaks_a_flag():
            _debug.leak_state()
        """
    )
    result = pytester.runpytest_subprocess("-p", "no:cacheprovider")
    result.assert_outcomes(passed=2, errors=1)
    result.stdout.fnmatch_lines(["*test leaked global state*amputateExternalLegs*"])
