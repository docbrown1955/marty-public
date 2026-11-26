"""Tree-level calculation: Feynman rules, amplitudes and squared amplitudes.

The calculation takes a finalized model as its first argument; ``Model`` itself
gains no calculation methods. ``compute_amplitude`` takes insertions built with
``incoming``, ``outgoing``, ``anti``, ``left`` and ``right``, and an optional
``FeynOptions`` made of a topology and particle filters. ``select_diagrams``
applies a Python predicate to the diagrams of an amplitude. ``set_options``
sets the global MARTY flags for the duration of a ``with`` block.
"""

from __future__ import annotations

import contextlib
import dataclasses
import difflib
from collections.abc import Callable, Iterator, Sequence
from typing import Any

from pymarty._pymarty import Expr
from pymarty._pymarty import calculation as _core
from pymarty._pymarty import modeling as _modeling
from pymarty.errors import (
    MartyLookupError,
    MartyStateError,
    MartyTypeError,
)

Order = _core.Order
FeynmanRules = _core.FeynmanRules

ParticleRef = Any


@dataclasses.dataclass(frozen=True)
class Field:
    """A particle, optionally antiparticle or Weyl component, to be inserted.

    Build it with ``anti``, ``left`` or ``right`` rather than directly.
    """

    particle: ParticleRef
    antiparticle: bool = False
    chirality: str = "dirac"


@dataclasses.dataclass(frozen=True)
class Insertion:
    """External leg of an amplitude: a field, incoming or outgoing.

    Build it with ``incoming`` or ``outgoing``.
    """

    field: Field
    incoming: bool


def _as_field(value: object) -> Field:
    if isinstance(value, Field):
        return value
    if isinstance(value, (str, _modeling.Particle)):
        return Field(value)
    raise MartyTypeError(
        "an insertion takes a particle name, a modeling.Particle or a Field, "
        f"not {type(value).__name__}"
    )


def incoming(particle: ParticleRef) -> Insertion:
    """Incoming external leg of a particle.

    >>> from pymarty.calculation import incoming
    >>> incoming("e").incoming
    True
    """
    return Insertion(_as_field(particle), incoming=True)


def outgoing(particle: ParticleRef) -> Insertion:
    """Outgoing external leg of a particle.

    >>> from pymarty.calculation import outgoing
    >>> outgoing("e").incoming
    False
    """
    return Insertion(_as_field(particle), incoming=False)


def anti(particle: ParticleRef) -> Field:
    """Antiparticle of a particle, e.g. the positron for ``anti("e")``.

    >>> from pymarty.calculation import anti
    >>> anti("e").antiparticle
    True
    """
    return Field(particle, antiparticle=True)


def left(particle: ParticleRef) -> Field:
    """Left-handed Weyl component of a fermion.

    >>> from pymarty.calculation import left
    >>> left("e").chirality
    'left'
    """
    return Field(particle, chirality="left")


def right(particle: ParticleRef) -> Field:
    """Right-handed Weyl component of a fermion.

    >>> from pymarty.calculation import right
    >>> right("e").chirality
    'right'
    """
    return Field(particle, chirality="right")


@dataclasses.dataclass(frozen=True)
class ParticleFilter:
    """Filter on the particles of the diagrams or of the interactions.

    ``kind`` is ``"force"`` (diagrams must contain the particles), ``"forbid"``
    (diagrams must not contain them) or ``"disable"`` (interactions with them
    are removed before the diagrams are built).
    """

    kind: str
    particles: tuple[ParticleRef, ...]


def force_particle(*particles: ParticleRef) -> ParticleFilter:
    """Keeps only the diagrams containing all these particles.

    >>> from pymarty.calculation import force_particle
    >>> force_particle("e").kind
    'force'
    """
    return ParticleFilter("force", tuple(particles))


def forbid_particles(*particles: ParticleRef) -> ParticleFilter:
    """Removes the diagrams containing any of these particles.

    >>> from pymarty.calculation import forbid_particles
    >>> forbid_particles("A").kind
    'forbid'
    """
    return ParticleFilter("forbid", tuple(particles))


def disable_particles(*particles: ParticleRef) -> ParticleFilter:
    """Removes the interactions of these particles before the diagrams are built.

    >>> from pymarty.calculation import disable_particles
    >>> disable_particles("A").kind
    'disable'
    """
    return ParticleFilter("disable", tuple(particles))


@dataclasses.dataclass(frozen=True)
class FeynOptions:
    """Options of an amplitude: a topology, particle filters and fermion order.

    ``topology`` is a name such as ``"box"`` or several names separated by
    ``"|"``; ``None`` means any topology. ``fermion_order`` is a permutation of
    the 0-based leg positions; empty keeps the default order.
    """

    topology: str | None = None
    filters: Sequence[ParticleFilter] = ()
    fermion_order: Sequence[int] = ()


def _particles_of(filters: Sequence[ParticleFilter], kind: str) -> list[Any]:
    return [
        particle for item in filters if item.kind == kind for particle in item.particles
    ]


def _check_finalized(model: Any) -> None:
    if not model.is_finalized():
        raise MartyStateError("the model must be finalized before a calculation")


def _check_particle_names(model: Any, names: Sequence[Any]) -> None:
    """Raises MartyLookupError for a name that is not a particle, with close matches."""
    known = [particle.name for particle in model.particles]
    for name in names:
        if not isinstance(name, str) or name in known:
            continue
        close = difflib.get_close_matches(name, known, n=3)
        hint = f"; did you mean {', '.join(close)}?" if close else ""
        raise MartyLookupError(f'no particle named "{name}"{hint}')


class Diagram:
    """One Feynman diagram of an amplitude.

    ``particles`` is the tuple of the particle names it contains and
    ``expression`` its symbolic value.
    """

    def __init__(self, native: _core.Amplitude, index: int) -> None:
        self._native = native
        self._index = index

    @property
    def index(self) -> int:
        """Position of the diagram in its amplitude."""
        return self._index

    @property
    def particles(self) -> tuple[str, ...]:
        """Names of the particles of the diagram, each listed once."""
        return tuple(self._native.diagram_particles(self._index))

    @property
    def mediators(self) -> tuple[str, ...]:
        """Names of the mediator particles of the diagram, each listed once."""
        return tuple(self._native.diagram_mediators(self._index))

    @property
    def expression(self) -> Expr:
        """Symbolic value of the diagram."""
        return self._native.diagram_expression(self._index)


class Amplitude:
    """Amplitude of external legs: a sum of Feynman diagrams.

    Returned by ``compute_amplitude``. Its diagrams are read with ``diagrams``.
    """

    def __init__(self, native: _core.Amplitude) -> None:
        self._native = native

    @property
    def diagrams(self) -> tuple[Diagram, ...]:
        """Diagrams of the amplitude, in order."""
        return tuple(
            Diagram(self._native, index)
            for index in range(self._native.diagram_count())
        )

    def filter(self, predicate: Callable[[Diagram], bool]) -> Amplitude:
        """Copy of the amplitude keeping only the diagrams where the predicate holds.

        The amplitude itself is not changed. Use it to build subsets, then
        pass them to ``compute_squared_amplitude``.
        """
        keep = [diagram.index for diagram in self.diagrams if predicate(diagram)]
        return Amplitude(self._native.select(keep))

    def display(self) -> str:
        """Printed form of every diagram expression, one per line."""
        return self._native.display()


def compute_feynman_rules(model: Any) -> FeynmanRules:
    """Feynman rules of a finalized model, as a read-only view.

    ``len`` gives the number of rules; ``rules.str(i)`` and ``rules.latex(i)``
    print the rule at index ``i``.
    """
    _check_finalized(model)
    return _core.compute_feynman_rules(model)


def compute_amplitude(
    model: Any,
    order: Order,
    insertions: Sequence[Insertion],
    kinematics: Sequence[int] | None = None,
    options: FeynOptions | None = None,
) -> Amplitude:
    """Tree-level or one-loop amplitude of the external legs of a finalized model.

    ``kinematics`` gives one distinct positive momentum index per insertion,
    in the order of ``insertions``; ``None`` keeps the default indices
    ``1, ..., N``. Explicit momentum tensors are not supported yet.

    >>> from pymarty.calculation import Order, compute_amplitude, incoming, outgoing
    >>> compute_amplitude.__name__
    'compute_amplitude'
    """
    _check_finalized(model)
    options = options or FeynOptions()
    _check_particle_names(
        model,
        [item.field.particle for item in insertions]
        + [particle for item in options.filters for particle in item.particles],
    )
    legs = [
        (
            item.field.particle,
            item.incoming,
            item.field.antiparticle,
            item.field.chirality,
        )
        for item in insertions
    ]
    native = _core.compute_amplitude(
        model,
        order,
        legs,
        list(kinematics) if kinematics is not None else [],
        options.topology or "",
        _particles_of(options.filters, "force"),
        _particles_of(options.filters, "forbid"),
        _particles_of(options.filters, "disable"),
        list(options.fermion_order),
    )
    return Amplitude(native)


def compute_squared_amplitude(
    model: Any, amplitude: Amplitude, other: Amplitude | None = None
) -> Expr:
    """Squared amplitude, summed over the spins and polarizations.

    With ``other``, returns the interference term of ``amplitude`` and
    ``other``.

    >>> from pymarty.calculation import compute_squared_amplitude
    >>> compute_squared_amplitude.__name__
    'compute_squared_amplitude'
    """
    _check_finalized(model)
    if other is None:
        return _core.compute_squared_amplitude(model, amplitude._native)
    return _core.compute_squared_amplitude(model, amplitude._native, other._native)


WilsonSet = _core.WilsonSet


def wilson_coefficients(
    model: Any, amplitude: Amplitude, operator_coefficient: Expr | None = None
) -> WilsonSet:
    """Wilson coefficients of a one-loop amplitude, by operator.

    ``operator_coefficient`` multiplies every operator (default 1). Read a
    coefficient with ``WilsonSet.coefficient``; the chromomagnetic operators
    come from ``chromo_magnetic_operator``.

    >>> from pymarty.calculation import wilson_coefficients
    >>> wilson_coefficients.__name__
    'wilson_coefficients'
    """
    _check_finalized(model)
    factor = operator_coefficient if operator_coefficient is not None else Expr(1)
    return _core.wilson_coefficients(model, amplitude._native, factor)


def chromo_magnetic_operator(
    model: Any, wilsons: WilsonSet, coupling: str
) -> list[Expr]:
    """Operators of the chromomagnetic dipole with the given Dirac coupling.

    ``coupling`` is one of S, P, L, R, V, A, VL, VR, T, TA, TL or TR.
    """
    _check_finalized(model)
    return _core.chromo_magnetic_operator(model, wilsons, coupling)


def chromo_magnetic_coefficient(model: Any, wilsons: WilsonSet, coupling: str) -> Expr:
    """Coefficient of the chromomagnetic dipole with the given Dirac coupling.

    Sum of the Wilson coefficients matching the chromomagnetic operator of
    ``coupling`` (S, P, L, R, V, A, VL, VR, T, TA, TL or TR), computed in one
    step from ``wilsons``. Prefer it to ``WilsonSet.coefficient`` applied to
    ``chromo_magnetic_operator``, which loses the operator prefactors.
    """
    _check_finalized(model)
    return _core.chromo_magnetic_coefficient(model, wilsons, coupling)


def select_diagrams(
    amplitude: Amplitude, predicate: Callable[[Diagram], bool]
) -> list[Diagram]:
    """Diagrams of the amplitude for which the predicate is true.

    The amplitude itself is not changed.

    >>> from pymarty.calculation import select_diagrams
    >>> select_diagrams.__name__
    'select_diagrams'
    """
    return [diagram for diagram in amplitude.diagrams if predicate(diagram)]


@contextlib.contextmanager
def set_options(**flags: bool) -> Iterator[None]:
    """Sets global MARTY flags for a ``with`` block, then restores them.

    Names are snake_case versions of the C++ option names. Unknown names raise
    ``MartyValueError`` listing the valid ones.

    >>> from pymarty.calculation import set_options
    >>> with set_options(print_errors=False):
    ...     pass
    """
    for name, value in flags.items():
        if not isinstance(value, bool):
            raise MartyTypeError(f"option {name!r} takes a bool")
    previous = {name: _core.get_option(name) for name in flags}
    try:
        for name, value in flags.items():
            _core.set_option(name, value)
        yield
    finally:
        for name, value in previous.items():
            _core.set_option(name, value)
