"""Built-in models.

``SM()`` is the Standard Model and ``QED()`` is quantum electrodynamics. The
built-in models have no gauge group description, so ``Particle.set_rep`` is
not available on their particles.
"""

from pymarty._pymarty import modeling as _core


def SM():
    """Return the built-in Standard Model."""
    return _core.standard_model()


def QED():
    """Return the built-in QED model."""
    return _core.qed()


__all__ = ["QED", "SM"]
