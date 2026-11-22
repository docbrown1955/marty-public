"""Model building: gauge groups, particles and breaking.

``Model`` is built from its gauge groups and flavor groups in one call. Fermions
and bosons are added with ``Model.add_weyl_fermion``, ``add_dirac_fermion``,
``add_majorana_fermion`` and ``add_scalar_boson``, which take their gauge and
flavor representations, self-conjugacy and mass as keyword arguments.
``Model.add_similar_particle`` adds a particle of the kind of another one.
``Particle.set_rep`` and ``set_self_conjugate`` remain for particles created
unattached (e.g. by ``Particle.generate_similar_particle``). Breaking a gauge
group names the particles it produces; ``renaming`` gives explicit names to
some of them. ``Model.promote_to_majorana`` turns a self-conjugate Weyl fermion
into a Majorana one. ``Particle.mass`` and ``Particle.set_gauge_choice`` set
the mass and the gauge fixing of a particle.
The Dirac tensors ``dirac_gamma``, ``dirac_gamma5``, ``dirac_sigma``,
``dirac_P_L``, ``dirac_P_R`` and ``dirac_C`` take Lorentz and Dirac indices
explicitly. They are named apart from ``pymarty.diracology`` chain builders.
See ``examples/usage/modeling.py`` for a complete model.
"""

from pymarty._pymarty import modeling as _core

GaugeChoice = _core.GaugeChoice
GaugeGroup = _core.GaugeGroup
FlavorGroup = _core.FlavorGroup
Model = _core.Model
Particle = _core.Particle

dirac_gamma = _core.dirac_gamma
dirac_gamma5 = _core.dirac_gamma5
dirac_sigma = _core.dirac_sigma
dirac_P_L = _core.dirac_P_L
dirac_P_R = _core.dirac_P_R
dirac_C = _core.dirac_C


__all__ = [
    "FlavorGroup",
    "GaugeChoice",
    "GaugeGroup",
    "Model",
    "Particle",
    "dirac_C",
    "dirac_P_L",
    "dirac_P_R",
    "dirac_gamma",
    "dirac_gamma5",
    "dirac_sigma",
]
