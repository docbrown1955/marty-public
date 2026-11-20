"""Python counterparts of the C++ ``GroupTheoryApi`` tests, and Python-only checks."""

from collections.abc import Mapping
from fractions import Fraction

import pytest

from pymarty.errors import MartyValueError
from pymarty.group_theory import E6, SO, SU, U1, Decomposition, Irrep


@pytest.mark.parity("GroupTheoryApi.SU3Decompositions")
def test_su3_decompositions() -> None:
    su3 = SU(3)
    fund = su3.fundamental()
    adj = su3.adjoint()
    assert fund * ~fund == {su3.trivial(): 1, adj: 1}
    assert fund * fund == {su3.irrep(0, 1): 1, su3.irrep(2, 0): 1}
    assert adj * adj == {
        su3.trivial(): 1,
        adj: 2,
        su3.irrep(3, 0): 1,
        su3.irrep(0, 3): 1,
        su3.irrep(2, 2): 1,
    }
    assert sum(r.dim * m for r, m in (adj * adj).items()) == 64


@pytest.mark.parity("GroupTheoryApi.OtherGroupDecompositions")
def test_other_group_decompositions() -> None:
    f5 = SU(5).fundamental()
    assert f5 * ~f5 == {SU(5).trivial(): 1, SU(5).irrep(1, 0, 0, 1): 1}

    spinor = SO(10).irrep(0, 0, 0, 0, 1)
    assert spinor.dim == 16
    prod = spinor * spinor
    assert prod.dim == 256
    dims: dict[int, int] = {}
    for r, m in prod.items():
        dims[r.dim] = dims.get(r.dim, 0) + m
    assert dims == {10: 1, 120: 1, 126: 1}


@pytest.mark.parity("GroupTheoryApi.CasimirAndDynkinIndexUseCoreNormalisation")
def test_casimir_and_dynkin_index_use_core_normalisation() -> None:
    fund = SU(3).fundamental()
    adj = SU(3).adjoint()
    assert fund.casimir == Fraction(4, 9)
    assert adj.casimir == 1
    assert fund.dynkin_index == Fraction(1, 6)
    assert adj.dynkin_index == 1


@pytest.mark.parity("GroupTheoryApi.DynkinIndexSumRule")
def test_dynkin_index_sum_rule() -> None:
    fund = SU(3).fundamental()
    adj = SU(3).adjoint()
    total = sum(m * r.dynkin_index for r, m in (fund * adj).items())
    assert total == fund.dim * adj.dynkin_index + adj.dim * fund.dynkin_index


@pytest.mark.parity("GroupTheoryApi.GroupsAreCachedAndIrrepsCompareWithinAGroup")
def test_groups_are_cached_and_irreps_compare_within_a_group() -> None:
    assert SU(3) == SU(3)
    assert SU(3) != SU(5)
    fund = SU(3).fundamental()
    assert ~~fund == fund
    assert SU(3).adjoint().is_self_conjugate
    assert not fund.is_self_conjugate
    with pytest.raises(TypeError):
        _ = fund == SU(5).fundamental()


@pytest.mark.parity("GroupTheoryApi.InvalidGroupAndLabelsAreRejected")
def test_invalid_group_and_labels_are_rejected() -> None:
    with pytest.raises(MartyValueError):
        SU(1)
    with pytest.raises(MartyValueError):
        SO(3)
    with pytest.raises(MartyValueError):
        SU(3).irrep(1)
    with pytest.raises(MartyValueError):
        SU(3).irrep(1, -1)


@pytest.mark.parity("GroupTheoryApi.U1ChargesAddExactly")
def test_u1_charges_add_exactly() -> None:
    u1 = U1()
    q = u1.irrep(Fraction(1, 3))
    assert q.charge == Fraction(1, 3)
    assert list((q * q).items()) == [(u1.irrep(Fraction(2, 3)), 1)]


def test_irreps_are_hashable_and_usable_as_dict_keys() -> None:
    su3 = SU(3)
    table = {su3.fundamental(): "3", su3.irrep(0, 1): "3bar"}
    assert table[su3.irrep(1, 0)] == "3"
    assert {su3.irrep(1, 0), su3.fundamental()} == {su3.fundamental()}


def test_decomposition_is_a_read_only_mapping() -> None:
    su3 = SU(3)
    prod = su3.fundamental() * su3.fundamental()
    assert isinstance(prod, Mapping)
    assert isinstance(prod, Decomposition)
    assert su3.irrep(2, 0) in prod
    assert prod[su3.irrep(2, 0)] == 1
    assert len(prod) == 2


def test_irrep_repr_and_type() -> None:
    assert repr(SU(3).fundamental()) == "SU(3)[1,0]"
    assert isinstance(SU(3).fundamental(), Irrep)
    assert E6().fundamental().dim == 27
