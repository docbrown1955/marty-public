import pytest

import pymarty
from pymarty import errors
from pymarty._pymarty import _debug

ERROR_KINDS = {
    "value": (errors.MartyValueError, ValueError),
    "type": (errors.MartyTypeError, TypeError),
    "lookup": (errors.MartyLookupError, LookupError),
    "not_implemented": (errors.MartyNotImplementedError, NotImplementedError),
    "state": (errors.MartyStateError, RuntimeError),
    "build": (errors.MartyBuildError, RuntimeError),
}

# Errors thrown the way the core does (enum or core exception), with the typed
# class they must be translated to.
CORE_KINDS = {
    "core_value": errors.MartyValueError,
    "core_key": errors.MartyLookupError,
    "core_type": errors.MartyTypeError,
    "core_not_implemented": errors.MartyNotImplementedError,
    "core_model_building": errors.MartyBuildError,
    "core_other": errors.MartyError,
    "csl_type": errors.MartyTypeError,
    "csl_value": errors.MartyValueError,
}


@pytest.mark.unit
@pytest.mark.parity("ErrorApi.HierarchyDerivesFromStdRuntimeError")
@pytest.mark.parametrize("kind", sorted(ERROR_KINDS))
def test_each_error_class_derives_from_marty_error_and_builtin(kind: str) -> None:
    cls, builtin = ERROR_KINDS[kind]
    assert issubclass(cls, errors.MartyError)
    assert issubclass(cls, builtin)


@pytest.mark.unit
@pytest.mark.parametrize("kind", sorted(ERROR_KINDS))
def test_raise_error_maps_kind_to_typed_error(kind: str) -> None:
    cls, builtin = ERROR_KINDS[kind]
    with pytest.raises(cls, match="boom") as info:
        _debug.raise_error(kind, "boom")
    assert isinstance(info.value, errors.MartyError)
    assert isinstance(info.value, builtin)


@pytest.mark.unit
def test_builtin_except_clause_catches_marty_error() -> None:
    with pytest.raises(ValueError):
        _debug.raise_error("value", "caught as builtin")


@pytest.mark.unit
def test_unknown_kind_is_a_marty_value_error() -> None:
    with pytest.raises(errors.MartyValueError, match="unknown error kind: nope"):
        _debug.raise_error("nope", "message")


@pytest.mark.unit
def test_module_stays_usable_after_an_error() -> None:
    with pytest.raises(errors.MartyStateError):
        _debug.raise_error("state", "first")
    with pytest.raises(errors.MartyValueError):
        _debug.raise_error("value", "second")


@pytest.mark.unit
@pytest.mark.parity("ErrorApi.CoreEnumErrorIsTranslatedWithMessage")
def test_core_error_is_translated_with_function_and_message() -> None:
    with pytest.raises(errors.MartyValueError) as info:
        _debug.raise_error("core_value", "bad value 42")
    assert "bad value 42" in str(info.value)
    assert "raiseError" in str(info.value)


@pytest.mark.unit
@pytest.mark.parity("ErrorApi.CoreTypesAreMappedToTypedErrors")
@pytest.mark.parametrize("kind", sorted(CORE_KINDS))
def test_core_types_are_mapped_to_typed_errors(kind: str) -> None:
    with pytest.raises(CORE_KINDS[kind]):
        _debug.raise_error(kind, "message")


@pytest.mark.unit
@pytest.mark.parity("ErrorApi.GammaApiErrorIsTranslated")
def test_gamma_api_error_is_translated() -> None:
    with pytest.raises(errors.MartyError, match="gamma failure"):
        _debug.raise_error("gamma", "gamma failure")


@pytest.mark.unit
@pytest.mark.parity("ErrorApi.NothingIsPrintedToStderrInQuietMode")
@pytest.mark.parametrize("kind", ["core_value", "csl_value", "gamma"])
def test_core_errors_print_nothing_to_stderr(
    kind: str, capfd: pytest.CaptureFixture[str]
) -> None:
    with pytest.raises(errors.MartyError):
        _debug.raise_error(kind, "silent")
    captured = capfd.readouterr()
    assert captured.err == ""
    assert captured.out == ""


@pytest.mark.unit
def test_version_is_exposed() -> None:
    assert isinstance(pymarty.__version__, str)
    assert pymarty.__version__
