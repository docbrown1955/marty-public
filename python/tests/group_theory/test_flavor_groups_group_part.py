"""Group part of the C++ ``flavor_groups`` integration tests.

The model-level flavour spaces and amplitudes of that test are Phase 4 and 5
features. This file checks the U(1) charge structure it relies on: charge-1
scalars, their conjugates, and a neutral gauge boson.
"""

from fractions import Fraction

from pymarty.group_theory import U


def test_charged_scalar_pair_contains_the_singlet() -> None:
    u1 = U(1)
    p = u1.irrep(1)
    p_bar = ~p
    assert p.charge == 1
    assert p_bar.charge == -1
    assert list((p * p_bar).items()) == [(u1.trivial(), 1)]


def test_gauge_boson_is_neutral_and_self_conjugate() -> None:
    u1 = U(1)
    a_x = u1.trivial()
    assert a_x.charge == 0
    assert a_x.is_self_conjugate
    assert list((a_x * a_x).items()) == [(u1.trivial(), 1)]


def test_charged_scalar_pair_has_no_neutral_leftover() -> None:
    u1 = U(1)
    p = u1.irrep(Fraction(1))
    products = dict((r.charge, m) for r, m in (p * p).items())
    assert products == {Fraction(2): 1}
