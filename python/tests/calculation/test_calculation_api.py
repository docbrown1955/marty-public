"""Python tests for the tree-level calculation API, ``pymarty.calculation``."""

import pytest

from pymarty._pymarty import calculation as core
from pymarty.calculation import (
    FeynOptions,
    Insertion,
    Order,
    WilsonSet,
    anti,
    chromo_magnetic_coefficient,
    chromo_magnetic_operator,
    compute_amplitude,
    compute_feynman_rules,
    compute_squared_amplitude,
    compute_width,
    incoming,
    outgoing,
    set_options,
    wilson_coefficients,
)
from pymarty.errors import (
    MartyLookupError,
    MartyStateError,
    MartyTypeError,
    MartyValueError,
)
from pymarty.group_theory import U
from pymarty.modeling import GaugeGroup, Model
from pymarty.modeling.models.qcd import QCD
from pymarty.modeling.models.qed import QED
from pymarty.symbolic import Expr, constant

PSI = "\\psi"


@pytest.fixture(scope="module")
def qed() -> QED:
    return QED()


@pytest.fixture(scope="module")
def qcd() -> QCD:
    return QCD()


def _compton() -> list[Insertion]:
    return [incoming(PSI), incoming("A"), outgoing(PSI), outgoing("A")]


def test_unfinalized_model_is_refused() -> None:
    open_model = QED(finalize=False)
    with pytest.raises(MartyStateError):
        compute_feynman_rules(open_model)
    with pytest.raises(MartyStateError):
        compute_amplitude(open_model, Order.TREE, _compton())


def test_one_loop_adds_diagrams_to_tree_level(qed: QED) -> None:
    tree = compute_amplitude(qed, Order.TREE, _compton())
    loop = compute_amplitude(qed, Order.ONE_LOOP, _compton())
    assert len(tree.diagrams) > 0
    assert len(loop.diagrams) > len(tree.diagrams)


def test_topology_name_is_case_insensitive(qed: QED) -> None:
    upper = compute_amplitude(
        qed, Order.TREE, _compton(), options=FeynOptions(topology="BOX")
    )
    lower = compute_amplitude(
        qed, Order.TREE, _compton(), options=FeynOptions(topology="box")
    )
    assert len(upper.diagrams) == len(lower.diagrams) > 0


@pytest.mark.parity("ModelingApi.FeynmanRuleIndexOutOfRangeIsLookupError")
def test_feynman_rule_index_out_of_range_is_lookup_error(qed: QED) -> None:
    rules = compute_feynman_rules(qed)
    with pytest.raises(MartyLookupError):
        rules.str(len(rules))


@pytest.mark.parity("ModelingApi.FeynmanRuleVertexGivesFieldNamesAndExpression")
def test_feynman_rule_vertex_lists_its_field_names(qed: QED) -> None:
    rules = compute_feynman_rules(qed)
    names = rules.vertex_particles(0)
    assert names
    assert set(names) <= {particle.name for particle in qed.particles}
    assert str(rules.vertex_expression(0))


def test_unknown_particle_name_suggests_close_matches(qed: QED) -> None:
    with pytest.raises(MartyLookupError) as info:
        compute_amplitude(qed, Order.TREE, [incoming("\\psii"), outgoing(PSI)])
    message = str(info.value)
    assert "did you mean" in message
    assert PSI in message.split("did you mean", 1)[1]


def test_set_options_restores_flags_after_an_error() -> None:
    before = core.get_option("simplify_amplitudes")
    with pytest.raises(RuntimeError):
        with set_options(simplify_amplitudes=not before):
            raise RuntimeError("failure inside the block")
    assert core.get_option("simplify_amplitudes") == before


def test_set_options_rejects_unknown_names_and_non_bools() -> None:
    before = core.get_option("simplify_amplitudes")
    with pytest.raises(MartyValueError):
        with set_options(no_such_flag=True):
            pass
    with pytest.raises(MartyTypeError):
        with set_options(simplify_amplitudes="yes"):  # type: ignore[arg-type]
            pass
    assert core.get_option("simplify_amplitudes") == before


def test_kinematics_indices_name_the_momenta(qed: QED) -> None:
    default = compute_amplitude(qed, Order.TREE, _compton()).display()
    indexed = compute_amplitude(qed, Order.TREE, _compton(), kinematics=[3, 4, 5, 6])
    assert "p_1" in default
    assert "p_3" in indexed.display()
    assert "p_1" not in indexed.display()


@pytest.mark.parametrize("indices", [[1, 2], [1, 1, 2, 3], [0, 1, 2, 3]])
def test_kinematics_indices_must_be_one_distinct_positive_per_leg(
    qed: QED, indices: list[int]
) -> None:
    with pytest.raises(MartyValueError):
        compute_amplitude(qed, Order.TREE, _compton(), kinematics=indices)


def test_fermion_order_must_be_a_permutation(qed: QED) -> None:
    with pytest.raises(MartyValueError):
        compute_amplitude(
            qed, Order.TREE, _compton(), options=FeynOptions(fermion_order=[0, 0, 2, 3])
        )


def test_fermion_order_permutation_is_accepted(qed: QED) -> None:
    amplitude = compute_amplitude(
        qed, Order.TREE, _compton(), options=FeynOptions(fermion_order=[1, 0, 2, 3])
    )
    assert len(amplitude.diagrams) == 2


def test_interference_term_of_two_amplitudes(qed: QED) -> None:
    left = compute_amplitude(qed, Order.TREE, _compton())
    right = compute_amplitude(qed, Order.TREE, _compton(), kinematics=[3, 4, 5, 6])
    assert compute_squared_amplitude(qed, left, right) is not None


def test_scalar_qed_phi_phi_to_two_photons_has_three_tree_diagrams() -> None:
    # Port of examples/sampleSQED.cpp: complex scalar of charge -1 in U(1).
    model = Model(gauge=[GaugeGroup(U(1), name="em", boson_name="A", coupling="e")])
    model.add_scalar_boson(
        "\\phi",
        gauge_rep={"em": -1},
        mass=constant("m"),
        self_conjugate=False,
    )
    model.finalize()
    amplitude = compute_amplitude(
        model,
        Order.TREE,
        [incoming("\\phi"), incoming(anti("\\phi")), outgoing("A"), outgoing("A")],
    )
    assert len(amplitude.diagrams) == 3
    assert all(d.particles.count("A") == 1 for d in amplitude.diagrams)


def test_width_of_massless_particle_is_zero(qed: QED) -> None:
    assert compute_width(qed, "A") == 0


def test_width_of_unknown_particle_is_lookup_error(qed: QED) -> None:
    with pytest.raises(MartyLookupError, match='no particle named "photon"'):
        compute_width(qed, "photon")


def test_higgs_width_is_nonzero_expression() -> None:
    from pymarty.modeling.models.sm import SM

    width = compute_width(SM(), "h")
    assert isinstance(width, Expr)
    assert width != 0


@pytest.mark.slow
def test_wilson_coefficients_of_one_loop_amplitude_return_a_set(qcd: QCD) -> None:
    amplitude = compute_amplitude(
        qcd,
        Order.ONE_LOOP,
        [incoming("u"), incoming("G"), outgoing("u"), outgoing("G")],
    )
    wilsons = wilson_coefficients(qcd, amplitude)
    assert isinstance(wilsons, WilsonSet)
    assert isinstance(chromo_magnetic_operator(qcd, wilsons, "T"), list)


@pytest.mark.slow
def test_unknown_dirac_coupling_is_value_error(qcd: QCD) -> None:
    wilsons = wilson_coefficients(
        qcd,
        compute_amplitude(
            qcd,
            Order.ONE_LOOP,
            [incoming("u"), incoming("G"), outgoing("u"), outgoing("G")],
        ),
    )
    with pytest.raises(MartyValueError):
        chromo_magnetic_operator(qcd, wilsons, "XX")
    with pytest.raises(MartyValueError):
        chromo_magnetic_coefficient(qcd, wilsons, "XX")
