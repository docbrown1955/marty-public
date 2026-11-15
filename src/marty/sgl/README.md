# SGL: Dirac and gamma-matrix algebra

SGL implements the Dirac-space algebra used by MARTY. It works on its own objects (gamma-matrix
chains, indices, traces) and converts to and from CSL expressions.

Main features:

- Gamma-matrix chains (`IndexChain`, `Prod`, `Sum`) with Minkowski, Dirac, epsilon, momentum
  and generalised indices.
- Contraction and simplification of chains (`contraction.*`, `simplify.*`, `sglalgo.h`,
  `sgloperations.*`), ordering (`ordering.*`), and commutation (`sglcommutation.*`).
- Fierz rearrangements (`fierz.*`) and traces (`tracer.*`).
- Conversion to and from CSL (`cslconverter.*`, `cslexpr.*`, `typecast.h`), used through
  `csl_to_sgl` / `sgl_to_csl`.
- Field objects (`sglfield.*`), tensor sets (`tensorset.*`) and options (`sgloptions.h`).
- Error and debug support (`exceptions.h`, `debuglog.h`).

Entry points: `sgl.h` (includes everything) and `sglinterface.h`. The gamma API in
`../api/` is a thin free-function layer over this code.
