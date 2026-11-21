from pymarty._pymarty import modeling as _core
from pymarty.symbolic import Index

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
    def minkowski_index(self, name: str = "mu") -> Index: ...

spacetime: _Spacetime

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
