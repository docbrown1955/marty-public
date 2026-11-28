Status: APPROVED
Updated: 2026-11-17
Author: orchestrator
Approved (G1): user, 2026-11-17

# Phase 1 Q&A

Inputs: `01-investigation-core-classes.md` (READY, claims re-checked in its section 7) and section 7.1 of the same file (index and space lifetime, not yet a topic file).

Answers are recorded below each question once given.

Provenance: Q1 to Q4 were put to the user by a subagent that went beyond its role, not by the orchestrator. The user confirmed on 2026-11-17 that they were asked and answered them ("the subagent asked me those questions"), so the answers stand. Q5 and Q6 are the user's scope decisions of the same day.

## Q1. Equality of symbols (constants and variables)

- Context: CSL compares constants and variables by parent identity, not by name (`src/csl/literal.cpp:239-250`). Each `constant_s("a")` call creates a new parent (`src/csl/literal.cpp:665-670`), so two calls with the same name give two different symbols.
- Options:
  - (a) Keep CSL identity. `equals` and `hash` are identity-based and consistent with `==`.
  - (b) Same name means same symbol. The facade interns symbols by name. This differs from CSL and from the existing unit test (`tests/units/csl/test_equality.cpp:28-29`, from the investigation, not re-read).
- Recommendation: (a). It matches the C++ behaviour the parity tests compare against, and it keeps `hash` simple.
- Blocks: `EqualityDocumentedSemantics`, `HashConsistentWithEquality`, the symbol constructors in the spec.
- Answer (2026-11-17): (a) Identity, as in CSL. Recorded as decided.

## Q2. `pow` with a zero base or an infinite base

- Context: `pow_s` calls `std::raise(SIGSEGV)` for base `oo` and for base 0 with a negative numerical exponent (`src/csl/operations.cpp:3158-3170`). A signal cannot be caught as an exception, so a Python call would kill the interpreter.
- Options:
  - (a) The facade checks base and exponent before calling `pow_s` and raises `ValueError`. CSL is unchanged, so C++ users keep the current behaviour.
  - (b) Change `pow_s` to throw a `CSLError` (`ZeroDivisionError` or `ValueError`). This changes MARTY core for all users and is a C++ behaviour change.
- Recommendation: (a) for phase 1, with (b) recorded as a follow-up item for later phases.
- Blocks: the error table of the spec and the `pow` tests.
- Answer (2026-11-17): (b) Change `pow_s` to throw a `CSLError`. This is a C++ behaviour change in MARTY core (affects every caller that relied on `SIGSEGV`). The spec must list it as a core change requiring explicit approval. Error types, decided: base `oo` gives `ValueError`; base 0 with a negative numerical exponent gives `ZeroDivisionError`.

## Q3. `sqrt(4)` output form

- Context: `sqrt_s(const Expr&)` is `pow_s(x, 1/2)` (`src/csl/operations.cpp:3194-3196`). The integer-base branch does not fire for 1/2 (`operations.cpp:3134-3137`), so the result is `Pow(4, 1/2)`, not `2`. Acceptance criterion 3 of the plan requires `str(e)` to match the C++ output byte for byte.
- Options:
  - (a) Accept CSL's form and document it. Same output as C++, so the parity requirement holds.
  - (b) The facade turns perfect squares into integers. Output then differs from C++, which breaks criterion 3 unless the same rule is added to the C++ side.
  - (c) Change CSL so that `sqrt(4)` gives `2`. This is a core behaviour change.
- Recommendation: (a). It keeps parity without touching core.
- Blocks: the `FunctionsSpecialValues` test and the expected output table.
- Answer (2026-11-17): (c) Change CSL `pow_s` so that perfect powers give exact results (`sqrt(4)` gives `2`). This is a core change: C++ output changes for these inputs, so the spec must list the affected tests and golden data, and it needs explicit approval.
- Verification that prompted the re-ask: today `pow_s(4, 1/2)` returns `Pow(4, 1/2)`. No perfect-square branch exists (`operations.cpp:3122-3140`, `3134-3137`). `sqrt_s(int)` calls `sqrt_s(int_s)`, then `pow_s`. `Pow::evaluate` on numerical operands calls `Integer::exponentiation_own` (`numerical.cpp:414-440`), which calls `pow_s` again for an `IntFraction` exponent, so it also stays `Pow(4, 1/2)`. Side finding: `pow_s(4, 3/2)` returns `4 * Pow(4, 1/2)` (integer part extracted, not fully simplified). The spec must decide whether that case changes too.

## Q4. Lifetime of spaces and indices seen from Python

- Context: `Index` holds a raw `const Space *` (`src/csl/index.h:84`). Tensors built on a user-created space keep `this` (`src/csl/space.cpp:71`). Static spaces (`Minkowski`, `Euclid_R2..R4`) live for the whole program (`src/csl/space.h:440-474`). Group-owned and flavour-owned spaces are owned by their group or `FlavorGroup` (`src/marty/core/group.cpp:143-146`, `src/marty/core/flavor.h:77`).
- Options:
  - (a) The Python `Space` handle keeps its owner alive (a `shared_ptr` to the space, or to the group or model that owns it). Any space can then be used, and an `Index` or `Tensor` never dangles.
  - (b) Expose only static spaces in phase 1, and forbid user-created spaces until a later phase. Simpler, but `SpaceHandle` cannot be built from a user dimension.
- Recommendation: (a), with a test `IndexOutlivesScope`. The plan already lists `SpaceHandle` and `tensor(name, spaces, symmetry)`, and user-created spaces need the same guarantee.
- Blocks: the `SpaceHandle` and `IndexHandle` design, the lifetime tests, and the `tensor()` signature.
- Answer (2026-11-17): (a) Handle keeps its owner alive. Recorded as decided.

## Q5. How many transformations are exposed

- Context: the plan lists `expand`, `deep_expand`, `factor`, `collect`, `diff`, `subs`, `simplify`, `evaluate`, `conjugate`, `real`, `imag`, `depends_on`, `free_symbols`. CSL has many more algorithms, and several take callbacks or global flags.
- Answer (2026-11-17, user): the general transformation machinery is not ported. Only specific, pre-defined transformations are exposed, with a fixed signature each. More can be added in a later phase.
- Answer (2026-11-17, user, second batch): exposed set is `expand`, `diff`, `subs`, `evaluate`, `depends_on`, `free_symbols`, `conjugate`, `real`, `imag`, `factor`, `collect`, `simplify`. `deep_expand` was not selected, so it is deferred (an `expand(full=...)` option is not part of phase 1). Each has a fixed signature and no callback.
- Consequences: the transforms investigation marks the flags and the tests of `factor` and `collect` as "not checked", and `simplify`, `real`, `imag` were not read to the same depth. `GetComplexConjugate` has existing C++ uses (`tests/units/csl/test_latex.cpp:585`, `tests/integration/src/test_physics_latex.cpp`). A short follow-up investigation of those six is needed before WP3 (or each gets a pinning C++ test written first). A `simplify` that depends on global flags is a risk to flag at the spec.
- Blocks: spec sections 3.1 and 5, WP3, the transform tests, and the port-table classification of the cases that need a deferred transformation.

## Q6. Indices and tensors

- Answer (2026-11-17, user): important, part of this phase. `Space`, `Index`, `Tensor`, `delta`, `metric`, `epsilon`, symmetries, contraction and relabelling are in scope, and the usage example must show them with asserted results, like the rest of it.
- Consequence: the indices-tensors investigation (`01-investigation-indices-tensors.md`, READY, verified in its section 7) is the input for WP4 and for the tensor part of the usage example. The Python signatures of `Space`, `Index` and `tensor` in the previous spec draft are placeholders until the spec is rewritten.

## Q7. `subs` semantics

- Context: CSL `Replace` is a bottom-up single pass (`src/csl/algo.cpp:419-429`). A compound key may match after a child was replaced (code reading, no test).
- Answer (2026-11-17, user): (b) call `Replace` as is. No placeholder pass and no new matching rule. All pairs of the mapping are given to `Replace` in one call.
- Consequences: the spec documents it as "replacements are applied in one bottom-up pass"; the behaviour for chained and compound keys is whatever `Replace` does, and C++ tests pin the cases the usage example relies on (symbol swap, a sum key, a `Pow` key). The example must not claim more than the tests show.

## Q8. `evaluate` with free symbols

- Answer (2026-11-17, user): (a) return the partially evaluated `Expr`, as CSL does. Values are given by symbol name (`evaluate(x=3)`) and applied by replacement; no `setValue` on the shared parent (no global change). `complex()` and `float()` raise `MartyValueError` and list the free symbols.

## Q9. Spaces exposed

- Answer (2026-11-17, user): user-created space (name, integer dimension, optional index names), and predefined `Minkowski` and `Euclid_R2`, `Euclid_R3`, `Euclid_R4`. Signed user spaces and symbolic-dimension spaces are deferred.
- Note: `Minkowski` is a signed space, so index sign rules (lower-lower contraction raises) apply to it. The Python API needs a way to flip an index sign (`flipped()`, mirroring CSL).

## Q10. Hash of `Expr`

- Context: the usage example passes symbols as dict keys. CSL equality is structural and depends on global flags, so a structural hash of compound nodes cannot be kept sound.
- Answer (2026-11-17, user): (a) symbols and numbers are hashable, consistent with `==` (symbols by parent identity, numbers by value). `hash()` of a sum, product, power or function raises `TypeError`. `structural_key()` is still offered. This replaces the draft `__hash__ = None` and gives `free_symbols` a `frozenset` of `Expr`.

## Q11. Tensor symmetries

- Answer (2026-11-17, user): (a) full symmetric, full antisymmetric, and index-pair symmetric or antisymmetric. A conflicting combination raises instead of CSL's silent no-op (`addSymmetry` after `setFullySymmetric`, verified). General permutations are deferred. No C++ test exists for any of it, so tests come first.

## Q12. Contraction

- Answer (2026-11-17, user): (a) through the product operator only (`T(i) * U(i)`), plus `free_indices` and `dummy_indices` queries. No `ISum` or `IProd` objects.

## Q13. Space and index creation

- Answer (2026-11-17, user, free text): single index generation with an optional name, and generation of a list of indices (for example 10 indices, then use `idx[5]` in expressions).
- Interpretation, to be confirmed at G1: `space.index(name=None)` returns one `Index` (CSL `generateIndex(name)` or `generateIndex()` with the next default name); `space.indices(n, names=None)` returns a list (CSL `generateIndices(N)` and the overload with names, `space.h:287-288`). `Index(name, space)` is not public. The space constructor has `index_names` with a default; the default tuple is to be settled in the spec. `resetIndexNumber` stays out of scope.

## Q14. `simplify`

- Context: `csl::Simplify` is only declared (`src/csl/simplification.h:124`). No definition exists in the repo (`src/csl/simplification.cpp` has only comparison rules and `operator<` overloads; the `algorithms/` files are empty), and no code calls it. Verified by the orchestrator, see `01-investigation-transforms-2.md` section 7.
- Answer (2026-11-17, user, free text): "Do nothing in python, remove the stale C++ declarations". `simplify` is not exposed. The undefined declarations are removed from the C++ header.
- Consequences: removing declarations is a C++ source change (header only, no behaviour change). The orchestrator found that every declaration from `printVector` to `evolve` in `src/csl/simplification.h` (lines 63-137) looks stale: `grep` finds no definition and no caller outside the header for `getRecursiveAlternateForms`, `internalRecursiveAlternateForms`, `numberOfMutatingNodes`, `insertSortMutant`, `addMutants`; the other names are not checked yet. The work package removes only declarations with zero definitions and zero callers (grep evidence in the log) and reports the list. `sgl::Simplify` in `src/marty/` is another function and is untouched.

## Q15. Exact perfect-power rule

- Answer (2026-11-17, user): (a) full rule. For an integer base `a > 0` and a rational exponent `p/q` in lowest terms with `q > 1`: if `a = r^q`, the result is `r^p`. `sqrt(4)=2`, `4^(3/2)=8`, `sqrt(2)` unchanged.

## Q16. Division by an exact zero

- Context: `div_s` prints "Warning division by zero" to `std::cerr` and returns `CSL_INF` (`src/csl/operations.cpp:2520-2529`); `intfraction_s(5, 0)` is `CSL_INF` (`tests/units/csl/test_numbers.cpp:119-120`).
- Answer (2026-11-17, user): (b) keep the CSL behaviour, `x / 0` and `Expr(5) / 0` return the CSL infinity.
- Tension to confirm at G1: Q2 makes `0 ** -1` (base 0, negative exponent) raise `ZeroDivisionError`, while `x / 0` returns `oo` (with the C++ warning on stderr, which the facade cannot silence without a flag change). Two paths of CSL, two behaviours. The spec records both as decided and asks at G1 whether `x / 0` should also raise.

## Q17. Error class for zero to a negative power

- Answer (2026-11-17, user): (a) new `MartyZeroDivisionError(MartyError, ZeroDivisionError)` in `python/pymarty/errors.py`, so that callers can catch either.
