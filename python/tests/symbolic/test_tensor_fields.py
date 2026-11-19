"""Complex property, tensor fields and tensorial derivatives of the symbolic API."""

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError, MartyValueError


@pytest.mark.unit
def test_complex_property_values() -> None:
    assert symbolic.ComplexProperty.Complex.value == 0
    assert symbolic.ComplexProperty.Real.value == 1
    assert symbolic.ComplexProperty.Imaginary.value == 2


@pytest.mark.unit
def test_constant_and_variable_accept_complex_property() -> None:
    a = symbolic.constant("a", complex_property=symbolic.ComplexProperty.Complex)
    x = symbolic.variable("x", complex_property=symbolic.ComplexProperty.Imaginary)
    assert str(a) == "a"
    assert str(x) == "x"


@pytest.mark.unit
def test_tensor_field_element_keeps_index_free() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    field = symbolic.tensor_field("F", euclid, [euclid])
    mu = euclid.index("mu")
    assert [i.name for i in field(mu, point).free_indices] == ["mu"]


@pytest.mark.unit
def test_scalar_tensor_field_element_has_no_free_index() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    field = symbolic.tensor_field("G", euclid)
    assert field(point).free_indices == ()


@pytest.mark.unit
def test_tensor_field_rejects_wrong_index_count() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    field = symbolic.tensor_field("F", euclid, [euclid])
    with pytest.raises(MartyValueError):
        field(point)
    with pytest.raises(MartyValueError):
        field(euclid.index("a"), euclid.index("b"), point)


@pytest.mark.unit
def test_tensor_field_last_argument_must_be_tensor() -> None:
    euclid = symbolic.Euclid_R3
    field = symbolic.tensor_field("F", euclid, [euclid])
    with pytest.raises(MartyTypeError):
        field(euclid.index("a"), euclid.index("b"))


@pytest.mark.unit
def test_tensor_field_rejects_foreign_point() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("Y", [symbolic.Euclid_R2])
    field = symbolic.tensor_field("F", euclid, [euclid])
    with pytest.raises(MartyValueError):
        field(euclid.index("a"), point)


@pytest.mark.unit
def test_tensor_derivative_of_point_tensor_is_delta() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    d = symbolic.tensor_derivative("d", euclid)
    mu = euclid.index("mu")
    nu = euclid.index("nu")
    expr = d(mu, point)(point(nu))
    assert expr == symbolic.delta(mu, nu)


@pytest.mark.unit
def test_tensor_derivative_of_scalar_is_zero() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    d = symbolic.tensor_derivative("d", euclid)
    mu = euclid.index("mu")
    assert d(mu, point)(symbolic.variable("x")) == 0


@pytest.mark.unit
def test_tensor_derivative_element_rejects_foreign_index() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    d = symbolic.tensor_derivative("d", euclid)
    with pytest.raises(MartyValueError):
        d(symbolic.Euclid_R2.index("a"), point)


@pytest.mark.unit
def test_tensor_derivative_element_rejects_foreign_point() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("Y", [symbolic.Euclid_R2])
    d = symbolic.tensor_derivative("d", euclid)
    with pytest.raises(MartyValueError):
        d(euclid.index("a"), point)


@pytest.mark.unit
def test_call_on_non_derivative_expression_is_type_error() -> None:
    euclid = symbolic.Euclid_R3
    point = symbolic.tensor("X", [euclid])
    field = symbolic.tensor_field("F", euclid, [euclid])
    mu = euclid.index("mu")
    with pytest.raises(MartyTypeError):
        field(mu, point)(point(euclid.index("nu")))


@pytest.mark.unit
def test_tensor_field_read_only_getters() -> None:
    point = symbolic.Space("Point", 3)
    index = symbolic.Space("Index", 2)
    field = symbolic.tensor_field("F", point, [index, point])
    assert field.name == "F"
    assert field.point_space.name == "Point"
    assert [s.name for s in field.index_spaces] == ["Index", "Point"]


@pytest.mark.unit
def test_tensor_field_primary_type_is_field() -> None:
    point = symbolic.Space("Point", 3)
    field = symbolic.tensor_field("F", point, [point])
    assert field.primary_type == symbolic.PrimaryType.Field
