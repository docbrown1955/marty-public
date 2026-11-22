"""Model building: gauge groups, particles and breaking.

``Model`` is built from its gauge groups and flavor groups in one call. Fermions
and bosons are added with ``Model.add_weyl_fermion``, ``add_dirac_fermion``,
``add_majorana_fermion`` and ``add_scalar_boson``, which take their gauge and
flavor representations, self-conjugacy and mass as keyword arguments.
``Model.add_similar_particle`` adds a particle of the kind of another one.
``Particle.set_rep`` and ``set_self_conjugate`` remain for particles created
unattached (e.g. by ``Particle.generate_similar_particle``). Breaking a gauge
group names the particles it produces; ``renaming`` gives explicit names to
some of them.
See ``examples/usage/modeling.py`` for a complete model.
"""

from pymarty._pymarty import modeling as _core

GaugeGroup = _core.GaugeGroup
FlavorGroup = _core.FlavorGroup
Model = _core.Model
Particle = _core.Particle


__all__ = [
    "FlavorGroup",
    "GaugeGroup",
    "Model",
    "Particle",
]
