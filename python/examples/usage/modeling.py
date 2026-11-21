"""Usage example: model building (gauge groups, particles, breaking).

``pymarty.modeling`` builds a model the way ``examples/gettingStarted.cpp``
does, up to the Feynman rules. A ``Model`` owns gauge groups and particles,
and a particle obtained by name from a model refers to the particle the model
holds. Built-in models come from ``models``; this phase provides ``QED`` and
``SM`` only.

Every step prints what it shows and asserts the result, so the example fails
as soon as the library stops doing what is written here.

Run it with ``python modeling.py``. It is also run by the test suite
(see ``docs/architecture/pymarty/testing-strategy.md``, section 1.2).
"""

from fractions import Fraction

from pymarty import models
from pymarty.errors import MartyLookupError
from pymarty.group_theory import SU, U
from pymarty.modeling import GaugeGroup, Model, spacetime

# A toy model: SU(2)_L x U(1)_Y, as in gettingStarted.cpp. The constructor
# declares the groups, initializes the model and names the gauge bosons.
toy = Model(
    gauge=[
        GaugeGroup(SU(2), name="L", boson_name="W"),
        GaugeGroup(U(1), name="Y", boson_name="B"),
    ],
)

# Weyl fermions are created unattached, then given their representations
Q_L = toy.weyl_fermion("Q_L", chirality="L")
u_R = toy.weyl_fermion("u_R", chirality="R")
d_R = toy.weyl_fermion("d_R", chirality="R")

Q_L.set_rep("L", 1)
Q_L.set_rep("Y", Fraction(1, 6))
u_R.set_rep("Y", Fraction(2, 3))
d_R.set_rep("Y", Fraction(-1, 3))

toy.add_particle(Q_L)
toy.add_particle(u_R)
toy.add_particle(d_R)

# Breaking names the resulting particles. Q_L gets explicit names, the other
# broken particles keep the default names.
toy.break_gauge_symmetry("Y")
toy.break_gauge_symmetry("L", renaming={"Q_L": ["u_L", "d_L"]})

print(toy)

# Particles obtained by name see the state of the model
W_1 = toy.particle("W_1")
assert "W_1" in toy
assert W_1.name == "W_1"

# Spacetime objects are needed to write Lagrangian terms
mu = spacetime.minkowski_index()
print("index:", mu)

# Unknown names raise a typed error, they are not printed and ignored
try:
    toy.particle("no_such_particle")
except MartyLookupError as error:
    print("expected error:", error)
else:
    raise AssertionError("unknown particle name must raise")

# Built-in models: the Standard Model and QED
sm = models.SM()
print("SM particles:", [p.name for p in sm.particles])
assert len(sm.particles) > 0
assert len(sm.feynman_rules) > 0

qed = models.QED()
print("QED particles:", [p.name for p in qed.particles])
assert len(qed.feynman_rules) > 0
