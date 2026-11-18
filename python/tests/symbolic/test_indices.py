"""Spaces, indices and index names of the symbolic API."""

import gc

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError, MartyValueError
from pymarty.symbolic import Index, Space


def _index_of_temporary_space() -> Index:
    space = Space("Tmp", 3)
    return space.index("a")


@pytest.mark.unit
def test_index_outlives_scope() -> None:
    index = _index_of_temporary_space()
    gc.collect()
    assert index.name == "a"
    assert index.space is not None
    assert index.space.dimension == 3


@pytest.mark.unit
def test_unnamed_index_cycles_default_names() -> None:
    space = Space("Cycle", 3, ("i", "j"))
    first, second, third = space.index(), space.index(), space.index()
    assert [first.name, second.name, third.name] == ["i", "j", "i"]
    assert first != third


@pytest.mark.unit
def test_indices_from_list_are_distinct() -> None:
    indices = symbolic.Euclid_R3.indices(3, "a")
    assert len(indices) == 3
    assert all(index.name == "a" for index in indices)
    assert len(set(indices)) == 3


@pytest.mark.unit
def test_minkowski_is_one_shared_space() -> None:
    mu = symbolic.Minkowski.index("mu")
    assert mu.space is symbolic.Minkowski
    assert symbolic.Minkowski.dimension == 4


@pytest.mark.unit
def test_fixed_index_range() -> None:
    space = Space("Big", 200, ("a",))
    space.fixed_index(127)
    with pytest.raises(MartyValueError):
        space.fixed_index(128)
    with pytest.raises(MartyValueError):
        symbolic.Euclid_R3.fixed_index(3)
    with pytest.raises(MartyValueError):
        symbolic.Euclid_R3.fixed_index(-1)


@pytest.mark.unit
def test_name_and_value_type_checked() -> None:
    with pytest.raises(MartyTypeError):
        Space("X", 2.5)  # type: ignore[arg-type]
    with pytest.raises(MartyTypeError):
        Space("X", True)
    with pytest.raises(MartyTypeError):
        Space("X", 2, index_names="ij")
    with pytest.raises(MartyTypeError):
        Space("X", 2, index_names=[1])  # type: ignore[list-item]
    with pytest.raises(MartyTypeError):
        Space(3, 2)  # type: ignore[arg-type]
    with pytest.raises(MartyTypeError):
        Space("X", 2).index(name=1)  # type: ignore[arg-type]
    with pytest.raises(MartyTypeError):
        Space("X", 2).indices("2")  # type: ignore[arg-type]
    with pytest.raises(MartyTypeError):
        Space("X", 2).fixed_index("0")  # type: ignore[arg-type]


@pytest.mark.unit
def test_empty_index_names_raise() -> None:
    with pytest.raises(MartyValueError):
        Space("X", 2, ())


@pytest.mark.unit
def test_negative_index_count_raises() -> None:
    with pytest.raises(MartyValueError):
        symbolic.Euclid_R3.indices(-1)


@pytest.mark.unit
def test_index_hash_ignores_sign() -> None:
    mu = symbolic.Minkowski.index("mu")
    nu = mu.flipped()
    assert nu.name == "mu"
    assert mu == nu
    assert hash(mu) == hash(nu)


@pytest.mark.unit
def test_index_is_not_equal_to_non_index() -> None:
    mu = symbolic.Minkowski.index("mu")
    assert mu != 1
    assert mu != "mu"


@pytest.mark.unit
def test_index_space_mismatch_raises() -> None:
    a = symbolic.Euclid_R3.index("a")
    b = symbolic.Euclid_R2.index("b")
    with pytest.raises(MartyValueError):
        symbolic.delta(a, b)
    with pytest.raises(MartyValueError):
        symbolic.metric(a, b)
