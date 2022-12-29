def test_traces():
    from pymarty.gamma_api import (
        g,
        eps,
        gamma,
        gamma5,
        P_L,
        P_R,
        imaginary_unit as ii,
    )
    from pymarty.gamma_api import trace

    assert trace([gamma(0), gamma(1)]) == 4 * g(0, 1)
    assert trace([gamma(0), gamma(1), P_L()]) == 2 * g(0, 1)
    assert trace([gamma(0), gamma(1), P_R()]) == 2 * g(0, 1)
    assert trace([gamma5()]) == 0
    assert trace([gamma(0), gamma(1), gamma(2), gamma(3), gamma5()]) == -4 * ii * eps(
        0, 1, 2, 3
    )


def test_simplifications():
    from pymarty.gamma_api import Expr, gamma, gamma5, P_L, P_R, C
    from pymarty.gamma_api import (
        current,
        set_symbolic_dimension,
        FierzBasis,
        single_fierz,
        double_fierz,
    )

    set_symbolic_dimension(keep_symbolic=False)
    assert current([gamma(0), gamma(0)], 0, 1).simplify() == 4 * current([], 0, 1)
    assert current([C(), C()], 0, 1).simplify() == -current([], 0, 1)
    assert current([P_L(), gamma5()], 0, 1).simplify() == -current([P_L()], 0, 1)
    assert current([P_R(), gamma5()], 0, 1).simplify() == current([P_R()], 0, 1)
    assert current([P_R(), P_L()], 0, 1).simplify() == 0
    assert current([gamma(0), gamma(1), gamma(0)], 2, 3).simplify() == -2 * current(
        [gamma(1)], 2, 3
    )
    assert current([gamma(0), gamma(1), gamma(0)], 2, 3).simplify() != -2 * current(
        [gamma(1)], 3, 2
    )
    assert current([gamma5(), gamma(0)], 0, 1).order() == -current(
        [gamma(0), gamma5()], 0, 1
    )
    assert current([gamma(0, 1), gamma5()], 0, 1).project(
        basis=FierzBasis.Chiral
    ) == current([gamma(0, 1), P_R()], 0, 1) - current([gamma(0, 1), P_L()], 0, 1)

    assert double_fierz(
        left_current=current([gamma(0), gamma(1), gamma(2), P_L()], 0, 1),
        right_current=current([gamma(2), gamma(1), gamma(0), P_L()], 0, 1),
        basis=FierzBasis.Chiral,
    ) == 4 * current([gamma(0), P_L()], 0, 1) * current([gamma(0), P_L()], 0, 1)

    assert single_fierz(
        left_current=current([P_R()], 0, 1),
        right_current=current([P_L()], 2, 3),
        basis=FierzBasis.Chiral,
    ) == Expr(1) / 2 * current([gamma(0), P_L()], 0, 3) * current(
        [gamma(0), P_R()], 2, 1
    )
