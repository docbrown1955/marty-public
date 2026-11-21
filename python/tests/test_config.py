from pathlib import Path

import pytest

import pymarty
from pymarty import config, errors


def make_package_data(root: Path) -> Path:
    (root / "include").mkdir(parents=True)
    (root / "include" / "marty.h").write_text("")
    (root / "lib").mkdir()
    (root / "lib" / "libmarty.so").write_text("")
    return root


def make_program(directory: Path, name: str) -> Path:
    directory.mkdir(parents=True, exist_ok=True)
    program = directory / name
    program.write_text("#!/bin/sh\n")
    program.chmod(0o755)
    return program


@pytest.mark.unit
@pytest.mark.parity("ConfigApi.EnvironmentOverridesPackageData")
def test_explicit_beats_environment_beats_package_data(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    package = make_package_data(tmp_path / "package")
    monkeypatch.delenv("MARTY_INCLUDE_PATH", raising=False)
    monkeypatch.delenv("MARTY_LIBRARY_PATH", raising=False)
    from_package = pymarty._pymarty.Config(package_data=str(package))
    assert from_package.include_path == str(package / "include")
    assert from_package.library_path == str(package / "lib")

    monkeypatch.setenv("MARTY_INCLUDE_PATH", "/from/environment")
    from_env = pymarty._pymarty.Config(package_data=str(package))
    assert from_env.include_path == "/from/environment"

    explicit = pymarty._pymarty.Config(
        include_path="/explicit", package_data=str(package)
    )
    assert explicit.include_path == "/explicit"


@pytest.mark.unit
@pytest.mark.parity("ConfigApi.EnvironmentCompilerOverridesDefaultsAndCxxVariable")
def test_marty_cxx_beats_cxx_beats_path_search(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    marty = make_program(tmp_path / "bin", "marty-g++")
    other = make_program(tmp_path / "bin", "other-g++")
    monkeypatch.setenv("PATH", str(tmp_path / "bin"))
    monkeypatch.setenv("CXX", "other-g++")
    monkeypatch.setenv("MARTY_CXX", "marty-g++")
    assert config.resolve().cxx == str(marty)
    monkeypatch.delenv("MARTY_CXX")
    assert config.resolve().cxx == str(other)


@pytest.mark.unit
@pytest.mark.parity("ConfigApi.MissingCompilerReportsBuildError")
def test_missing_compiler_raises_build_error(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    monkeypatch.setenv("PATH", str(tmp_path))
    monkeypatch.delenv("MARTY_CXX", raising=False)
    monkeypatch.delenv("CXX", raising=False)
    with pytest.raises(errors.MartyBuildError):
        _ = config.resolve().cxx
    monkeypatch.setenv("MARTY_CXX", "no-such-compiler")
    with pytest.raises(errors.MartyBuildError, match="no-such-compiler"):
        _ = config.resolve().cxx


@pytest.mark.unit
@pytest.mark.parity("ConfigApi.NoShellInvolved")
def test_hostile_names_are_data(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    monkeypatch.chdir(tmp_path)
    hostile = "evil; touch PWNED #$(touch PWNED)`touch PWNED`"
    program = make_program(tmp_path / "bin", hostile)
    monkeypatch.setenv("PATH", str(tmp_path / "bin"))
    monkeypatch.delenv("CXX", raising=False)
    monkeypatch.setenv("MARTY_CXX", hostile)
    assert config.resolve().cxx == str(program)
    monkeypatch.setenv("MARTY_CXX", "x; touch PWNED")
    with pytest.raises(errors.MartyBuildError):
        _ = config.resolve().cxx
    assert not (tmp_path / "PWNED").exists()
