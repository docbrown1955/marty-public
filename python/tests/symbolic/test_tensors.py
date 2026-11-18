"""Tensors, metric, delta, epsilon and relabelling of the symbolic API."""

import pytest

from pymarty import symbolic
from pymarty.errors import MartyTypeError, MartyValueError


@pytest.mark.unit
def test_tensor_equality_depends_on_order_and_rank() -> None:
    euclid = symbolic.Euclid_R3
    i1 = euclid.index("i1")
    i2 = euclid.index("i2")
    t = symbolic.tensor("t", [euclid, euclid])
    assert t(i1, i2) == t(i1, i2)
    assert t(i1, i2) != t(i1, i1)
    assert t(i1, i2) != t(i2, i1)
    assert t(i1, i2) != symbolic.tensor("u", [euclid, euclid])(i1, i2)
    assert t(i1, i2) != symbolic.tensor("t", [euclid])(i1)


@pytest.mark.unit
def test_tensor_element_free_indices() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    b = euclid.index("b")
    element = symbolic.tensor("T", [euclid, euclid])(a, b)
    assert sorted(i.name for i in element.free_indices) == ["a", "b"]
    assert element.dummy_indices == ()


@pytest.mark.unit
def test_tensor_element_rejects_wrong_rank_and_foreign_index() -> None:
    euclid = symbolic.Euclid_R3
    tensor = symbolic.tensor("T", [euclid])
    with pytest.raises(MartyValueError):
        tensor(euclid.index("a"), euclid.index("b"))
    with pytest.raises(MartyValueError):
        tensor(symbolic.Euclid_R2.index("a"))


@pytest.mark.unit
def test_symmetric_tensor_pair() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    b = euclid.index("b")
    symmetric = symbolic.tensor("S", [euclid, euclid], symmetric=[(0, 1)])
    plain = symbolic.tensor("P", [euclid, euclid])
    assert symmetric(a, b) == symmetric(b, a)
    assert plain(a, b) != plain(b, a)


@pytest.mark.unit
def test_antisymmetric_tensor_pair() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    b = euclid.index("b")
    anti = symbolic.tensor("A", [euclid, euclid], antisymmetric=[(0, 1)])
    assert anti(a, b) + anti(b, a) == 0


@pytest.mark.unit
def test_fully_symmetric_tensor() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    b = euclid.index("b")
    c = euclid.index("c")
    sym = symbolic.tensor("S", [euclid, euclid, euclid], symmetric=True)
    assert sym(a, b, c) == sym(c, a, b)


@pytest.mark.unit
def test_symmetry_conflicts_raise() -> None:
    euclid = symbolic.Euclid_R3
    with pytest.raises(MartyValueError):
        symbolic.tensor("T", [euclid, euclid], symmetric=True, antisymmetric=True)
    with pytest.raises(MartyValueError):
        symbolic.tensor(
            "T", [euclid, euclid], symmetric=[(0, 1)], antisymmetric=[(1, 0)]
        )
    with pytest.raises(MartyValueError):
        symbolic.tensor("T", [euclid, euclid], symmetric=[(0, 2)])
    with pytest.raises(MartyValueError):
        symbolic.tensor("T", [euclid, euclid], symmetric=[(1, 1)])
    with pytest.raises(MartyValueError):
        symbolic.tensor("T", [])


@pytest.mark.unit
def test_metric_concrete_values() -> None:
    mink = symbolic.Minkowski
    m0 = mink.fixed_index(0)
    m1 = mink.fixed_index(1)
    assert symbolic.metric(m0, m0) == 1
    assert symbolic.metric(m1, m1) == -1
    assert symbolic.metric(m0, m1) == 0


@pytest.mark.unit
def test_delta_concrete_values() -> None:
    mink = symbolic.Minkowski
    m0 = mink.fixed_index(0)
    m1 = mink.fixed_index(1)
    assert symbolic.delta(m0, m0) == 1
    assert symbolic.delta(m0, m1) == 0


@pytest.mark.unit
def test_epsilon_concrete_values() -> None:
    euclid = symbolic.Euclid_R3
    e0, e1, e2 = (euclid.fixed_index(k) for k in (0, 1, 2))
    assert symbolic.epsilon(e0, e2, e1) == -1
    assert symbolic.epsilon(e0, e0, e1) == 0


@pytest.mark.unit
def test_epsilon_antisymmetry_with_free_indices() -> None:
    euclid = symbolic.Euclid_R3
    a = euclid.index("a")
    b = euclid.index("b")
    c = euclid.index("c")
    assert symbolic.epsilon(a, b, c) + symbolic.epsilon(b, a, c) == 0


@pytest.mark.unit
def test_epsilon_rejects_wrong_arity_and_empty_call() -> None:
    euclid = symbolic.Euclid_R3
    with pytest.raises(MartyValueError):
        symbolic.epsilon()
    with pytest.raises(MartyValueError):
        symbolic.epsilon(euclid.index("a"), euclid.index("b"))


@pytest.mark.unit
def test_relabel_canonical_names() -> None:
    euclid = symbolic.Euclid_R3
    T = symbolic.tensor("T", [euclid, euclid])
    U = symbolic.tensor("U", [euclid])
    a, b, c, d = (euclid.index(name) for name in ("a", "b", "c", "d"))
    first = T(a, b) * U(b)
    second = T(c, d) * U(d)
    assert symbolic.relabel(first) == symbolic.relabel(second)


@pytest.mark.unit
def test_relabel_does_not_mutate() -> None:
    euclid = symbolic.Euclid_R3
    T = symbolic.tensor("T", [euclid, euclid])
    U = symbolic.tensor("U", [euclid])
    a = euclid.index("a")
    b = euclid.index("b")
    expr = T(a, b) * U(b)
    before = str(expr)
    relabelled = symbolic.relabel(expr)
    assert str(expr) == before
    assert [i.name for i in expr.free_indices] == ["a"]
    assert str(relabelled) != before


@pytest.mark.unit
def test_relabel_free_keeps_free_names_when_disabled() -> None:
    euclid = symbolic.Euclid_R3
    T = symbolic.tensor("T", [euclid, euclid])
    U = symbolic.tensor("U", [euclid])
    a = euclid.index("a")
    b = euclid.index("b")
    expr = T(a, b) * U(b)
    kept = symbolic.relabel(expr, relabel_free=False)
    assert [i.name for i in kept.free_indices] == ["a"]
    assert [i.name for i in symbolic.relabel(expr).free_indices] != ["a"]


@pytest.mark.unit
def test_relabel_expression_without_indices_is_unchanged() -> None:
    a = symbolic.variable("a")
    b = symbolic.variable("b")
    expr = a + 2 * b
    assert symbolic.relabel(expr).to_latex() == expr.to_latex()


@pytest.mark.unit
def test_relabel_single_free_index_takes_first_name() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    relabelled = symbolic.relabel(x(space.index("f")))
    assert relabelled.to_latex() == "x_{alpha}"
    assert [(i.name, i.id) for i in relabelled.free_indices] == [("alpha", 0)]


@pytest.mark.unit
def test_relabel_distinct_free_indices_take_distinct_names() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    T = symbolic.tensor("T", [space, space])
    relabelled = symbolic.relabel(T(space.index("f"), space.index("g")))
    assert {i.name for i in relabelled.free_indices} == {"alpha", "beta"}


@pytest.mark.unit
def test_relabel_repeated_free_index_in_sum_keeps_one_label() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    relabelled = symbolic.relabel(x(i) + y(i))
    assert {idx.name for idx in relabelled.free_indices} == {"alpha"}
    latex = relabelled.to_latex()
    assert "x_{alpha}" in latex
    assert "y_{alpha}" in latex


@pytest.mark.unit
def test_relabel_dummy_pair_shares_one_label() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    relabelled = symbolic.relabel(x(i) * y(i))
    assert {idx.name for idx in relabelled.dummy_indices} == {"alpha"}
    assert relabelled.to_latex() == "x_{alpha}y_{alpha}"


@pytest.mark.unit
def test_relabel_distinct_pairs_take_names_in_order() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    z = symbolic.tensor("z", [space])
    w = symbolic.tensor("w", [space])
    i = space.index("i")
    j = space.index("j")
    relabelled = symbolic.relabel(x(i) * y(i) * z(j) * w(j))
    assert {idx.name for idx in relabelled.dummy_indices} == {"alpha", "beta"}


@pytest.mark.unit
def test_relabel_pool_wrap_around_adds_suffixes() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    tensors = [symbolic.tensor(f"t{k}", [space]) for k in range(5)]
    tensors_u = [symbolic.tensor(f"u{k}", [space]) for k in range(5)]
    expr = symbolic.Expr(1)
    for k in range(5):
        index = space.index(f"k{k}")
        expr = expr * tensors[k](index) * tensors_u[k](index)
    latex = symbolic.relabel(expr).to_latex()
    assert "alpha_{1}" in latex
    assert "alpha_{2}" in latex
    assert "alpha_{3}" not in latex


@pytest.mark.unit
def test_relabel_free_indices_take_names_before_dummies() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    z = symbolic.tensor("z", [space])
    p = space.index("p")
    f = space.index("f")
    relabelled = symbolic.relabel(x(p) * y(p) * z(f))
    assert relabelled.to_latex() == "x_{beta}y_{beta}z_{alpha}"


@pytest.mark.unit
def test_relabel_spaces_keep_their_own_sequences() -> None:
    lorentz = symbolic.Space("L", 4, index_names=("mu", "nu"))
    spatial = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    g = symbolic.tensor("g", [lorentz, lorentz])
    x = symbolic.tensor("x", [spatial])
    y = symbolic.tensor("y", [spatial])
    m = lorentz.index("m")
    n = lorentz.index("n")
    s = spatial.index("s")
    relabelled = symbolic.relabel(g(m, n) * g(m, n) * x(s) * y(s))
    names = {idx.name for idx in relabelled.dummy_indices}
    assert names == {"mu", "nu", "alpha"}


@pytest.mark.unit
def test_relabel_free_of_one_space_dummy_of_another() -> None:
    lorentz = symbolic.Space("L", 4, index_names=("mu", "nu"))
    spatial = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    g = symbolic.tensor("G", [lorentz, spatial, spatial])
    h = symbolic.tensor("H", [spatial, spatial])
    mu = lorentz.index("m")
    i = spatial.index("i")
    j = spatial.index("j")
    relabelled = symbolic.relabel(g(mu, i, j) * h(i, j))
    assert {idx.name for idx in relabelled.free_indices} == {"mu"}


@pytest.mark.unit
def test_relabel_resets_all_ids_to_zero() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    z = symbolic.tensor("z", [space])
    i = space.index("i")
    relabelled = symbolic.relabel(x(i) * y(i) * z(space.index("f")))
    assert all(idx.id == 0 for idx in relabelled.free_indices)
    assert all(idx.id == 0 for idx in relabelled.dummy_indices)


@pytest.mark.unit
def test_relabel_twice_gives_same_labels() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    once = symbolic.relabel(x(i) * y(i))
    assert symbolic.relabel(once).to_latex() == once.to_latex()


@pytest.mark.unit
def test_relabel_fixed_index_is_not_renamed() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    assert symbolic.relabel(x(space.fixed_index(1))).to_latex() == "x_{1}"


@pytest.mark.unit
def test_relabel_dummy_pairs_avoid_kept_free_names() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    free = space.index("alpha")
    i = space.index("i")
    relabelled, _renaming = symbolic.relabel_with_renaming(
        x(free) * y(i) * x(i), relabel_free=False
    )
    assert {idx.name for idx in relabelled.free_indices} == {"alpha"}
    assert {idx.name for idx in relabelled.dummy_indices} == {"beta"}


@pytest.mark.unit
def test_dummy_pairs_relabelled_in_order() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    j = space.index("j")
    k = space.index("k")
    expr = x(i) * y(i) * x(j) * y(j) * x(k) * y(k)
    relabelled = symbolic.relabel(expr)
    assert relabelled.to_latex() == (
        "x_{alpha}x_{beta}x_{alpha_{1}}y_{alpha}y_{beta}y_{alpha_{1}}"
    )


@pytest.mark.unit
def test_relabel_same_name_different_ids_are_different() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    free = space.index("f")
    dummy = space.index("p")
    latex = symbolic.relabel(x(free) * y(dummy) * x(dummy)).to_latex()
    assert "x_{alpha}" in latex
    assert "x_{beta}" in latex
    assert "y_{beta}" in latex


@pytest.mark.unit
def test_relabel_leaves_user_index_unchanged() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    before = (i.name, i.id, i.type)
    symbolic.relabel(x(i) * y(i))
    assert (i.name, i.id, i.type) == before


@pytest.mark.unit
def test_relabel_repeated_subexpression_stays_valid() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    sub = x(i) * y(i)
    assert symbolic.relabel(sub + sub) == 2 * symbolic.relabel(sub)


@pytest.mark.unit
def test_free_index_attributes() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    i = space.index("i")
    assert i.name == "i"
    assert i.type == symbolic.IndexType.Free
    assert i.space is space
    with pytest.raises(MartyTypeError):
        _ = i.value


@pytest.mark.unit
def test_fixed_index_attributes() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    fixed = space.fixed_index(2)
    assert fixed.type == symbolic.IndexType.Fixed
    assert fixed.value == 2
    assert fixed.space is space
    with pytest.raises(MartyTypeError):
        _ = fixed.name


@pytest.mark.unit
def test_contracted_index_is_dummy() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    expr = x(i) * y(i)
    assert expr.free_indices == ()
    (dummy,) = expr.dummy_indices
    assert dummy.name == "i"
    assert dummy.type == symbolic.IndexType.Dummy


@pytest.mark.unit
def test_renaming_pairs_map_dummies_to_new_names() -> None:
    space = symbolic.Space("S", 4, index_names=("alpha", "beta"))
    x = symbolic.tensor("x", [space])
    y = symbolic.tensor("y", [space])
    i = space.index("i")
    j = space.index("j")
    expr = x(i) * y(i) * x(j) * y(j)
    relabelled, renaming = symbolic.relabel_with_renaming(expr)
    assert [(old.name, new.name) for old, new in renaming.pairs] == [
        ("i", "alpha"),
        ("j", "beta"),
    ]
    assert all(old.type == symbolic.IndexType.Dummy for old, _ in renaming.pairs)
    assert relabelled.to_latex() == "x_{alpha}x_{beta}y_{alpha}y_{beta}"
