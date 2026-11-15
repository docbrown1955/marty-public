# CSL algorithms

Algorithms that operate on whole CSL expressions.

- `simplification.cpp`: the main simplification routine (combining like terms and factors).
- `patternMatch.cpp`: pattern matching between expressions, with wildcards, used by
  replacement and simplification rules.

Public declarations are in `src/csl/simplification.h`, `src/csl/patternMatch.h` and
`src/csl/algo.h` (generic sub-expression walks).
