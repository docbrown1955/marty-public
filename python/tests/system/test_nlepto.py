"""Port of tests/system/src/NLepto.cpp: squared amplitudes of the NLepto toy model.

Only the symbolic amplitudes and squared amplitudes are computed here. The
library generation and the comparison with reference results belong to
phase 6.
"""

from fractions import Fraction

import pytest

from pymarty.calculation import (
    Insertion,
    Order,
    anti,
    compute_amplitude,
    compute_squared_amplitude,
    incoming,
    outgoing,
)
from pymarty.modeling import Particle
from pymarty.modeling.models.nlepto import NLepto
from pymarty.symbolic import Expr


@pytest.fixture(scope="module")
def model() -> NLepto:
    return NLepto()


def _physical_fermions(model: NLepto, prefix: str) -> list[Particle]:
    # Dirac N_k split into Weyl halves N_k_L/N_k_R: keep the full states only.
    return [
        p
        for p in model.particles
        if p.spin == Fraction(1, 2)
        and p.name.startswith(prefix)
        and not p.name.endswith(("_L", "_R"))
    ]


def _squared_amplitude(model: NLepto, legs: list[Insertion]) -> Expr:
    amplitude = compute_amplitude(model, Order.TREE, legs)
    assert amplitude.diagrams
    return compute_squared_amplitude(model, amplitude)


def test_physical_fermion_content(model: NLepto) -> None:
    neutrals = [p.name for p in _physical_fermions(model, "N")]
    charged = [p.name for p in _physical_fermions(model, "L")]
    assert neutrals == ["N1", "N2", "N3", "N4", "N5", "N6"]
    assert charged == ["L_e", "L_m", "L_t"]


def test_scalar_portal_squared_amplitude(model: NLepto) -> None:
    legs = [
        incoming("ss"),
        incoming(anti("ss")),
        outgoing("H"),
        outgoing(anti("H")),
    ]
    assert isinstance(_squared_amplitude(model, legs), Expr)


def test_heavy_neutrino_decays(model: NLepto) -> None:
    N = _physical_fermions(model, "N")[0]
    L = _physical_fermions(model, "L")[0]
    assert isinstance(
        _squared_amplitude(model, [incoming(N), outgoing("X"), outgoing(anti("ss"))]),
        Expr,
    )
    assert isinstance(
        _squared_amplitude(model, [incoming(N), outgoing(L), outgoing(anti("H"))]),
        Expr,
    )


@pytest.mark.slow
def test_all_processes_of_nlepto(model: NLepto) -> None:
    """Every process of NLepto.cpp, with the same loops as the C++ main."""
    N = _physical_fermions(model, "N")
    L = _physical_fermions(model, "L")
    processes: list[list[Insertion]] = [
        [incoming("ss"), incoming(anti("ss")), outgoing("H"), outgoing(anti("H"))]
    ]
    for Ni in N:
        processes.append([incoming(Ni), outgoing("X"), outgoing(anti("ss"))])
        for Lj in L:
            processes.append([incoming(Ni), outgoing(Lj), outgoing(anti("H"))])
    for Li in L:
        processes.append(
            [incoming("ss"), outgoing("X"), outgoing(Li), outgoing(anti("H"))]
        )
        processes.append(
            [incoming("ss"), outgoing("X"), outgoing(anti(Li)), outgoing("H")]
        )
        processes.append(
            [
                incoming(anti("ss")),
                outgoing(anti("X")),
                outgoing(anti(Li)),
                outgoing(anti("H")),
            ]
        )
        processes.append(
            [
                incoming(anti("ss")),
                outgoing(anti("X")),
                outgoing(anti(Li)),
                outgoing("H"),
            ]
        )
    for i, Ni in enumerate(N):
        for Nj in N[i:]:
            processes.append(
                [
                    incoming("X"),
                    incoming(anti("X")),
                    outgoing(Ni),
                    outgoing(anti(Nj)),
                ]
            )
    for legs in processes:
        assert isinstance(_squared_amplitude(model, legs), Expr)
