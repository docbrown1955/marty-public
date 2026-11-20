"""Python counterparts of the C++ ``GroupTheoryApi`` tests, and Python-only checks."""

from collections.abc import Mapping
from fractions import Fraction

import pytest

from pymarty.errors import MartyValueError
from pymarty.group_theory import (
    E6,
    E7,
    E8,
    F4,
    G2,
    SO,
    SU,
    U1,
    Decomposition,
    Irrep,
    Sp,
)


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


@pytest.mark.parity("GroupTheoryApi.SymplecticAndExceptionalDecompositions")
def test_symplectic_and_exceptional_decompositions() -> None:
    def dims_of(prod: Decomposition) -> dict[int, int]:
        dims: dict[int, int] = {}
        for r, m in prod.items():
            dims[r.dim] = dims.get(r.dim, 0) + m
        return dims

    sp4 = Sp(4)
    assert (sp4.fundamental().dim, sp4.adjoint().dim) == (4, 10)
    assert dims_of(sp4.fundamental() * sp4.fundamental()) == {1: 1, 5: 1, 10: 1}

    g2 = G2()
    assert (g2.fundamental().dim, g2.adjoint().dim) == (7, 14)
    assert dims_of(g2.fundamental() * g2.fundamental()) == {1: 1, 7: 1, 14: 1, 27: 1}

    f4 = F4()
    assert (f4.fundamental().dim, f4.adjoint().dim) == (26, 52)
    assert dims_of(f4.fundamental() * f4.fundamental()) == {
        1: 1,
        26: 1,
        52: 1,
        273: 1,
        324: 1,
    }

    e7 = E7()
    assert (e7.fundamental().dim, e7.adjoint().dim) == (56, 133)
    assert dims_of(e7.fundamental() * e7.fundamental()) == {
        1: 1,
        133: 1,
        1463: 1,
        1539: 1,
    }

    e8 = E8()
    assert (e8.fundamental().dim, e8.adjoint().dim) == (248, 248)
    assert dims_of(e8.fundamental() * e8.fundamental()) == {
        1: 1,
        248: 1,
        3875: 1,
        27000: 1,
        30380: 1,
    }


@pytest.mark.parity("GroupTheoryApi.SymplecticDimensionIsValidated")
def test_symplectic_dimension_is_validated() -> None:
    with pytest.raises(MartyValueError):
        Sp(2)
    with pytest.raises(MartyValueError):
        Sp(5)


@pytest.mark.parity("GroupTheoryApi.ConjugationFollowsDiagramAutomorphism")
def test_conjugation_follows_diagram_automorphism() -> None:
    e6 = E6()
    c27 = ~e6.fundamental()
    assert (c27.dim, c27.highest_weight) == (27, (0, 0, 0, 0, 1, 0))
    assert not e6.fundamental().is_self_conjugate
    assert e6.adjoint().is_self_conjugate

    assert Sp(4).fundamental().is_self_conjugate

    so10 = SO(10)
    spinor = so10.irrep(0, 0, 0, 0, 1)
    assert spinor.dim == 16
    assert (~spinor).highest_weight == (0, 0, 0, 1, 0)
    assert not spinor.is_self_conjugate
    assert so10.fundamental().is_self_conjugate

    assert E7().fundamental().is_self_conjugate
