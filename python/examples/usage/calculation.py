"""Usage example: tree-level calculation with ``pymarty.calculation``.

Covers Feynman rules, amplitudes, squared amplitudes, filters, options and export.
The calculation takes a finalized model; ``Model`` itself gains no calculation methods.

Run it with ``python calculation.py``.
"""

from pymarty import visualization
from pymarty.calculation import (
    FeynOptions,
    Order,
    anti,
    compute_amplitude,
    compute_feynman_rules,
    compute_squared_amplitude,
    force_particle,
    incoming,
    left,
    outgoing,
    select_diagrams,
    set_options,
)
from pymarty.errors import MartyLookupError, MartyValueError
from pymarty.modeling.models.qed import QED

# 1. A calculation needs a finalized model; it is refused on an open one.
PSI = "\\psi"
qed = QED()
print("model finalized:", qed.is_finalized())

# 2. Feynman rules: a read-only view; len() and the printed or LaTeX form of each rule.
rules = compute_feynman_rules(qed)
print(f"{len(rules)} Feynman rules")
assert len(rules) > 0

# 3. Compton scattering psi A -> psi A at tree level (first computation of sampleQED).
#    The QED fermion is named "\psi"; its Weyl components are "\psi_L" and "\psi_R".
#    Insertions are external legs: incoming for initial, outgoing for final states.
compton = compute_amplitude(
    qed,
    Order.TREE,
    [incoming(PSI), incoming("A"), outgoing(PSI), outgoing("A")],
)
print("Compton diagrams:", len(compton.diagrams))
assert len(compton.diagrams) == 2
# display() returns the text, so it can be stored or checked.
text = compton.display()
assert isinstance(text, str) and text
print("Compton amplitude:", text[:80], "...")

# 4. anti(PSI) is the antiparticle; left(PSI) is the left-handed component.
diff = compute_amplitude(
    qed,
    Order.TREE,
    [
        incoming(PSI),
        incoming(anti(PSI)),
        outgoing(PSI),
        outgoing(anti(PSI)),
    ],
)
print("psi psibar -> psi psibar diagrams:", len(diff.diagrams))
chiral = compute_amplitude(
    qed,
    Order.TREE,
    [incoming(left(PSI)), incoming("A"), outgoing(left(PSI)), outgoing("A")],
)
assert chiral.display()

# 5. Squared amplitude, same form as the C++ computeSquaredAmplitude.
sq = compute_squared_amplitude(qed, compton)
print("squared Compton:", str(sq)[:60], "...")

# 6. FeynOptions is plain data; filters are converted to the C++ filters.
with_top = FeynOptions(topology="box", filters=[force_particle(PSI)])
print("options set:", with_top is not None)

# 7. Global flags use a context manager, restored on exit (also after an error).
with set_options(simplify_amplitudes=False):
    raw = compute_amplitude(
        qed,
        Order.TREE,
        [
            incoming(PSI),
            incoming("A"),
            outgoing(PSI),
            outgoing("A"),
        ],
    )
    assert raw.display()
# Outside the block the flag is back to its default.
again = compute_amplitude(
    qed,
    Order.TREE,
    [incoming(PSI), incoming("A"), outgoing(PSI), outgoing("A")],
)
assert again.display()

# 8. A Python predicate selects which diagrams are returned, not the expression.
#    Diagram.particles is the tuple of particle names, so PSI is an exact match.
with_fermion = select_diagrams(compton, lambda diagram: PSI in diagram.particles)
print("diagrams containing psi:", len(with_fermion))

# 9. Export towards the diagram viewer: the GRAFED diagram file, without any GUI.
payload = visualization.export_json(compton)
assert '"diagrams"' in payload
print("GRAFED export: first 40 characters:", payload[:40])

# 10. Errors come from C++ with a message, typed through pymarty.errors.
try:
    compute_amplitude(qed, Order.TREE, [incoming("no_such_particle"), outgoing(PSI)])
except MartyLookupError as error:
    print("unknown particle is a LookupError:", error)

try:
    with set_options(no_such_flag=True):
        pass
except MartyValueError as error:
    print("unknown option is a ValueError, listing the valid names:", str(error)[:60])

# 11. After an error the state is intact and the next call works.
assert compute_amplitude(
    qed, Order.TREE, [incoming(PSI), incoming("A"), outgoing(PSI), outgoing("A")]
)
print("state restored after errors")
