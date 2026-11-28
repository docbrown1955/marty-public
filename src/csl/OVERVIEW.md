# CSL: computer algebra kernel

`src/csl/` is CSL, the symbolic engine used by MARTY. It is logically separate from the physics
code and can be used on its own. Its main features:

- Symbolic expressions as shared pointers (`csl::Expr`) to polymorphic `csl::Abstract` objects:
  numbers, constants, variables, elementary functions, sums, products, powers, and tensors.
- Indices and tensors (`Index`, `IndexStructure`, `Tensor`, `ISum`, `IProd`) with free-index
  checking and contraction.
- Simplification, pattern matching, commutation and symmetry rules, replacement, and
  expansion/factorisation (`interface.h`, `interface_indicial.h`).
- Generation of C numerical libraries from expressions (`librarygenerator*.cpp` and the
  `Library*` classes), with C++ and Python layers on top. Quadruple precision is an option of
  the C implementation only, never part of a public interface.

Layout:

| Path | Content |
|------|---------|
| `*.h`, `*.cpp` at top level | core types (`abstract`, `operations`, `literal`, `numerical`, `index`, `tensorField`, `symmetry`, `simplification`, `interface`) |
| `core/` | `operator.cpp`, the `Operator` base for expression visitors |
| `utils/` | locks and general utilities |
| `algorithms/` | pattern matching and simplification algorithms |
| `numerical/` | numerical math functions |
| `library/` | evaluation and function groups for generated libraries |
| `csl.h` | umbrella header |

Build targets: `csl` (shared), `csl-static` and `csl-core` (object library), defined in
`CMakeLists.txt`.

The `README.md` in this folder is the Doxygen main page for CSL (`USE_MDFILE_AS_MAINPAGE`), so
it is kept as is. The `csl` file (no extension) is a stray copy of a license header with
`#include "csl.h"` and is not used by the build.

For more, see [../../docs/architecture/functionalities.md](../../docs/architecture/functionalities.md)
(section 2.1) and the CSL manual `docs/doc/csl-manual.pdf`.
