# CSL core

Base operator machinery for CSL expressions.

- `operator.cpp` implements `csl::Operator`, the base class for operator-like objects acting on
  expressions. MARTY's Wick calculator derives from it (`src/marty/core/wick.h`).

The expression types themselves (`Abstract`, `Sum`, `Prod`, `Pow`, ...) are declared in the
parent folder `src/csl/`, not here.
