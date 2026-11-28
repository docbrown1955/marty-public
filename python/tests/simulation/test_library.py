"""Tests of pymarty.simulation: generation, build and load of standalone libraries."""

import importlib.util
import inspect
import math
from pathlib import Path

import pytest

from pymarty import config, simulation
from pymarty._pymarty import simulation as _simulation
from pymarty.calculation import (
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.errors import MartyStateError, MartyValueError
from pymarty.modeling.models.sm import SM
from pymarty.modeling.models.zmumu import Zmumu
from pymarty.symbolic import ComplexProperty, constant


def test_invalid_library_name_is_rejected(tmp_path) -> None:
    with pytest.raises(MartyValueError):
        simulation.generate("1bad", {"f": constant("x")}, path=tmp_path)


def test_load_before_build_is_a_state_error(tmp_path) -> None:
    lib = simulation.generate("unbuilt", {"f": constant("x")}, path=tmp_path)
    with pytest.raises(MartyStateError):
        lib.load()


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_built_library_matches_symbolic_values(tmp_path) -> None:
    x, y = constant("x"), constant("y")
    lib = simulation.generate("toy", {"f": x * y + 2 * x}, path=tmp_path)
    lib.build(n_jobs=2)

    toy = lib.load()
    assert toy.f(x=2.0, y=3.0) == pytest.approx(10.0)
    # The generated binding is standalone: it raises ValueError, not MartyValueError.
    with pytest.raises(ValueError):
        toy.f(x=2.0)


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_squared_amplitude_expression_is_generated(tmp_path) -> None:
    model = Zmumu()
    legs = [incoming("e"), incoming(anti("e")), outgoing("mu"), outgoing(anti("mu"))]
    m2 = compute_squared_amplitude(model, compute_amplitude(model, Order.TREE, legs))

    lib = simulation.generate("m2_scalar", {"M2": m2}, path=tmp_path)
    lib.build(n_jobs=2)

    binding = lib.load()
    parameters = inspect.signature(binding.M2).parameters
    assert {"aL", "aR", "M_Z", "G_Z", "s_12"} <= set(parameters)
    assert isinstance(binding.M2(**{name: 1.0 for name in parameters}), complex)


@pytest.mark.needs_compiler
def test_complex_parameter_is_passed_as_complex(tmp_path) -> None:
    x = constant("x")
    z = constant("z", complex_property=ComplexProperty.Complex)
    lib = simulation.generate("complexwrap", {"f": x * z}, path=tmp_path)
    lib.build(n_jobs=2)
    binding = lib.load()
    assert binding.f(x=2.0, z=1.0 + 2.0j) == pytest.approx(2.0 + 4.0j)


def _load_binding(generator, name: str):
    binding = Path(generator.library_directory) / "src" / "py" / f"{name}.py"
    spec = importlib.util.spec_from_file_location(name, binding)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _sm_generator(tmp_path, name: str):
    generator = _simulation.Generator(name, str(tmp_path))
    generator.add_include_path(config.resolve().include_path)
    return generator


def test_spectrum_sources_are_written(tmp_path) -> None:
    # Diagonalization data is computed with the widths; without it, the spectrum
    # has no mass or diagonalization functions and cspectrum.c is not written.
    model = SM()
    generator = _sm_generator(tmp_path, "sm_spectrum")
    generator.compute_model_widths(model, Order.TREE)
    generator.add_spectrum(model)
    generator.print_sources()

    sources = Path(generator.library_directory) / "src" / "c"
    assert (sources / "cspectrum.c").is_file()
    assert (sources / "cdiagonalization.c").is_file()


def test_spectrum_can_be_added_only_once(tmp_path) -> None:
    generator = _sm_generator(tmp_path, "sm_twice")
    generator.add_spectrum(SM())
    with pytest.raises(MartyStateError):
        generator.add_spectrum(SM())


@pytest.mark.needs_compiler
@pytest.mark.slow
def test_model_widths_build_and_evaluate(tmp_path) -> None:
    library_path = config.resolve().library_path
    if not library_path:
        pytest.skip("no LoopTools library path configured")

    model = SM()
    generator = _sm_generator(tmp_path, "sm_widths")
    generator.add_library_path(library_path)
    generator.compute_model_widths(model, Order.TREE)
    generator.add_spectrum(model)
    generator.print_sources()
    generator.build(2, config.resolve().make)

    binding = _load_binding(generator, "sm_widths")
    parameters = inspect.signature(binding.Gamma_Z).parameters
    value = binding.Gamma_Z(**{name: 1.0 for name in parameters})
    assert isinstance(value, complex)
    assert math.isfinite(value.real)


@pytest.mark.needs_compiler
@pytest.mark.slow
@pytest.mark.quad
@pytest.mark.skipif(
    not config.Config.quad_available,
    reason="MARTY was built without quadruple precision",
)
def test_quadruple_option_is_written_to_makefile(tmp_path) -> None:
    x, y = constant("x"), constant("y")
    expr = x * y + 2 * x

    double_gen = _sm_generator(tmp_path, "toy_double")
    double_gen.add_function("f", expr)
    double_gen.print_sources()
    double_dir = Path(double_gen.library_directory)
    assert "QUAD ?= 0" in (double_dir / "Makefile").read_text()

    quad_gen = _sm_generator(tmp_path, "toy_quad")
    quad_gen.set_quadruple_precision(True)

    quad_gen.print_sources()
    quad_dir = Path(quad_gen.library_directory)
    assert "QUAD ?= 1" in (quad_dir / "Makefile").read_text()

    quad_gen.build(2, config.resolve().make)
    binding = _load_binding(quad_gen, "toy_quad")
    assert binding.f(x=2.0, y=3.0) == pytest.approx(10.0)
