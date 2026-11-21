"""Model building: gauge groups, particles and breaking.

``Model`` is built from its gauge groups in one call. Its weyl fermions are
created unattached, given their representations with ``Particle.set_rep``,
then added with ``Model.add_particle``. Breaking a gauge group names the
particles it produces; ``renaming`` gives explicit names to some of them.
See ``examples/usage/modeling.py`` for a complete model.
"""

from pymarty._pymarty import modeling as _core
from pymarty.symbolic import Index, Minkowski

GaugeGroup = _core.GaugeGroup
FlavorGroup = _core.FlavorGroup
FeynmanRules = _core.FeynmanRules
Model = _core.Model
Particle = _core.Particle
Order = _core.Order
Leg = _core.Leg
Amplitude = _core.Amplitude
dirac_index = _core.dirac_index
renamed_indices = _core.renamed_indices


class _Spacetime:
    """Spacetime objects needed to write Lagrangian terms."""

    def minkowski_index(self, name: str = "mu") -> Index:
        """Return a Lorentz index of the Minkowski space, named ``name``."""
        return Minkowski.index(name)


spacetime = _Spacetime()

__all__ = [
    "Amplitude",
    "FeynmanRules",
    "FlavorGroup",
    "GaugeGroup",
    "Leg",
    "Model",
    "Order",
    "Particle",
    "dirac_index",
    "renamed_indices",
    "spacetime",
]
