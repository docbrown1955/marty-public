# UFO export (`mty::experimental::ufo`)

Converts MARTY Feynman rules into simplified vertices that can be written in the UFO format
(Universal FeynRules Output) for Monte Carlo tools.

Main classes:

- `Vertex` (`vertexinfo.h`, `simplifiedvertex.*`): one interaction vertex with its fields,
  colour and Lorentz couplings.
- `Field` (`field.h`): an external field of a vertex, with its name, spin, charge, colour
  dimension and conjugation flag.
- `LorentzCoupling` and `DiracMatrix` (`lorentzcoupling.h`, `diracmatrix.h`): Minkowski and
  Dirac tensor structure of a vertex.
- `ColorCoupling` (`colorcoupling.h`): colour structure.
- `Charge` and `electriccharge.*` (`charge.h`): electric charge of a particle, found from the
  Lagrangian.
- `SMData` (`ufosmdata.h`): conventions for SM-like models (names of the photon, colour group
  and electromagnetic coupling).
- `parsefactors.*`: splits a Feynman rule into separate contributions.

Entry point: `obtainSimplifiedVertices` (and the single-rule and subset overloads), declared in
`simplifiedvertex.h`. Include everything through `marty-ufo.h`.
