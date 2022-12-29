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
