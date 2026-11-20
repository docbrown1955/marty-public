"""Python counterparts of the ``marty_gamma_api`` cases of the C++ integration
suite (``tests/integration/src/gammaAPI.cpp``)."""

import itertools
from fractions import Fraction

import pytest

from pymarty.diracology import (
    P_L,
    P_R,
    C,
    D,
    DiracExpr,
    FierzBasis,
    I,
    current,
    double_fierz,
    eps,
    g,
    gamma,
    gamma5,
    single_fierz,
    symbolic_dimension,
    trace,
)

HALF = Fraction(1, 2)
QUARTER = Fraction(1, 4)
EIGHTH = Fraction(1, 8)
ALL_DISTINCT_EPSILON_INDICES = list(itertools.permutations(range(4)))


def _sign(indices: tuple[int, ...]) -> int:
    """Sign of the permutation sorting four distinct indices."""
    inversions = sum(
        1 for i, j in itertools.combinations(range(4), 2) if indices[i] > indices[j]
    )
    return -1 if inversions % 2 else 1


@pytest.mark.parity("marty_gamma_api.equality")
def test_equality_distinguishes_structures() -> None:
    with symbolic_dimension(keep=False):
        assert g(0, 1) != g(0, 2)
        assert g(0, 0) != g(0, 1)
        assert eps(0, 1, 2, 3) != eps(1, 0, 2, 3)
        assert g(0, 1) != eps(0, 1, 2, 3)
        assert current([gamma(0)], 0, 1) != current([gamma(1)], 0, 1)
        assert current([gamma(0)], 0, 1) != current([gamma(0)], 0, 2)
        assert current([gamma(0)], 0, 1) != current([], 0, 1)
        assert current([gamma(0)], 0, 1) != current([gamma5()], 0, 1)
        assert current([gamma5()], 0, 1) != current([P_L()], 0, 1)
        assert current([P_L()], 0, 1) != current([P_R()], 0, 1)
        assert current([P_R()], 0, 1) != current([C()], 0, 1)
        assert current([gamma(0)], 0, 1) != current([gamma(0, 1)], 0, 1)
        assert current([gamma(1, 0)], 0, 1) != current([gamma(0, 1)], 0, 1)
    with symbolic_dimension(keep=True):
        assert D != 4
        assert D.simplify() != 4
    with symbolic_dimension(keep=False):
        assert D != 4
        assert D.simplify() == 4


@pytest.mark.parity("marty_gamma_api.metric_contractions")
def test_metric_contractions() -> None:
    with symbolic_dimension(keep=False):
        assert (g(0, 1) * g(0, 2)).simplify() == g(1, 2)
        assert (g(1, 0) * g(0, 2)).simplify() == g(1, 2)
        assert (g(0, 1) * g(2, 3)).simplify() == g(0, 1) * g(2, 3)
    with symbolic_dimension(keep=True):
        assert (g(0, 1) * g(0, 1)).simplify() == D


@pytest.mark.parity("marty_gamma_api.epsilon_contractions")
def test_epsilon_contractions() -> None:
    for m in ALL_DISTINCT_EPSILON_INDICES:
        m0, m1, m2, m3 = m
        assert eps(m0, m1, m2, m2) == 0
        assert (g(m2, m3) * eps(m0, m1, m2, m3)).simplify() == 0
        with symbolic_dimension(keep=True):
            expected = -_sign(m) * (D * (D - 1) * (D - 2) * (D - 3)).simplify()
            assert (eps(0, 1, 2, 3) * eps(m0, m1, m2, m3)).simplify() == expected
        with symbolic_dimension(keep=False):
            sign = _sign(m)
            assert (eps(0, 1, 2, 3) * eps(m0, m1, m2, m3)).simplify() == -24 * sign
            assert (eps(0, 1, 2, 4) * eps(m0, m1, m2, m3)).simplify() == (
                -6 * sign * g(3, 4)
            )
            assert (eps(0, 1, 4, 5) * eps(m0, m1, m2, m3)).simplify() == (
                -2 * sign * (g(2, 4) * g(3, 5) - g(2, 5) * g(3, 4))
            )
            assert (eps(0, 4, 5, 6) * eps(m0, m1, m2, m3)).simplify() == (
                -sign
                * (
                    g(1, 4) * g(2, 5) * g(3, 6)
                    - g(1, 4) * g(2, 6) * g(3, 5)
                    - g(1, 5) * g(2, 4) * g(3, 6)
                    + g(1, 5) * g(2, 6) * g(3, 4)
                    - g(1, 6) * g(2, 5) * g(3, 4)
                    + g(1, 6) * g(2, 4) * g(3, 5)
                )
            )


@pytest.mark.parity("marty_gamma_api.gamma_contractions")
def test_gamma_contractions() -> None:
    with symbolic_dimension(keep=False):
        assert current([gamma(0), gamma(0)], 0, 1).simplify() == 4 * current([], 0, 1)
        assert current([gamma5(), gamma5()], 0, 1).simplify() == current([], 0, 1)
        assert current([P_L(), P_L()], 0, 1).simplify() == current([P_L()], 0, 1)
        assert current([P_R(), P_R()], 0, 1).simplify() == current([P_R()], 0, 1)
        assert current([P_L(), P_R()], 0, 1).simplify() == 0
        assert current([gamma5(), P_R()], 0, 1).simplify() == current([P_R()], 0, 1)
        assert current([gamma5(), P_L()], 0, 1).simplify() == -current([P_L()], 0, 1)
        assert current([C(), C()], 0, 1).simplify() == -current([], 0, 1)
    with symbolic_dimension(keep=True):
        assert current([gamma(0), gamma(0, 1)], 0, 1).simplify() == (D - 1) * current(
            [gamma(1)], 0, 1
        )
        assert current([gamma(1), gamma(0, 1)], 0, 1).simplify() == (1 - D) * current(
            [gamma(0)], 0, 1
        )
        assert current([gamma(0, 1), gamma(0, 1)], 0, 1).simplify() == (
            D * (1 - D) * current([], 0, 1)
        )
        assert current([gamma(0, 1), gamma(1, 0)], 0, 1).simplify() == (
            D * (D - 1) * current([], 0, 1)
        )


@pytest.mark.parity("marty_gamma_api.simple_simplification")
def test_simple_simplification() -> None:
    with symbolic_dimension(keep=True):
        c0 = current([gamma(0), gamma(0)], 0, 1)
        assert c0.simplify() == D * current([], 0, 1)
        assert c0.simplify() != 4 * current([], 0, 1)

        c1 = current([gamma(0), gamma(1), gamma(0)], 0, 1)
        assert c1.simplify() == 2 * current([gamma(1)], 0, 1) - D * current(
            [gamma(1)], 0, 1
        )
        assert c1.simplify() != 2 * current([gamma(0)], 0, 1) - D * current(
            [gamma(0)], 0, 1
        )

        c2 = current([gamma(0), gamma5(), gamma(0)], 0, 1)
        assert c2.simplify() == -D * current([gamma5()], 0, 1)
        assert c2.simplify() != -D * current([gamma5()], 0, 2)

        c3 = current([gamma5(), gamma(0)], 0, 1)
        assert c3.simplify() == -1 * current([gamma(0), gamma5()], 0, 1)
        assert c3.simplify() != -1 * current([gamma(1), gamma5()], 0, 1)

        c4 = current(
            [gamma5(), gamma(1), P_L(), gamma5(), gamma(0), P_R(), gamma5()], 0, 1
        )
        assert c4.simplify() == -1 * current([gamma(1), gamma(0), P_R()], 0, 1)
        assert c4.simplify() != -1 * current([gamma(1), gamma5(), P_L()], 0, 1)

        c5 = current([gamma5(), gamma(1), P_L(), C(), C(), gamma(0), P_R()], 0, 1)
        assert c5.simplify() == -1 * current([gamma(1), gamma(0), P_R()], 0, 1)
        assert c5.simplify() != -1 * current([gamma(1), gamma5(), P_L()], 0, 1)


@pytest.mark.parity("marty_gamma_api.basis_projection")
def test_basis_projection() -> None:
    chiral = FierzBasis.CHIRAL
    standard = FierzBasis.STANDARD
    assert current([gamma(0)], 0, 1).project(chiral) == current(
        [gamma(0), P_L()], 0, 1
    ) + current([gamma(0), P_R()], 0, 1)
    assert current([gamma(0), gamma5()], 0, 1).project(chiral) == -current(
        [gamma(0), P_L()], 0, 1
    ) + current([gamma(0), P_R()], 0, 1)
    assert current([], 0, 1).project(chiral) == current([P_L()], 0, 1) + current(
        [P_R()], 0, 1
    )
    assert current([gamma5()], 0, 1).project(chiral) == -current(
        [P_L()], 0, 1
    ) + current([P_R()], 0, 1)

    assert current([P_L()], 0, 1).project(standard) == HALF * current(
        [], 0, 1
    ) - HALF * current([gamma5()], 0, 1)
    assert current([P_R()], 0, 1).project(standard) == HALF * current(
        [], 0, 1
    ) + HALF * current([gamma5()], 0, 1)
    assert current([gamma(0), P_L()], 0, 1).project(standard) == HALF * current(
        [gamma(0)], 0, 1
    ) - HALF * current([gamma(0), gamma5()], 0, 1)
    assert current([gamma(0, 1), gamma(2), P_R()], 0, 1).project(standard) == (
        HALF * current([gamma(0, 1), gamma(2)], 0, 1)
        + HALF * current([gamma(0, 1), gamma(2), gamma5()], 0, 1)
    )


@pytest.mark.parity("marty_gamma_api.simple_traces")
def test_simple_traces() -> None:
    with symbolic_dimension(keep=True):
        assert trace([]) == 4
        assert trace([gamma(0)]) == 0
        assert trace([gamma(0), gamma(1)]) == 4 * g(0, 1)
        assert trace([gamma(0, 1)]) == 0
        assert trace([gamma(0), gamma(1), gamma(2)]) == 0
        assert trace([gamma(0), gamma(1), gamma(2), gamma(3)]) == 4 * (
            g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3)
        )
        assert trace([gamma(0, 1), gamma(2, 3)]) == 4 * (
            g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3)
        )
        assert trace([gamma5()]) == 0
        assert trace([P_L()]) == 2
        assert trace([P_R()]) == 2
        assert trace([gamma(0), gamma5()]) == 0
        assert trace([gamma(0), gamma(1), gamma5()]) == 0
        assert trace([gamma(0), gamma(1), gamma(2), gamma5()]) == 0
        assert trace([gamma(0), gamma(1), gamma(2), gamma(3), gamma5()]) == (
            -4 * I * eps(0, 1, 2, 3)
        )
        with pytest.raises(RuntimeError):
            trace([C()])
        with pytest.raises(RuntimeError):
            trace([gamma(0), C()])
        with pytest.raises(RuntimeError):
            trace([gamma(0), gamma5(), C()])


@pytest.mark.parity("marty_gamma_api.complex_traces")
def test_complex_traces() -> None:
    metric_part = 4 * (g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3))
    with symbolic_dimension(keep=False):
        assert (
            2 * trace([gamma(0), gamma(1), gamma(2), gamma(3), P_L()])
        ).simplify() == (metric_part - (-4 * I * eps(0, 1, 2, 3))).simplify()
        assert (
            2 * trace([gamma(0), gamma(1), gamma(2), gamma(3), P_R()])
        ).simplify() == (metric_part + (-4 * I * eps(0, 1, 2, 3))).simplify()
        assert (
            2 * trace([gamma(2), gamma(3), P_L(), gamma(0), gamma(1)])
        ).simplify() == (metric_part - (-4 * I * eps(0, 1, 2, 3))).simplify()
        assert (
            2 * trace([gamma(2), gamma(3), P_R(), gamma(0), gamma(1)])
        ).simplify() == (metric_part + (-4 * I * eps(0, 1, 2, 3))).simplify()
        assert (
            2 * trace([gamma(2), gamma(3), P_R(), gamma(0), P_R(), gamma(1)])
        ).simplify() == 0
        assert (
            2 * trace([gamma(2), gamma(3), P_R(), P_L(), gamma(0), gamma(1)])
        ).simplify() == 0
        assert (
            2 * trace([gamma(2), gamma(3), gamma(0), gamma(1), gamma(4)])
        ).simplify() == 0

    six_gammas = [gamma(0), gamma(1), gamma(2), gamma(3), gamma(4), gamma(5), gamma5()]
    factor = -4 * I
    with symbolic_dimension(keep=False):
        cases = [
            (0, 1, D * factor * eps(2, 3, 4, 5)),
            (0, 2, (2 - D) * factor * eps(1, 3, 4, 5)),
            (0, 3, 0 * factor),
            (0, 4, (D - 2) * factor * eps(1, 2, 3, 5)),
            (0, 5, -D * factor * eps(1, 2, 3, 4)),
        ]
        for mu, nu, expected in cases:
            tr = g(mu, nu) * trace(six_gammas)
            assert tr.simplify() == expected.simplify()


@pytest.mark.parity("marty_gamma_api.chisholm_identities")
def test_chisholm_identities() -> None:
    # The C++ "simple" Chisholm case sets sgl::option::orderEspilonIndices to
    # false, which is not exposed to Python. This test covers the sigma and
    # fully contracted Chisholm identities.
    with symbolic_dimension(keep=False):
        for m in ALL_DISTINCT_EPSILON_INDICES:
            m0, m1, m2, m3 = m
            c = eps(m0, m1, m2, m3) * current([gamma(m3, 4)], 0, 1)
            expected = (
                I * g(m0, 4) * current([gamma(m1, m2), gamma5()], 0, 1)
                - I * g(m1, 4) * current([gamma(m0, m2), gamma5()], 0, 1)
                + I * g(m2, 4) * current([gamma(m0, m1), gamma5()], 0, 1)
            )
            assert c.simplify() == expected.simplify()
            wrong = (
                I * g(m0, 4) * current([gamma(m1, m2)], 0, 1)
                - I * g(m1, 4) * current([gamma(m0, m2)], 0, 1)
                + I * g(m2, 4) * current([gamma(m0, m1)], 0, 1)
            )
            assert c.simplify() != wrong.simplify()
    with symbolic_dimension(keep=True):
        for m in ALL_DISTINCT_EPSILON_INDICES:
            m0, m1, m2, m3 = m
            c = eps(m0, m1, m2, m3) * current([gamma(m2, m3)], 0, 1)
            assert (
                c.simplify()
                == (I * (2 - D) * current([gamma(m0, m1), gamma5()], 0, 1)).simplify()
            )


@pytest.mark.parity("marty_gamma_api.conjugation_identities")
def test_conjugation_identities() -> None:
    with symbolic_dimension(keep=False):
        c = current([C(), gamma(0), C()], 1, 0)
        assert c.simplify() == current([gamma(0)], 0, 1)
        assert c.simplify() != current([gamma(0)], 1, 0)

        c = current([C(), C(), C()], 1, 0)
        assert c.simplify() != -1 * current([C()], 0, 1)
        assert c.simplify() == -1 * current([C()], 1, 0)

        c = current([C(), gamma5(), C()], 1, 0)
        assert c.simplify() == -1 * current([gamma5()], 0, 1)
        assert c.simplify() != -1 * current([gamma5()], 1, 0)

        c = current([C(), P_L(), C()], 1, 0)
        assert c.simplify() == -1 * current([P_L()], 0, 1)
        assert c.simplify() != -1 * current([P_L()], 1, 0)

        c = current([C(), P_R(), C()], 1, 0)
        assert c.simplify() == -1 * current([P_R()], 0, 1)
        assert c.simplify() != -1 * current([P_R()], 1, 0)

        c = current([C(), gamma(0, 1), C()], 1, 0)
        assert c.simplify() == current([gamma(0, 1)], 0, 1)
        assert c.simplify() != current([gamma(0, 1)], 1, 0)

        c = current([C(), gamma(0)], 1, 0) * current([gamma(1)], 1, 2)
        assert c.simplify() == current([C(), gamma(0), gamma(1)], 0, 2)

        c = current([gamma(0), C()], 1, 0) * current([C(), gamma(1)], 1, 2)
        assert c.simplify() == -1 * current([gamma(0), gamma(1)], 0, 2)

        c = current([gamma(1)], 1, 2) * current([C(), gamma(0)], 1, 0)
        assert c.simplify() == current([C(), gamma(0), gamma(1)], 0, 2)

        c = current([C(), gamma(1)], 1, 2) * current([gamma(0), C()], 1, 0)
        assert c.simplify() == -1 * current([gamma(0), gamma(1)], 0, 2)

        c = current(
            [
                C(),
                gamma(1),
                gamma5(),
                gamma(0),
                gamma5(),
                C(),
                C(),
                P_L(),
                gamma(2, 3),
                P_L(),
                C(),
            ],
            1,
            0,
        )
        expected = current([gamma(2, 3), gamma(0), gamma(1), P_L()], 0, 1)
        assert c.simplify() == expected.simplify()


def test_simple_fierz_completeness_chiral() -> None:
    with symbolic_dimension(keep=False):
        fierz = single_fierz(
            current([], 0, 1), current([], 2, 3), FierzBasis.CHIRAL
        ).rhs
        expected = HALF * (
            current([P_L()], 0, 3) * current([P_L()], 2, 1)
            + current([P_R()], 0, 3) * current([P_R()], 2, 1)
            + current([gamma(0), P_R()], 0, 3) * current([gamma(0), P_L()], 2, 1)
            + current([gamma(0), P_L()], 0, 3) * current([gamma(0), P_R()], 2, 1)
            - QUARTER
            * current([gamma(0, 1), P_R()], 0, 3)
            * current([gamma(0, 1), P_R()], 2, 1)
            - QUARTER
            * current([gamma(0, 1), P_R()], 0, 3)
            * current([gamma(0, 1), P_L()], 2, 1)
            - QUARTER
            * current([gamma(0, 1), P_L()], 0, 3)
            * current([gamma(0, 1), P_R()], 2, 1)
            - QUARTER
            * current([gamma(0, 1), P_L()], 0, 3)
            * current([gamma(0, 1), P_L()], 2, 1)
        )
        assert fierz == expected


def test_simple_fierz_completeness_standard() -> None:
    with symbolic_dimension(keep=False):
        fierz = single_fierz(
            current([], 0, 1), current([], 2, 3), FierzBasis.STANDARD
        ).rhs
        expected = QUARTER * (
            current([], 0, 3) * current([], 2, 1)
            + current([gamma5()], 0, 3) * current([gamma5()], 2, 1)
            + current([gamma(0)], 0, 3) * current([gamma(0)], 2, 1)
            - current([gamma(0), gamma5()], 0, 3) * current([gamma(0), gamma5()], 2, 1)
            - HALF * current([gamma(0, 1)], 0, 3) * current([gamma(0, 1)], 2, 1)
        )
        assert fierz == expected


def test_simple_fierz_projector_pairs() -> None:
    with symbolic_dimension(keep=False):
        fierz = single_fierz(current([P_L()], 0, 1), current([P_R()], 2, 3)).rhs
        assert fierz == HALF * current([gamma(0), P_R()], 0, 3) * current(
            [gamma(0), P_L()], 2, 1
        )
        fierz = single_fierz(current([P_R()], 0, 1), current([P_L()], 2, 3)).rhs
        assert fierz == HALF * current([gamma(0), P_L()], 0, 3) * current(
            [gamma(0), P_R()], 2, 1
        )


@pytest.mark.parity("marty_gamma_api.simple_fierz_identities")
def test_simple_fierz_identities() -> None:
    test_simple_fierz_completeness_chiral()
    test_simple_fierz_completeness_standard()
    test_simple_fierz_projector_pairs()
    with symbolic_dimension(keep=False):
        fierz = single_fierz(
            current([P_L()], 0, 1), current([P_L()], 2, 3), FierzBasis.CHIRAL
        ).rhs
        assert fierz == (
            HALF * current([P_L()], 0, 3) * current([P_L()], 2, 1)
            - EIGHTH
            * current([gamma(0, 1), P_R()], 0, 3)
            * current([gamma(0, 1), P_L()], 2, 1)
            - EIGHTH
            * current([gamma(0, 1), P_L()], 0, 3)
            * current([gamma(0, 1), P_L()], 2, 1)
        )
        fierz = single_fierz(
            current([P_R()], 0, 1), current([P_R()], 2, 3), FierzBasis.CHIRAL
        ).rhs
        assert fierz == (
            HALF * current([P_R()], 0, 3) * current([P_R()], 2, 1)
            - EIGHTH
            * current([gamma(0, 1), P_R()], 0, 3)
            * current([gamma(0, 1), P_R()], 2, 1)
            - EIGHTH
            * current([gamma(0, 1), P_L()], 0, 3)
            * current([gamma(0, 1), P_R()], 2, 1)
        )
        fierz = single_fierz(
            current([P_R(), gamma(0, 1)], 0, 1),
            current([P_R(), gamma(1)], 2, 3),
            FierzBasis.CHIRAL,
        ).rhs
        assert fierz == (
            Fraction(3, 2) * current([gamma(0), P_L()], 0, 3) * current([P_R()], 2, 1)
            - HALF
            * current([gamma(1), P_L()], 0, 3)
            * current([gamma(0, 1), P_R()], 2, 1)
        )


@pytest.mark.parity("marty_gamma_api.double_fierz_identities")
def test_double_fierz_identities() -> None:
    def canonical(g1: list[DiracExpr], g2: list[DiracExpr], basis: FierzBasis) -> None:
        c1 = current(g1, 0, 1)
        c2 = current(g2, 2, 3)
        assert double_fierz(c1, c2, basis).rhs == c1 * c2

    with symbolic_dimension(keep=False):
        for g1, g2 in [
            ([P_L()], [P_L()]),
            ([P_L()], [P_R()]),
            ([P_L()], [gamma(0), P_R()]),
            ([gamma(1), P_R()], [gamma(0), P_R()]),
            ([gamma(1), P_R()], [gamma(0), P_L()]),
            ([gamma(0, 1), P_L()], [P_L()]),
            ([gamma(0, 1), P_R()], [P_L()]),
        ]:
            canonical(g1, g2, FierzBasis.CHIRAL)
        for g1, g2 in [
            ([], []),
            ([], [gamma5()]),
            ([gamma5()], [gamma5()]),
            ([], [gamma(0)]),
            ([gamma5()], [gamma(0)]),
            ([gamma5()], [gamma(0), gamma5()]),
            ([gamma(0, 1)], []),
            ([gamma(0, 1)], [gamma5()]),
        ]:
            canonical(g1, g2, FierzBasis.STANDARD)

        def fierz(g1: list[DiracExpr], g2: list[DiracExpr]) -> DiracExpr:
            return double_fierz(
                current(g1, 0, 1), current(g2, 2, 3), FierzBasis.CHIRAL
            ).rhs

        assert fierz(
            [gamma(0), gamma(1), gamma(2), P_L()], [gamma(2), gamma(1), gamma(0), P_L()]
        ) == 4 * current([gamma(0), P_L()], 0, 1) * current([gamma(0), P_L()], 2, 3)
        assert fierz(
            [gamma(0), gamma(1), gamma(2), P_L()], [gamma(1), gamma(2), gamma(0), P_L()]
        ) == 4 * current([gamma(0), P_L()], 0, 1) * current([gamma(0), P_L()], 2, 3)
        assert fierz(
            [gamma(0), gamma(1), gamma(2), P_L()], [gamma(0), gamma(1), gamma(2), P_L()]
        ) == 16 * current([gamma(0), P_L()], 0, 1) * current([gamma(0), P_L()], 2, 3)
