def test_set_symbolic_dimension():
    from pymarty.gamma_api import Expr, set_symbolic_dimension, D

    set_symbolic_dimension(keep_symbolic=True)
    expr = D.simplify()
    assert D == expr
    assert Expr(4) != expr
    set_symbolic_dimension(keep_symbolic=False)
    expr = D.simplify()
    assert D != expr
    assert Expr(4) == expr


def test_basic_expressions():
    from pymarty.gamma_api import g, eps, gamma, gamma5, P_L, P_R, C, D
    from pymarty.gamma_api import current, set_symbolic_dimension

    set_symbolic_dimension(keep_symbolic=False)
    assert g(0, 1) == g(0, 1)
    assert g(0, 1) == g(1, 0)
    assert g(0, 1) != g(0, 2)
    assert g(0, 1) * g(1, 2) == g(0, 2)
    assert g(0, 0) == D
    assert eps(0, 1, 2, 3) == eps(0, 1, 2, 3)
    assert eps(0, 1, 2, 3) == (-eps(0, 2, 1, 3)).simplify()
    assert eps(0, 1, 2, 3) != eps(1, 0, 2, 3)
    assert eps(0, 0, 1, 3) == 0
    assert 0 == eps(0, 0, 1, 3)
    assert eps(0, 1, 2, 3) != 0
    assert 0 != eps(0, 1, 2, 3)
    assert (eps(0, 1, 2, 3) * eps(0, 1, 2, 4)).simplify() == -6 * g(3, 4)

    c1 = current([gamma(0), gamma5()], 0, 1)
    c2 = current([gamma(0, 1), P_L()], 2, 3)
    c3 = current([P_R(), C()], 4, 5)
    assert c1 == c1
    assert c2 == c2
    assert c3 == c3
    assert c1 != c2
    assert c1 != c3
    assert c2 != c3
