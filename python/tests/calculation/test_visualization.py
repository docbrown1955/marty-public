"""Tests for ``pymarty.visualization``: diagram export, LaTeX and GRAFED."""

import os
import stat
from pathlib import Path

import pytest

from pymarty.calculation import (
    Amplitude,
    Order,
    compute_amplitude,
    compute_feynman_rules,
    incoming,
    outgoing,
)
from pymarty.errors import MartyStateError, MartyTypeError, MartyValueError
from pymarty.modeling.models.qed import QED
from pymarty.visualization import export_json, grafed, latex, render_png, save, show

PSI = "\\psi"


@pytest.fixture(scope="module")
def compton() -> Amplitude:
    model = QED()
    return compute_amplitude(
        model,
        Order.TREE,
        [incoming(PSI), incoming("A"), outgoing(PSI), outgoing("A")],
    )


def _fake_grafed(directory: Path, body: str) -> Path:
    script = directory / "grafed-gui"
    script.write_text("#!/bin/sh\n" + body + "\n")
    script.chmod(0o755)
    return script


def test_saved_diagram_file_is_readable_by_the_owner_only(
    compton: Amplitude, tmp_path: Path
) -> None:
    target = tmp_path / "diagrams.json"
    save(compton, str(target))
    assert '"diagrams"' in target.read_text()
    assert stat.S_IMODE(os.stat(target).st_mode) == 0o600


def test_latex_is_a_standalone_tikz_document(compton: Amplitude) -> None:
    document = latex(compton)
    assert "\\documentclass" in document
    assert "tikzpicture" in document


def test_feynman_rules_have_a_latex_document() -> None:
    document = latex(compute_feynman_rules(QED()))
    assert "\\documentclass" in document


def test_export_refuses_non_amplitudes() -> None:
    with pytest.raises(MartyTypeError):
        export_json("not an amplitude")  # type: ignore[arg-type]


def test_explicit_grafed_path_must_be_executable(tmp_path: Path) -> None:
    not_executable = tmp_path / "grafed-gui"
    not_executable.write_text("")
    with pytest.raises(MartyValueError):
        grafed.find(not_executable)


def test_grafed_is_found_from_the_environment(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    script = _fake_grafed(tmp_path, "exit 0")
    monkeypatch.setenv("MARTY_GRAFED", str(script))
    assert grafed.find() == str(script)
    assert grafed.is_available()


def test_show_passes_the_diagram_file_and_returns_the_exit_status(
    compton: Amplitude, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    # The fake viewer exits 3 only when it receives a non-empty file.
    script = _fake_grafed(tmp_path, '[ -s "$1" ] && exit 3\nexit 4')
    monkeypatch.setenv("MARTY_GRAFED", str(script))
    assert show(compton, wait=True) == 3


def test_show_without_grafed_is_a_state_error(
    compton: Amplitude, monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    monkeypatch.delenv("MARTY_GRAFED", raising=False)
    monkeypatch.setenv("PATH", str(tmp_path))
    with pytest.raises(MartyStateError):
        show(compton)


def test_render_png_without_latex_is_a_state_error(
    compton: Amplitude, monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    monkeypatch.setenv("PATH", str(tmp_path))
    with pytest.raises(MartyStateError):
        render_png(compton)
