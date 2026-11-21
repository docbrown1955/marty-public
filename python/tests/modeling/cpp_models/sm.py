"""Partial pure-pymarty port of the C++ Standard Model (src/marty/models/sm.cpp).

Ported: gauge groups C, L, Y; the first-generation-like quark doublet Q with
SM_flavor; the Higgs doublet H; gauge symmetry breaking of Y and then L with
the Q renaming used by the C++ model; the flavour symmetry breaking.

Not yet ported: ghost-boson replacements, gauge-fixing terms, explicit
Yukawa/CKM matrices, and the Higgs potential/VEV expansion steps that depend
on them.
"""

from fractions import Fraction

from pymarty.group_theory import SU, U
from pymarty.modeling import FlavorGroup, GaugeGroup, Model


def build() -> Model:
    """Return the partially ported Standard Model."""
    model = Model(
        gauge=[
            GaugeGroup(SU(3), name="C", boson_name="G", coupling="g_s"),
            GaugeGroup(SU(2), name="L", boson_name="W", coupling="g_L"),
            GaugeGroup(U(1), name="Y", boson_name="B", coupling="g_Y"),
        ],
        flavor=[FlavorGroup("SM_flavor", 3)],
    )

    model.add_weyl_fermion(
        "Q",
        chirality="L",
        gauge_rep={"C": (1, 0), "L": 1, "Y": Fraction(1, 6)},
        flavor_rep="SM_flavor",
    )

    model.add_scalar_boson(
        "H", gauge_rep={"L": 1, "Y": Fraction(1, 2)}, self_conjugate=False
    )

    model.break_gauge_symmetry("Y")
    model.break_gauge_symmetry("L", renaming={"Q": ["U_L", "D_L"]})
    model.break_flavor_symmetry(
        "SM_flavor",
        renaming={
            "U_L": ["u_L", "c_L", "t_L"],
            "D_L": ["d_L", "s_L", "b_L"],
        },
    )
    return model
