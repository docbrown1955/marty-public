"""Port of tests/system/src/SM_triplet.cpp: amplitudes of the scalar-triplet model.

The symbolic amplitudes and squared amplitudes are checked on every process.
The C++ test has no numerical reference (it only checks that the binary is
built) and sets no numerical couplings. The slow test generates, builds and
loads the libraries for the two one-loop squared amplitudes of the C++ example,
plus the tree-level squared amplitude N_1 -> e eta^+ (one-loop Passarino-Veltman
coefficients cannot be evaluated symbolically). The tree-level function is then
checked at fixed complex couplings against the symbolic evaluation (CSL
consistency, relative 1e-10). The coupling values are chosen by the test, not
taken from the C++ example.
"""

import pytest

from pymarty import config
from pymarty._pymarty import simulation as _simulation
from pymarty.calculation import (
    Insertion,
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.modeling.models.sm_triplet import SMTriplet
from pymarty.simulation import Library
from pymarty.symbolic import Expr


@pytest.fixture(scope="module")
def model() -> SMTriplet:
    return SMTriplet()


@pytest.mark.parametrize(
    "legs",
    [
        [incoming("N_1"), outgoing("e"), outgoing(anti("\\eta^-"))],
        [incoming("Z"), outgoing("nu_e"), outgoing("nu_e")],
        [incoming("\\Delta^{--}"), outgoing("e"), outgoing("\\mu")],
        [incoming("\\Delta^{--}"), outgoing("\\eta^-"), outgoing("\\eta^-")],
        [incoming("\\Delta^{--}"), outgoing("e"), outgoing("e")],
        [incoming("\\eta^-"), incoming("\\eta^-"), outgoing("e"), outgoing("e")],
    ],
    ids=["N1_e_etaP", "Z_nu_nu", "Dmm_e_mu", "Dmm_eta_eta", "Dmm_e_e", "eta_eta_e_e"],
)
def test_amplitude_and_squared_amplitude_are_symbolic(
    model: SMTriplet, legs: list[Insertion]
) -> None:
    amplitude = compute_amplitude(model, Order.ONE_LOOP, legs)
    assert amplitude.diagrams
    assert isinstance(compute_squared_amplitude(model, amplitude), Expr)


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_squared_amplitudes_generate_build_and_load(
    model: SMTriplet, tmp_path_factory, assert_library_matches_symbolic
) -> None:
    squared = {
        "squared_ampl0": compute_squared_amplitude(
            model,
            compute_amplitude(
                model,
                Order.ONE_LOOP,
                [incoming("N_1"), outgoing("e"), outgoing(anti("\\eta^-"))],
            ),
        ),
        "squared_ampl1": compute_squared_amplitude(
            model,
            compute_amplitude(
                model,
                Order.ONE_LOOP,
                [incoming("Z"), outgoing("nu_e"), outgoing("nu_e")],
            ),
        ),
    }
    tree = compute_squared_amplitude(
        model,
        compute_amplitude(
            model,
            Order.TREE,
            [incoming("N_1"), outgoing("e"), outgoing(anti("\\eta^-"))],
        ),
    )
    generator = _simulation.Generator(
        "sm_triplet", str(tmp_path_factory.mktemp("smTriplet"))
    )
    generator.add_include_path(config.resolve().include_path)
    generator.add_library_path(config.resolve().library_path)
    for name, expr in {**squared, "squared_tree_N1": tree}.items():
        generator.add_function(name, expr)
    generator.print_sources()
    lib = Library("sm_triplet", generator)
    lib.build(n_jobs=4)
    module = lib.load()
    for name in squared:
        assert (lib.directory / "src" / "c" / f"c_{name}.c").is_file()
    assert_library_matches_symbolic(
        module, "squared_tree_N1", tree, complex_values=True
    )
