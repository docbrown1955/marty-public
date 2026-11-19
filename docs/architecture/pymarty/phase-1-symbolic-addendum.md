# pymarty phase 1: addendum on `pymarty.symbolic` extensions

Status: proposal, pending user review. Nothing here is implemented. This addendum extends
[phase-1-symbolic.md](phase-1-symbolic.md) and must be started after phase 1. Ground rules are in
[python-porting-rules.md](python-porting-rules.md), and the list of unported features is in
[unported-features.md](unported-features.md).

Each section gives the current state, the proposal, and the decision needed (marked **Decision**).

## A. `relabel` becomes `Expr.relabel_indices`

**Current state (before this change).** Three free functions: `relabel(expr, *, relabel_free=True)`,
`relabel_with_renaming(expr)` (returns a tuple), and `apply_renaming(expr, renaming)`.

**Implemented.**

- One Expr method: `Expr.relabel_indices(renaming=None, *, relabel_free=True)`, returning a new
  expression. It is the only way to apply a renaming.
- `renaming` is an optional `dict[Index, Index]`, used as input and output. Entries for the
  expression's indices are presets and are honoured. The other indices get canonical names and are
  added to the dict. Entries for indices absent from the expression are kept. Without a dict, no map
  is exposed.
- `relabel`, `relabel_with_renaming`, `apply_renaming` and the `IndexRenaming` class are removed from
  `pymarty.symbolic`, with no aliases. The C++ facade `applyIndexRenaming` is removed too, since no
  caller needs it once `apply_renaming` is gone. CSL `ApplyIndexRenaming` stays, as MARTY core uses it.
- C++: the 3-argument `csl::RelabelIndices` honours presets already in `IndexRenaming`. The facade
  `relabelIndices(expr, relabelFree, IndexRenamingHandle&)` writes into the handle, which the Python
  layer then copies back into the dict.

**Decision A (resolved).** The renaming map is both input and output. The parameter is optional. When
it is given, its entries are used as presets, and the generated entries are written into it. When it
is omitted, no map is exposed. The map holds free indices only: dummy indices are local to
each expression and are never read from or written to it (Python raises `ValueError` for a dummy key). Writing into a caller's dict is a side effect, which the immutability
rule normally forbids. This is accepted as an explicit exception, and the docstring states it.

## B. `replace` and index replacement

**Current state.** Python `replace` is expression-only. `coerce` and `tryAsExpr`
(`python/src/symbolic.cpp:44-57`, `109-118`) reject bool and str. CSL offers more:

- `Replace(Expr, Expr, Expr)`, `Replace(Expr, Parent, Expr)`, `Replace(Expr, Parent, Parent)`
  (`src/csl/replace.h:30-32`);
- `ReplaceIndicial` (`src/csl/replace.h`, around line 38);
- member `replaceIndex` and `replaceIndexInPlace` (`src/csl/abstract.h:583-593`).

There is also a documentation conflict. The C++ facade `replaced` says "simultaneously"
(`src/csl/api/transform.h:66-71`), while the Python docstring says "single bottom-up pass"
(`python/src/symbolic.cpp`, replace binding, around lines 320-335). Phase-1 decision Q7
(`phase-1/02-questions.md`, `phase-1/03-spec.md`) chose a single pass.

**Proposal.**

- Keep `Expr.replace(old, new)` for expression-to-expression replacement only.
- Add `Expr.replace_index(old, new)` for index replacement (wraps `replaceIndex`).
- Add `Expr.replace_indicial(...)` only after its CSL semantics are confirmed by a C++ test.
- Parent replacement (`Replace(Expr, Parent, ...)`) is deferred. It needs a Python parent model
  (section D).
- Fix the C++ comment in `transform.h` so it matches the single-pass behaviour, or change the
  behaviour and update Q7. Both are C++ changes, so they need the user's approval first.

**Decision B1 (resolved).** `replace` stays expression-only. Index replacement is a separate method
(`replace_index`), not an overload.

**Decision B2 (resolved).** Single-pass semantics (Q7). The C++ facade comment in `transform.h` must be
corrected to match, and that is a C++ change to make with the other C++ fixes.

## C. Expression accessors (get and set)

**Current state.** Python exposes only `operand`, `free_indices`, `dummy_indices`, `name`,
`complex_property`, `structural_key`, `constant`, `variable`. CSL `Abstract.h` has about 77 getter and
predicate virtuals and about 22 setter virtuals. The full list is in
[unported-features.md](unported-features.md), section 2.

**Proposal (naming, following rules section 4).**

- Read-only getters that are cheap and total become Python properties (for example `args`,
  `value`, `numerical_factor`, `is_real`).
- Getters that depend on an argument become methods (for example `index(i)`, `argument(i)`).
- No modifiers that mutate. CSL setters are never ported (decision C2). A change to an expression is
  expressed by building a new expression or sub-expression.
- Predicates are properties prefixed with `is_` or `has_`.

**Error model (rules section 8).** Three options were proposed:

- **A.** Return `None` when the accessor does not apply.
- **B.** Raise `MartyTypeError` when the accessor does not apply (recommended), plus a `kind`
  property so that callers can check first.
- **C.** Typed views (for example `expr.as_sum()` returns a `SumView` or raises).

Only real absence (for example `parent` on an expression with no parent) returns `None`.

**C++ issues to fix first (ask before patching, per the ground rules).**

- Default accessors print to stdout before throwing (`src/csl/abstract.cpp:212-217`, `467-495`).
  The Python layer cannot silence this cleanly.
- The default error path throws the bare `CSLError` enum (`src/csl/error.cpp:75-96`,
  `src/csl/error.h:80`). The facade must translate it into a typed error at the boundary.
- `getNum` silently returns 0 for the wrong kind (`src/csl/abstract.cpp:189-192`). The facade must
  check the kind before calling it.

**Decision C1 (resolved).** Option B: raise a typed Python exception when an accessor does not apply,
based on the CSL error types, following the earlier exception design. Only real absence (for example
`parent` on an expression with no parent) returns `None`.

**Decision C2 (resolved).** Direct setters are never ported (rules section 4, principle 5). The C++
`set*` virtuals are not bound, and no `with_*` or `set_*` counterpart is created. Changes are made by
building new (sub-)expressions.

**Decision C3 (resolved).** The C++ inconsistencies (print to stdout, `getNum` returning 0, the bare
`CSLError` enum) are not fixed now. They are recorded in [unported-features.md](unported-features.md).
Exceptions are raised at the Python boundary, as the phase-1 exception design already does.

## D. Parent accessors (`TensorParent`, `AbstractParent`, `TensorField`)

**Current state.** `Tensor` and `TensorField` expose only `__call__`. CSL has read-only getters
(section 4 of [unported-features.md](unported-features.md)).

**Proposal.** Expose read-only getters first, as properties on the Python parent classes:
`space`, `symmetry`, `fully_symmetric`, `fully_anti_symmetric`, `permutation`, `dim`, `type`,
`primary_type`, `is_valued`, `is_traceless_in`, `field_space`, `point`, `argument`, `operand`,
`index_structure`, `sub_symbols`, `latex_point_suffix`, `complex_conjugate`, `is_indexed`.

- `getTensor` and `getTrace` are internal details. Proposal: exempt them from Python and record the
  reason in the unported list.
- Setters on parents are deferred (same error model as section C).

**Decision D (resolved).** The read-only subset is confirmed. `getTensor` and `getTrace` are exempt
from Python and recorded as such in the unported list.

## E. Callback algorithms duplicated in Python

**Current state.** CSL algorithms that take C++ callbacks (`ExpandIf`, `DistributeIf`, predicate
searches) cannot be bound directly. The phase-1 plan lists them in section 1 of
[phase-1-symbolic.md](phase-1-symbolic.md).

**Proposal (rules section 6: Python duplicates for every callback algorithm).**

- Option a: write Python duplicates in `python/pymarty/symbolic_algorithms.py`, imported by
  `symbolic.py` and re-exported. They use the accessors from section C, so they are thin. Parity
  tests check that Python and C++ give the same results on a shared set of golden expressions in
  `tests/`.
- Option b: when the predicate is simple (a fixed set of accessor-based conditions), expose it
  declaratively from C++ (an enum or a small predicate object), and avoid a duplicate algorithm.

**Decision E (resolved).** Duplicate every callback algorithm in Python, in a consistent way: option
(a) only. The C++ algorithms keep their callbacks, and no C++ API is changed for them. Each duplicate
lives in `python/pymarty/symbolic_algorithms.py`, is imported by `symbolic.py`, and has parity tests
against C++ on shared golden expressions (`tests/api/data/symbolic/algorithms.txt`).

**Status.** The nine search and visit algorithms, `DistributeIf` and `ExpandIf` are ported.
`DistributeIf` required the CSL fix at `src/csl/interface.cpp:256-290`: the mutating overload now
writes its result back to `expr`. `ExpandIf` covers products and integer powers. Function nodes
with arguments (`Exp`, scalar, duo and multi functions, vectorial and tensor derivative) are not
ported yet and raise `NotImplementedError`; see [unported-features.md](unported-features.md) section 5.

## F. `evaluate` and the eval mode

**Confirmed gap.** The facade calls `csl::Evaluated(..., csl::eval::all)`
(`src/csl/api/transform.cpp:129`). CSL's `eval::mode` is a bitset with base, literal, numerical,
indicial, abbreviation and all (`src/csl/evaluation.h:30-37`). `Evaluated(expr, user_mode)` and
`Evaluate(expr, mode)` exist in `src/csl/interface.h:339-351`. The mode is not exposed in Python.

**Proposal.** Add an `EvalMode` flag object in Python, with members `LITERAL`, `NUMERICAL`,
`INDICIAL`, `ABBREVIATION` and `ALL`, and `evaluate(mode=EvalMode.ALL)`. The C++ facade takes the
mode as an argument. The default stays `ALL`, so behaviour does not change for existing callers.

**Decision F (accepted and implemented).** `EvalMode` is a flag enum: modes combine with `|` and
test with `&`. Members are `Base`, `Literal`, `Numerical`, `Indicial`, `Abbreviation` and `All`
(Title case, as for `IndexType`). `All` is the union and the default, so `evaluate()` now expands
abbreviations. `evaluate` takes
`mode=` as a keyword, which is reserved and cannot be used as a variable name. `ABBREVIATION` is in
scope, so abbreviation management is in scope for phase 1. This supersedes the A11 exclusion in
`phase-1/03-spec.md` (line 273). Port-table rows 80-83 are now `ported`.

## G. Order of work

1. User review of [python-porting-rules.md](python-porting-rules.md), section 9, and of this addendum.
   All decisions A to F are resolved, and so are C1 to C3. Setters are never ported.
   Abbreviation management is in phase 1 scope.
2. Abbreviation management: done (`make_abbreviation`, `EvalMode`), tests first.
3. [unported-features.md](unported-features.md) is updated with the accepted statuses.
4. For each accepted item: failing tests first (rules section 3), then the C++ facade and the
   binding, then the Python layer. Run `ctest -L units` and `ctest -L integration` (and `api` for
   facade changes) after any C++ change. Never run the system suite.

No code is changed until the user accepts the decisions.
