from pymarty._pymarty import modeling as _core
from pymarty.symbolic import Index

GaugeGroup = _core.GaugeGroup
Model = _core.Model
Particle = _core.Particle

class _Spacetime:
    def minkowski_index(self, name: str = "mu") -> Index: ...

spacetime: _Spacetime

__all__ = ["GaugeGroup", "Model", "Particle", "spacetime"]
