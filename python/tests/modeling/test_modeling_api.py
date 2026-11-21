"""Python counterparts of the C++ ``ModelingApi`` tests."""

from fractions import Fraction

import pytest

from pymarty import models
from pymarty.errors import (
    MartyError,
    MartyLookupError,
    MartyStateError,
    MartyTypeError,
    MartyValueError,
)
from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model
from pymarty.symbolic import ComplexProperty, constant, tensor


def make_toy() -> Model:
    return Model(
        gauge=[
            GaugeGroup(SU(2), name="L", boson_name="W"),
            GaugeGroup(U(1), name="Y", boson_name="B"),
        ]
    )


@pytest.mark.parity("ModelingApi.UnknownParticleIsLookupError")
def test_unknown_particle_is_lookup_error() -> None:
    toy = make_toy()
    assert "no_such_particle" not in toy
    with pytest.raises(MartyLookupError):
        toy.particle("no_such_particle")


@pytest.mark.parity("ModelingApi.ChargeOnNonU1GroupIsTypeError")
def test_charge_on_non_u1_group_is_type_error() -> None:
    toy = make_toy()
    with pytest.raises(MartyTypeError):
        toy.add_weyl_fermion("Q_L", chirality="L", gauge_rep={"L": Fraction(1, 6)})
    toy.add_weyl_fermion("Q_L", chirality="L", gauge_rep={"Y": Fraction(1, 6)})


@pytest.mark.parity("ModelingApi.LabelsOnU1GroupIsTypeError")
def test_labels_on_u1_group_is_type_error() -> None:
    toy = make_toy()
    with pytest.raises(MartyTypeError):
        toy.add_weyl_fermion("Q_L", chirality="L", gauge_rep={"Y": [1]})


@pytest.mark.parity("ModelingApi.UnknownChiralityIsValueError")
def test_unknown_chirality_is_value_error() -> None:
    with pytest.raises(MartyValueError):
        make_toy().add_weyl_fermion("Q_L", chirality="X")


@pytest.mark.parity("ModelingApi.CallsRestoreCurrentModel")
def test_models_are_independent() -> None:
    toy = make_toy()
    other = make_toy()
    toy.add_weyl_fermion("Q_L", chirality="L")
    assert "Q_L" in toy
    assert "Q_L" not in other
    with pytest.raises(MartyLookupError):
        toy.particle("no_such_particle")
    assert "Q_L" in toy


@pytest.mark.parity("ModelingApi.RenameParticleChecksNames")
def test_rename_particle_checks_names() -> None:
    toy = make_toy()
    toy.add_weyl_fermion("Q_L", chirality="L")
    toy.add_dirac_fermion("E")
    with pytest.raises(MartyLookupError):
        toy.rename_particle("no_such_particle", "X")
    with pytest.raises(MartyValueError):
        toy.rename_particle("Q_L", "E")
    toy.rename_particle("Q_L", "Q")
    assert "Q" in toy
    assert "Q_L" not in toy
    assert toy.particle("Q").name == "Q"


@pytest.mark.parity("ModelingApi.FlavorGroupNeedsAFlavor")
def test_flavor_group_needs_a_flavor() -> None:
    gauge = [GaugeGroup(SU(2), name="L", boson_name="W")]
    with pytest.raises(MartyValueError):
        Model(gauge, flavor=[FlavorGroup("F", 0)])
    with pytest.raises(MartyValueError):
        Model(gauge, flavor=[FlavorGroup("L", 3)])
    Model(gauge, flavor=[FlavorGroup("F", 3)])


@pytest.mark.parity("ModelingApi.ParticleFactoriesAndRefresh")
def test_particle_factories_and_finalize() -> None:
    toy = make_toy()
    toy.add_scalar_boson("phi", self_conjugate=False)
    toy.add_majorana_fermion("chi")
    assert "phi" in toy
    assert "chi" in toy
    assert not toy.is_finalized()
    toy.finalize()
    assert toy.is_finalized()


@pytest.mark.parity("ModelingApi.CouplingIsCreatedThenFound")
def test_coupling_is_created_then_found() -> None:
    toy = make_toy()
    with pytest.raises(MartyLookupError):
        toy.coupling("g")
    created = toy.create_coupling("g")
    assert toy.coupling("g") == created


@pytest.mark.parity("ModelingApi.CustomGaugeCouplingNameIsUsed")
def test_custom_gauge_coupling_name_is_used() -> None:
    toy = Model(
        [
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_weak"),
            GaugeGroup(U(1), name="Y", boson_name="B"),
        ]
    )
    toy.coupling("g_weak")
    toy.coupling("g_Y")
    with pytest.raises(MartyLookupError):
        toy.coupling("g_L")


@pytest.mark.parity("ModelingApi.MassAndRotationRejectInconsistentInput")
def test_mass_and_rotation_reject_inconsistent_input() -> None:
    toy = make_toy()
    phi = toy.add_scalar_boson("phi", self_conjugate=False)
    qL = toy.add_weyl_fermion("Q_L", chirality="L")
    qR = toy.add_weyl_fermion("Q_R", chirality="R")
    toy.add_bosonic_mass(phi, toy.create_coupling("m"))
    toy.add_fermionic_mass(qL, qR, toy.create_coupling("y"))
    with pytest.raises(MartyError):
        toy.rotate_fields([phi], [phi, qL], [[]])


@pytest.mark.parity("ModelingApi.QedMatchesCoreParticlesAndRules")
def test_qed_matches_core_particles_and_rules() -> None:
    qed = models.QED()
    assert [p.name for p in qed.particles] == [
        "A",
        "c_A",
        "\\psi",
        "\\psi_L",
        "\\psi_R",
    ]
    assert len(qed.feynman_rules) == 1


@pytest.mark.parity("ModelingApi.StandardModelMatchesCoreParticlesAndRules")
def test_standard_model_matches_core_particles_and_rules() -> None:
    sm = models.SM()
    assert len(sm.particles) == 42
    assert len(sm.feynman_rules) == 190


@pytest.mark.parity("ModelingApi.LatexMatchesExistingTests")
def test_latex_matches_existing_tests() -> None:
    rules = models.QED().feynman_rules
    document = rules.latex_document()
    headers = [
        line for line in document.splitlines() if line.startswith("Feynman vertex for ")
    ]
    assert len(headers) == len(rules)
    assert rules.latex(0).count("\\vertex") == 9


@pytest.mark.parity("ModelingApi.FeynmanRuleIndexOutOfRangeIsLookupError")
def test_feynman_rule_index_out_of_range_is_lookup_error() -> None:
    rules = models.QED().feynman_rules
    with pytest.raises(MartyLookupError):
        rules[len(rules)]
    with pytest.raises(MartyLookupError):
        rules.latex(len(rules))
    assert rules[-1] == rules[0]


def make_flavored_model() -> Model:
    model = Model(
        gauge=[GaugeGroup(SU(2), name="L", boson_name="W")],
        flavor=[FlavorGroup("F", 2)],
    )
    model.add_weyl_fermion("Q", chirality="L", gauge_rep={"L": 1}, flavor_rep="F")
    model.add_scalar_boson("H", gauge_rep={"L": 1}, self_conjugate=False)
    return model


@pytest.mark.unit
def test_flavor_breaking_partial_renaming_keeps_default_names() -> None:
    model = make_flavored_model()
    model.break_flavor_symmetry("F", renaming={"Q": ["u_L", "d_L"]})
    names = {p.name for p in model.particles}
    assert {"u_L", "d_L", "H"} <= names
    assert "Q" not in names


@pytest.mark.unit
def test_flavor_breaking_default_names_and_unbroken_rename_is_value_error() -> None:
    model = make_flavored_model()
    with pytest.raises(MartyValueError):
        model.break_flavor_symmetry("F", renaming={"H": ["h1", "h2"]})
    model = make_flavored_model()
    model.break_flavor_symmetry("F")
    assert {"Q_1", "Q_2", "H"} <= {p.name for p in model.particles}


@pytest.mark.unit
def test_ghost_bosons_are_created_and_linked() -> None:
    toy = make_toy()
    boson = toy.particle("W")
    assert boson.ghost_boson().name == "c_W"
    conjugated = toy.create_ghost_boson("c_Wm", boson, conjugated=True)
    assert conjugated.name == "c_Wm"
    boson.set_conjugated_ghost_boson(conjugated)


@pytest.mark.unit
def test_particle_without_ghost_is_type_error() -> None:
    toy = make_toy()
    fermion = toy.add_weyl_fermion("F", chirality="L")
    with pytest.raises(MartyTypeError):
        fermion.ghost_boson()


@pytest.mark.unit
def test_dirac_builder_attaches_particle_with_mass_and_reps() -> None:
    toy = make_toy()
    toy.add_dirac_fermion(
        "psi",
        gauge_rep={"L": 1, "Y": Fraction(1, 6)},
        mass=constant("m_psi"),
        self_conjugate=False,
    )
    assert "psi" in toy


@pytest.mark.unit
def test_builder_rejects_unknown_group_in_gauge_rep() -> None:
    toy = make_toy()
    with pytest.raises(MartyLookupError):
        toy.add_weyl_fermion("Q", gauge_rep={"NoSuchGroup": 1})
    assert "Q" not in toy


@pytest.mark.unit
def test_create_similar_particle_attaches_to_model() -> None:
    toy = make_toy()
    toy.add_scalar_boson("phi", gauge_rep={"L": 1})
    toy.create_similar_particle("phi2", reference="phi", self_conjugate=False)
    assert {"phi", "phi2"} <= {p.name for p in toy.particles}


@pytest.mark.unit
def test_is_self_conjugate_reflects_builder_flags() -> None:
    toy = make_toy()
    real = toy.add_scalar_boson("phi", gauge_rep={"L": 1}, self_conjugate=True)
    complex_ = toy.add_scalar_boson("chi", gauge_rep={"L": 1}, self_conjugate=False)
    assert real.is_self_conjugate()
    assert not complex_.is_self_conjugate()
    assert not toy.create_similar_particle(
        "chi2", reference="chi", self_conjugate=False
    ).is_self_conjugate()


@pytest.mark.unit
def test_finalize_freezes_model_and_is_idempotent() -> None:
    toy = make_toy()
    assert not toy.is_finalized()
    toy.finalize()
    toy.finalize()
    assert toy.is_finalized()
    with pytest.raises(MartyStateError):
        toy.add_scalar_boson("phi", gauge_rep={"L": 1})
    with pytest.raises(MartyStateError):
        toy.break_gauge_symmetry("Y")


@pytest.mark.unit
def test_finalized_model_still_allows_reads() -> None:
    toy = make_toy()
    toy.add_scalar_boson("phi", gauge_rep={"L": 1})
    toy.finalize()
    assert "phi" in toy


@pytest.mark.unit
def test_vector_space_builds_tensor_coupling() -> None:
    toy = Model(gauge=[], flavor=[FlavorGroup("F", 2)])
    space = toy.vector_space("F")
    yukawa = tensor("Y", [space, space], complex_property=ComplexProperty.Complex)
    toy.add_tensor_coupling(yukawa)
    toy.finalize()
    assert toy.is_finalized()


def test_build_ckm_unknown_flavor_is_lookup_error() -> None:
    with pytest.raises(MartyLookupError):
        make_toy().build_ckm("no_such_flavor")
