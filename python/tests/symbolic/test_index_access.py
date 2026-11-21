"""Index access accessors of Expr: index_count, index and index_structure."""

import pytest

from pymarty import symbolic


@pytest.mark.unit
def test_index_accessors_of_tensor_element() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    b = euclid.index("b")
    element = symbolic.tensor("T", [euclid, euclid])(a, b)
    assert element.index_count == 2
    assert element.index(0).name == "a"
    assert element.index(1).name == "b"
    assert [i.name for i in element.index_structure] == ["a", "b"]


@pytest.mark.unit
def test_index_of_tensor_element_out_of_range_raises_lookup_error() -> None:
    euclid = symbolic.Euclid_R3
    element = symbolic.tensor("T", [euclid])(euclid.index("a"))
    with pytest.raises(LookupError):
        element.index(1)


@pytest.mark.unit
def test_index_accessors_of_non_indicial_expressions() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    product = symbolic.variable("x") * symbolic.tensor("U", [euclid])(a)
    assert product.index_count == 0
    with pytest.raises(LookupError):
        product.index(0)
    assert [i.name for i in product.index_structure] == ["a"]
    assert symbolic.variable("x").index_structure == ()
