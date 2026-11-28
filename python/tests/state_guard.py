"""The ``clean_state`` fixture: a test must leave the global C++ state intact."""

from __future__ import annotations

from collections.abc import Iterator

import pytest

from pymarty._pymarty import _debug


@pytest.fixture(autouse=True)
def clean_state() -> Iterator[None]:
    before = _debug.state_snapshot()
    yield
    after = _debug.state_snapshot()
    if after != before:
        # Restore first so that one leak does not fail every following test.
        _debug.restore_state(before)
        changed = sorted(key for key in before if before[key] != after[key])
        pytest.fail(f"test leaked global state: {', '.join(changed)}", pytrace=False)
