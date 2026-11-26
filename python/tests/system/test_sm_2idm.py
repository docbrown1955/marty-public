"""Port of tests/system/src/SM_2IDM.cpp: one-loop mu -> e Z amplitude of the 2IDM.

The symbolic amplitude is checked. The C++ test has no squared amplitude and no
numerical reference (it only checks that the binary is built). The slow test
generates, builds and loads a library for the squared one-loop mu -> e Z
amplitude (build and load only, no numbers: its Passarino-Veltman coefficients
cannot be evaluated symbolically), and a library for the tree-level squared
amplitude hh -> e e+, which is checked at fixed complex couplings against the
symbolic evaluation (CSL consistency, relative 1e-10). The coupling values are
chosen by the test, not taken from the C++ example.
"""

import pytest

from pymarty import config
from pymarty._pymarty import simulation as _simulation
from pymarty.calculation import (
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
    set_options,
)
from pymarty.modeling.models.sm_2idm import SM2IDM
from pymarty.simulation import Library


@pytest.fixture(scope="module")
def model() -> SM2IDM:
    return SM2IDM()


def _mu_to_e_z_squared(model: SM2IDM):
    with set_options(
        decompose_in_operators=False,
        decompose_in_local_operator=False,
        exclude_tadpoles=True,
        exclude_mass_corrections=True,
        exclude_boxes=True,
    ):
        amplitude = compute_amplitude(
            model,
            Order.ONE_LOOP,
            [incoming("mu"), outgoing("e"), outgoing("Z")],
        )
        return amplitude, compute_squared_amplitude(model, amplitude)


@pytest.mark.loops
def test_mu_to_e_z_one_loop_amplitude(model: SM2IDM) -> None:
    amplitude, _ = _mu_to_e_z_squared(model)
    assert amplitude.diagrams


@pytest.mark.needs_compiler
@pytest.mark.slow
@pytest.mark.loops
def test_mu_to_e_z_squared_generate_build_and_load(
    model: SM2IDM, tmp_path_factory, assert_library_matches_symbolic
) -> None:
    _, squared = _mu_to_e_z_squared(model)
    tree = compute_squared_amplitude(
        model,
        compute_amplitude(
            model, Order.TREE, [incoming("hh"), outgoing("e"), outgoing(anti("e"))]
        ),
    )
    generator = _simulation.Generator("sm_2idm", str(tmp_path_factory.mktemp("sm2IDM")))
    generator.add_include_path(config.resolve().include_path)
    generator.add_library_path(config.resolve().library_path)
    generator.add_function("squared_mu_e_z", squared)
    generator.add_function("squared_hh_e_e", tree)
    generator.print_sources()
    lib = Library("sm_2idm", generator)
    lib.build(n_jobs=4)
    module = lib.load()
    assert (lib.directory / "src" / "c" / "c_squared_mu_e_z.c").is_file()
    assert_library_matches_symbolic(module, "squared_hh_e_e", tree, complex_values=True)
