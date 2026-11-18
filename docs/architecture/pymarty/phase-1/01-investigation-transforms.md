Status: READY
Updated: 2026-11-17
Author: investigator

# Phase 1 investigation: transforms, evaluation, analysis (CSL)

## 1. Question

For each candidate Python transform (`expand`, `deep_expand`, `factor`, `collect`, `simplify`, `diff`, `subs`, `evaluate`, `conjugate`, `real`, `imag`, `depends_on`, `free_symbols`), what is the C++ CSL function, its header and signature, where it is defined, whether it mutates its input, which `csl::option` flags it reads or writes, what `Replace` does with several pairs, how numeric evaluation treats free symbols, how values are supplied, how free symbols are enumerated, which failure modes exist, and which existing tests pin the behaviour.

Scope: read-only. No build, no test run, no `system` suite. All line numbers were read in this session unless marked "not verified".

## 2. Facts

### 2.1 Overall

1. There is no CSL function named `Subs`, `Diff`, `Simplify` (as a method), `FreeSymbols` or `DeepExpand` with a callback; the closest C++ names are `Replace`/`Replaced`, `Derived`, `Simplify`, `GetSymbol`/`getSubSymbols`. Searched: `grep -rniE "subs\(|Subs[A-Z]|Simultaneous" src/csl/api tests/api python` (hits only in `python/examples/usage/symbolic.py`), and `grep -rniE "free_?symbol|freeSymbol|getSymbols\("` over `src`, `tests`, `python` (no helper that returns a set of free symbols).
2. Expression handles: `Expand`, `Factor`, `Collect`, `Replace` take `Expr&` and assign the handle; the `...ed` variants (`Expanded`, `Factored`, `Collected`, `Replaced`) return a new `Expr` (`src/csl/interface.cpp:58-65`, `src/csl/replace.h:28-46`).
3. The `OPT` macro returns the original expression when the virtual method returns `nullopt`: `#define OPT(expr, func) ((expr->func).value_or(expr))` (`src/csl/interface.cpp:29`). Consequence: a method that returns `nullopt` makes the wrapper a no-op, not an error.
4. The `csl::option` flags are declared in `src/csl/options.h:26-58` (X-macro `MTY_CSL_OPTION_FLAGS` at `:46-58`). `errorStopsProgram` (`:28`) is defined and listed but no code reads it: `grep -rn errorStopsProgram src` returns only `options.h:28` and `options.h:49`.

### 2.2 Candidate table

| Candidate | C++ entry point (file:line) | Method it calls | Mutates input? | Flags read on its path | Flags written |
|---|---|---|---|---|---|
| `expand` | `Expand(Expr&, bool full=false, bool inplace=false)` `src/csl/interface.h:101`, impl `src/csl/interface.cpp:58-60`; `Expanded` `interface.h:103-104`, `interface.cpp:63-65` | `Abstract::expand(bool full, bool inplace)` `src/csl/abstract.h:960`; default returns `nullopt` `src/csl/abstract.cpp:725` | `Expand` reassigns handle; `Prod::expand_if` works on `Expr copy = Copy(this)` `src/csl/operations.cpp:2084` | none read directly in `Prod::expand`/`expand_if` (`operations.cpp:2075-2110`). Constructors that merge products read `freezeMerge` and `checkCommutations` (`operations.cpp:1041-1074`, `:1674-1753`); whether expansion reaches them is not verified | none |
| `deep_expand` | `DeepExpand(Expr&, bool inplace=false)` `src/csl/interface.h:116-118` (body `Expand(expression, true, inplace)`); `DeepExpanded` `interface.h:121-124` | same as `expand`, with `full=true` | same as `expand` | same as `expand` | none |
| `factor` | `Factor(Expr&, bool full)`, `Factor(Expr&, Expr_info, bool)`, `Factored` overloads in `src/csl/interface.cpp` (symbol-level; exact line not re-read) | `Abstract::factor(bool full)` `src/csl/abstract.h:892`; `factor(Expr_info, bool)` `abstract.h:899` | Handle reassigned by `Factor`; new object by `Factored` | not checked | not checked |
| `collect` | `Collect(Expr&, std::vector<Expr> const&)`, `DeepCollect`, `Collected`, `DeepCollected` in `src/csl/interface.cpp` (symbol-level) | `Abstract::collect(factors, full)` `src/csl/abstract.h:926`, returns `optional`; `Collect` leaves `expr` untouched when `nullopt` | `Collect` reassigns handle only if result exists | not checked | not checked |
| `simplify` | `Simplify(const Expr&, int depth=-1)` `src/csl/simplification.h:124` | body not read | not verified | not verified | not verified |
| `diff` | `Derived(const Expr&, Expr_info)` `src/csl/interface.cpp:297-299` (`OPT(expression, derive(variable))`); `Derived(const Expr&, const Expr&)` `src/csl/interface.h:315`; declaration `interface.h:313` | `Abstract::derive(Expr_info)` `src/csl/abstract.h:882` | no (returns new `Expr`) | none read | none |
| `subs` | no `Subs`. Closest: `Replace(Expr&, Expr const&, Expr const&)` `src/csl/replace.cpp:31-34`, vector overload `replace.cpp:68`; `Replaced(...)` declarations `src/csl/replace.h:73`, `:87`, `:100`, `:113`; `Swapped` `src/csl/interface.cpp` (uses a `"FOO"` temporary, three `Replaced` calls) | `applyThroughAbbreviations` `replace.cpp:234-251` with `csl::Transform` | `Replace(Expr&)` calls `csl::WeakDeepCopy(expr)` (`utils.cpp:109-117`) before traversal | none in `replace.cpp` (grep: no `option::` hit in that file). `find()` uses `Expr ==`, whose flag dependence is not verified; `fullComparison` is read by `operator<` `abstract.cpp:1353` and `simplification.cpp:194` | none |
| `evaluate` | `Evaluated(const Expr&)` `src/csl/interface.cpp:307-309`; `Evaluated(const Expr&, csl::eval::mode)` `interface.cpp:312-314`; declarations `interface.h:339`, `:341` | `Abstract::evaluate(mode)` pure virtual `src/csl/abstract.h:871-872`, default mode `csl::eval::base` | no (returns new `Expr`), but see 2.5 on global values | none read | none |
| `conjugate` | `GetComplexConjugate(const Expr&)` `src/csl/interface.cpp:362-364`; declaration `interface.h:428` | `Abstract::getComplexConjugate()` `abstract.h:999` (optional) | no | none read | none |
| `real` | `GetRealPart(const Expr&)` `src/csl/interface.cpp:337-339`; declaration `interface.h:369` | `getRealPart()` `abstract.h:978` (optional) | no | none read | none |
| `imag` | `GetImaginaryPart(const Expr&)` `src/csl/interface.cpp:342-344`; declaration `interface.h:371` | `getImaginaryPart()` `abstract.h:983` returns `Expr` (not optional); default body not read | no | none read | none |
| `depends_on` | `DependsOn(const Expr&, const Expr&)` `src/csl/interface.h:80-83` (body `init->dependsOn(x.get())`); overload with `csl::Parent` `interface.h:90-93`; `DependsExplicitlyOn` `interface.h:85-88` | `Abstract::dependsOn(Expr_info)` `abstract.h:775`, `dependsOn(Parent_info)` `abstract.h:777`, default `abstract.cpp:879` | no | none read | none |
| `free_symbols` | no helper. Per-node `getSubSymbols()` `src/csl/abstract.h:145` (pure virtual, returns `std::vector<Parent>`); `VisitEachLeaf` `src/csl/algo.h:285-290`; `GetSymbol(name, init)` `src/csl/interface.cpp:35-48` | `getSubSymbols` | no | none | none |

### 2.3 Replace and substitution semantics (subs)

5. Single-pair `Replace(Expr&, from, to)` forwards to the vector overload with one element (`src/csl/replace.cpp:31-34`).
6. The vector-of-`Expr` overload (`src/csl/replace.cpp:68`) first checks sizes (`CSL_ASSERT_SPEC(from.size() == to.size(), CSLError::ValueError, ...)`, same pattern in each overload), then branches: if `from[0]` has free indices it calls `ReplaceIndicial`; otherwise it calls `csl::WeakDeepCopy(expr)`, builds the complex-conjugate keys `ccFrom`, and runs `applyThroughAbbreviations` with `scalarReplacement` (`replace.cpp:91-93`).
7. `WeakDeepCopy` deep-copies every node whose `use_count() > 1` (`src/csl/utils.cpp:109-117`). Nodes shared with other holders are not mutated in place. Consequence (from code): the caller's other handles are not changed by `Replace`. Not verified by a test.
8. `scalarReplacement` (`src/csl/replace.cpp:261-296`) has three rules: exact match by `find(from, expr)` returns `to[i]`; match with `ccFrom` returns `GetComplexConjugate(to[i])`; and for `Pow(arg, n)` with a key `Pow(arg, m)` and an integer ratio `n/m > 0` returns `pow_s(to[i], ratio)` (`replace.cpp:278-294`). So replacing `x**2 -> y` also rewrites `x**4` to `y**2`.
9. `applyThroughAbbreviations` calls `csl::Transform(expr, lambda)` (`src/csl/replace.cpp:234-251`); the lambda calls the rule on each node and, on a hit, sets `sub = opt.value()` and returns `true`.
10. `csl::Transform` is bottom-up: it recurses into the children first (writing through the reference `(*init)[i]`), then calls `f(init)`, and refreshes `init` only if something changed (`src/csl/algo.cpp:419-429`). A node replaced by `f` is not visited again in the same call.
11. Because `f` on a parent runs after the children were replaced and before the parent is refreshed (`algo.cpp:424-428`), a key that spans several children may match a parent that already contains replaced children. This is a code reading, not verified: a unit test would decide it. The simultaneous-versus-chained policy therefore is not guaranteed by the current traversal (see section 4).
12. `Replace` does not ever fail on a missing key. Empty `from` returns silently (`replace.cpp:~56-60`, `:~72-75`). Size mismatch throws `CSLError::ValueError`.
13. Index replacement uses `Replaced(expr, Index, Index, refresh)` (`src/csl/replace.cpp:176-181`) and handles common indices through an intermediate rename step (`replace.cpp:184-200`).

### 2.4 Derivative (diff)

14. `Derived` returns `OPT(expression, derive(variable))` (`src/csl/interface.cpp:297-299`). If `derive` returns `nullopt`, the wrapper returns the input expression unchanged, not zero. Whether `Abstract::derive` returns `nullopt` by default is not verified (`src/csl/abstract.cpp` derive body not read).
15. The documented rule is: a derivative w.r.t. any expression matches equal `Abstract` or same name, and `dx/d(exp(x)) = 0` (`src/csl/abstract.h`, doc block above `derive` at `:873-881`).
16. `Variable::derive` returns `CSL_1` if equal, a `Derivative` node if the parent is not elementary and depends on the variable, and `CSL_0` otherwise (`src/csl/literal.cpp:489-499`). `Constant::derive` returns `CSL_1` or `CSL_0` (`src/csl/literal.cpp:253-256`).

### 2.5 Evaluation (evaluate)

17. `Evaluated` returns `DeepRefreshed(OPT(expression, evaluate(mode)))` (`src/csl/interface.cpp:307-314`). The mode is `csl::eval::mode = std::bitset<6>`, with bits `literal=1`, `numerical=2`, `indicial=4`, `abbreviation=8`, `all` as their union (`src/csl/evaluation.h:17-27`).
18. A symbol that is not valued is left in place: `Variable::evaluate` returns `nullopt` when `parent->isValued()` is false (`src/csl/literal.cpp:421-436`). So `Evaluated` does not fail on free symbols. The same holds for `Constant::evaluate` (`src/csl/literal.cpp:208-218`).
19. A valued symbol is replaced by its stored value, conjugated if the symbol is conjugated (`src/csl/literal.cpp:430-433`, `:213-216`).
20. The doc comment on `evaluate` says `evaluateScalar()` returns only the real scalar, e.g. `x+iy` with `x=2, y=3` gives `3` by `evaluateScalar()` but `3+3i` by `evaluate` (`src/csl/abstract.h`, doc block above `:871`).
21. `Variable::evaluateScalar` asserts validity: `CSL_ASSERT_SPEC(getValued(), CSLError::ValueError, "variable not valued.")` (`src/csl/literal.cpp:414-418`).
22. `Abstract::getValue()` prints the object (`print()`) and throws `CSLError::AbstractCallError` (`src/csl/abstract.cpp:557-561`). Non-literal nodes therefore cannot be given a numeric value by this path.
23. The value store is on the shared parent. `setValue` overloads are declared in `src/csl/literal.h:114-116` and `:227-229`; the forwarding to the parent was not re-read. Two expressions that share the same symbol see the same value. Tests set values this way: `tests/units/csl/test_numbers.cpp:31` (`a->setValue(4)`), `:71` (`a->setValue(3.2)`), `tests/units/csl/test_relabel_indices.cpp:345` (`f.setValue(1)`).
24. CSL has no `values` mapping argument. The only substitution path for a numeric value is `Replace` with a numeric `to` (which does not touch the shared parent).

### 2.6 Analysis (depends_on, free_symbols, conjugate, real, imag)

25. `Constant::dependsOn` returns `true` only for a `Constant` with the same parent (`src/csl/literal.cpp:82-88`). `Variable::dependsOn` forwards to `parent->dependsOn(expr)` (`src/csl/literal.cpp:340-342`). `VariableParent::dependsOn` is at `src/csl/variableParent.cpp:265`.
26. `AbstractParent::dependsOn` prints the type to `std::cout` and throws `CSLError::AbstractCallError` (`src/csl/parent.cpp:292-297`). Any parent class without an override therefore throws on `depends_on`. Which classes lack an override is not verified.
27. `getSubSymbols()` is pure virtual (`src/csl/abstract.h:145`). Implementations exist for numbers (`src/csl/numerical.cpp:68`), functions (`src/csl/scalarFunc.cpp:44-46`, `:356-360`, `:651-655`), elements (`src/csl/element.cpp:62`), indicial tensors (`src/csl/indicial.cpp:2887`), vectors (`src/csl/vector.h:61`), literals (`src/csl/literal.h:45`, `:131`). `Constant::getSubSymbols` is at `src/csl/literal.cpp:186` (body not read; whether it includes abbreviation-encapsulated symbols is not verified). The result is a `std::vector<Parent>`; deduplication is not verified (for `AbstractDuoFunc` it is built from two child vectors).
28. `VisitEachLeaf` and `FirstOfLeaf` are declared in `src/csl/algo.h:285-290` and `:415`. `GetSymbol(name, init)` finds the first leaf whose `getName()` equals `name` (`src/csl/interface.cpp:35-48`).
29. `GetComplexConjugate` returns `OPT(expression, getComplexConjugate())` (`src/csl/interface.cpp:362-364`). `Constant::evaluate` uses it for conjugated symbols (`literal.cpp:215-216`). Printing of a conjugated symbol uses `\overline{...}` in LaTeX (see tests in 3.4).
30. `GetRealPart` uses `OPT` (`src/csl/interface.cpp:337-339`). `GetImaginaryPart` does not use `OPT` and calls `expression->getImaginaryPart()` directly (`src/csl/interface.cpp:342-344`). The default of `getImaginaryPart` (`abstract.h:983`) is not read, so its behaviour on unsupported nodes is not verified.

### 2.7 Failure modes

31. `CallErrorImpl` throws the bare enum: `throw error;`, after printing to `std::cerr` only if `csl::option::printErrors` (`src/csl/error.cpp:76-96`). Callers must catch `CSLError` (an `enum class`), not `std::exception`.
32. `CSL_ASSERT`, `CSL_ASSERT_SPEC`, `CALL_SMERROR`, `CALL_SMERROR_SPEC` are macros over `CallErrorImpl` (`src/csl/error.h`, macros at the end of the file).
33. `pow_s` with base `0` and a negative numeric exponent, or base `CSL_INF`, prints both operands to `std::cerr` and calls `std::raise(SIGSEGV)` (`src/csl/operations.cpp:3158-3170`; `raise` at `:3161` and `:3170`). This path is reachable from `Pow` construction, and therefore from `pow_s`, `sqrt_s` and any evaluation that builds such a power. It is not a `CSLError` and it cannot be caught as one.
34. `librarytensor.h:81` calls `exit(123)`. This is in generated-library output, not on the transform path (not verified further).
35. `printErrors` default: see `src/csl/options.h:36-40` (the comment is at `:38`; the value was not read in full).

### 2.8 Option flags

36. Writes to `csl::option` exist only in ScopedProperty guards: `src/csl/indicial.cpp:2678` (`fullComparison` true), `src/csl/pseudoIntegral.cpp:229` (`freezeMerge` true), `src/csl/libraryevaluator.cpp:778` (`canonicalSumNumericalFactor` false). None is on a candidate's direct path.
37. Reads on the candidate paths are in product and sum merging (`operations.cpp:49-77`, `:1041-1074`), commutation checks (`interface.h:63-73`, `indicial.cpp:3849`), `operator<` (`abstract.cpp:1353`), and `simplification.cpp:194`. The enclosing function of each of these reads was not checked for `expand`/`factor`/`collect`.

### 2.9 Comparison paragraphs (brief)

- `factor` and `collect`. `Factor` is a whole-expression factorization with a chosen factor (or automatic); `Collect` only groups terms by a given list of factors. `collect` does not support indicial tensors, while `factor` does (`src/csl/abstract.h`, doc above `collect` at `:920-926`).
- `simplify`. `Simplify(expr, depth)` is declared with a depth argument (`src/csl/simplification.h:118-124`). Its body was not read; whether it is idempotent, or whether it reads `csl::option` flags, is not verified.
- `deep_expand`. `DeepExpand` is `Expand(full=true)` with no callback (`src/csl/interface.h:116-118`). `DeepExpandIf` is the variant with a callback. The spec's condition ("included if CSL exposes it without a callback") is met.
- `conjugate`, `real`, `imag`. `GetComplexConjugate` and `GetRealPart` are optional-based (`OPT`), so an unsupported node returns itself. `GetImaginaryPart` is not optional-based and relies on `getImaginaryPart()` (see fact 30). The conjugate of a symbol is a flag on the node (`Constant::evaluate` and `Variable::evaluate` conjugate by calling `GetComplexConjugate` on the value).

## 3. Existing tests and data

1. `tests/units/csl/csltestdata.h:91-101` (macro `TEST_EXPECTATION`) checks, for each expectation: `GetComplexConjugate` (`:91`), `Factored` (`:93`), `Expanded` (`:94`), `Evaluated(expr, mode)` for each mode (`:96`), `Derived(expr, var)` for each derivative (`:99`), and `GetRealPart`/`GetImaginaryPart` (`:84-88`). The expected values are in the test data objects (not read).
2. `tests/units/csl/test_numbers.cpp:235-245`, `TEST(csl_numbers, evaluation)`: `ASSERT_EQ(csl::Evaluated(csl::int_s(3), csl::eval::all), csl::int_s(3))`; `ASSERT_EQ(csl::Evaluated(csl::float_s(3.4), csl::eval::all), csl::float_s(3.4))`; intfraction 1/2 compared with `float_s(1./2)` by `evaluateScalar`.
3. `tests/units/csl/test_numbers.cpp:523-528`, `TEST(csl_numbers, derivative)`: `derive` of `int_s(2)`, `float_s(2.5)`, `intfraction_s(2,5)`, `complex_s(2,5)` w.r.t. `a` equals `CSL_0`.
4. `tests/units/csl/test_numbers.cpp:12-14`: `int_s(5)->dependsOn(a.get())` is false. Also `:5-17`, `getSubSymbols().empty()` for numeric types.
5. `tests/units/csl/test_numbers.cpp` (`TEST(csl_numbers, integers_complex)`): `GetRealPart(CSL_2)` evaluates to `2`, `GetImaginaryPart(CSL_2)` evaluates to `0`, `GetComplexConjugate(CSL_2)` evaluates to `2`.
6. `tests/units/csl/test_numbers.cpp:326`, `:334`, `:340`: `Evaluated(pow_s(complex_s(1,2), 4))`, `Evaluated(pow_s(complex_s(1,2), 5.6))`, and a third call. Expected values not read.
7. `tests/units/csl/test_latex.cpp:521-554`, group `EvaluatedAbbreviations`. Literal string at `:544`: `EXPECT_EQ(printLatex(Evaluated(a + b, eval::abbreviation)), "a + b");`. Other tests: `abbreviationIsReplacedByItsDefinition` (`:521`), `nestedAbbreviationsAreExpandedFully` (`:530`), `expressionWithoutAbbreviationsIsUnchanged` (`:540`), `inputIsNotModified` (`:547`).
8. `tests/units/csl/test_latex.cpp:583-585`, `TEST(LatexConjugate, complexVariableUsesStarInLatexAndTerminal)`: `GetComplexConjugate(variable_s("c", ComplexProperty::Complex))`. Expected string not read.
9. `tests/integration/src/test_physics_latex.cpp:21`: `EXPECT_EQ(latex(csl::GetComplexConjugate(e) * e), "\\overline{e}(X)e(X)");`. Literal string shown as in the source.
10. `tests/units/csl/test_metric_delta_invariant.cpp:96-105`, `TEST(MetricDeltaInvariant, replaceFlipsMetricIntoDelta)`: `Replace(g, nu, nu.getFlipped())` followed by comparison of printed output (expected string not read). This is the only `Replace` call in `tests/units` and `tests/integration` (grep `\bReplaced?\(`).
11. `tests/units/csl/test_metric_delta_invariant.cpp:130`, `TEST(MetricDeltaInvariant, refreshingAndFactoringKeepsConsistency)`: factor-related, body not read.
12. `tests/integration/src/test_flavor_groups.cpp:37,50,93,108` and `test_feynman_rules_latex.cpp:357` call `Evaluated` with `eval::abbreviation`; bodies not read.
13. `tests/units/marty/test_smparam_definition.cpp:27`: `csl::Evaluated(mty::sm_input::all_params[i], csl::eval::all)`.
14. Tests named `Subs*`, `Expand*`, `Factor*` (as in the spec's planned names), `ConjugateOfComplexConstant`, `EvaluateWithValues`, `EvaluateWithFreeSymbolsFails`: none exists (grep `TEST\(.*(Expand|Factor|Collect)` over `tests/units` and `tests/integration` found only the names in 3.11 and `test_latex.cpp`'s LatexNonCommuting/LatexMatrixChains names; grep `Subs` over `tests` found no CSL test).
15. Nothing pins chained versus simultaneous replacement in `Replace`. No test covers a key that appears only after child replacement (see fact 11).
16. `python/examples/usage/symbolic.py:55-68` (spec usage example, not a test): asserts `(x + 2*y).subs({x: y, y: x}) == y + 2*x` and `f.subs({x: y+1})`. Python API does not exist yet on this branch; these asserts were not run.
17. Git: `git ls-tree -r --name-only api/gamma` filtered for `csl/api`, `tests/api`, `python/pymarty` returned no lines; `git grep -n -iE "subs|simultaneous|evaluateScalar|free_symbols" api/gamma -- src/csl/api tests/api python` returned no lines. Branch list includes `api/gamma`, `pymarty`, `master`. The current working tree `src/csl/api/` contains only `error.h`.

## 4. Options and trade-offs

- **subs (simultaneous).**
  - A. Two-pass with placeholders, as `Swapped` does (`src/csl/interface.cpp`). Costs one extra traversal per pair, plus placeholder symbols. Gives simultaneous semantics for any keys, including compound keys, with no change to CSL. Opinion: safest with current code.
  - B. A new one-pass rule that matches every key against the original node before any child is replaced (a snapshot of the expression's structure). Costs a new traversal in `csl::` and tests for it. Opinion: cleaner semantics but more code in CSL, which the phase may not allow.
  - C. Call existing `Replace(vector, vector)` as is. Costs nothing, but its result for compound keys is only known by test, and the spec's `SubsSimultaneous` test would be the first to pin it.
- **evaluate with values.**
  - A. Replace each symbol by its numeric value with `Replace`, then `Evaluated`. No shared state. Costs one traversal per symbol. Opinion: matches "never mutates".
  - B. `setValue` on the shared parent, then `Evaluated`. Cheap, but it changes every expression that shares the symbol, and it is a global side effect. Conflicts with "evaluate never changes the original object" in the plan (`phase-1-symbolic.md:53`).
- **free_symbols.**
  - A. Use `getSubSymbols()` and deduplicate in the binding. Costs: the result depends on per-class implementations; deduplication and abbreviation handling are not verified.
  - B. New recursive helper using `VisitEachLeaf`. Costs: one new helper in `csl::`, which is a CSL change.
- **evaluate with free symbols.** `Evaluated` never fails on free symbols today (fact 18). Raising an error for `evaluate` would need an explicit check on the result; the existing error is only for `evaluateScalar` (fact 21).

## 5. Questions for the user

1. `subs`: should the Python `subs` guarantee simultaneous semantics for compound keys (options A, B, or C in section 4)? The current traversal (facts 10-11) is not proven to give it.
2. `evaluate(**values)`: may the implementation use `Replace` to supply values (no shared state) rather than `setValue` on the shared parent (fact 23)? The plan says "never mutates".
3. `evaluate` with free symbols: should it return the partially evaluated expression (current CSL behaviour, fact 18) or raise `MartyValueError`? The spec says `to_complex`/`to_double` raise; it does not say what `evaluate` does.
4. `free_symbols`: return the leaves as `Parent`s (abbreviation-expanded, as `getSubSymbols` does) or the symbols as expressions? Duplicates and abbreviation handling need a decision (fact 27).
5. Failure modes: CSL prints to `std::cerr` when `printErrors` is set (fact 31), and `pow_s(0, negative)` raises `SIGSEGV` (fact 33). Should the API layer set `printErrors` off around calls (api-conventions rule 6 forbids stderr from the API) and pre-check the `0**negative` case? `errorStopsProgram` (fact 4) has no effect, so it should not be exposed.
6. `diff` of an unsupported node: `Derived` returns the input unchanged when `derive` gives `nullopt` (fact 14). Should the Python layer treat that as an error?

## 6. Not verified

- The bodies of `Simplify` (`simplification.h:124`), `Abstract::derive` default (`abstract.cpp`), `Abstract::getImaginaryPart` default (`abstract.h:983`), `Abstract::getComplexConjugate` default (`abstract.h:999`), `AbstractParent::getValue`/`Parent::getValue` for unvalued constants.
- Whether chained replacement at ancestors happens (fact 11). Only a code reading; no test pins it, and no run was made.
- Whether `Prod::expand_if` with `inPlace=true` mutates shared children (`operations.cpp:2084-2090`), and whether expansion reaches constructors that read `freezeMerge`.
- The flag reads in the enclosing functions of `operations.cpp:1041-1074`, `1674-1753`, `indicial.cpp:3849`, `simplification.cpp:194` (only the lines were confirmed).
- Whether `operator==` (used by `find` in `scalarReplacement`) reads `fullComparison`.
- Which parent classes do not override `dependsOn` (fact 26).
- The default of `csl::option::printErrors` (`options.h`, after line 40).
- The expected string values of the tests in section 3 (only the test names and the call sites were read, except where a literal is quoted).
- `tests/integration` and `tests/system` were searched only for `Replace`/`Replaced`; the `Evaluated`, `GetComplexConjugate` and `Derived` call sites in `tests/system` were not read.
- Whether the three `Replaced` overloads at `replace.h:73-123` have the expected parameter types (only their first lines were read).
- No build was run and no test was executed (read-only investigation by design).

## Searches run

- `grep` over `src/csl/**/*.{h,cpp}` for function declarations of `WeakDeepCopy`, `DeepCopy`, `Evaluated`, `Derived`, `GetRealPart`, `GetImaginaryPart`, `GetComplexConjugate`, `GetComplexModulus`.
- `grep` over `src/csl/*.h` for `WeakDeepCopy|DeepCopy|GetComplexConjugate|GetRealPart|GetImaginaryPart`.
- `grep` over `src/csl/**` for `option::` (76 hits, all listed by file in section 2.8), `errorStopsProgram|raise\(|\bexit\(|\babort\(|std::terminate|SIGSEGV`.
- `grep` over `src/csl` for `evaluateScalar|evaluate\(|eval::mode|namespace eval`; for `dependsOn` and `dependsExplicitlyOn` bodies; for `Simplif`, `simplified\(`, `expand\(bool`.
- `grep` over `tests/**` for `Evaluated\(|Derived\(|DependsOn\(|GetComplexConjugate\(|Replaced\(|Replace\(`; over `tests/units/**` for `setValue\(|Replace\(|Replaced\(|Derived\(|Evaluated\(|DependsOn\(|Expanded\(|DeepExpanded\(`; over `tests/units/csl/**` for `TEST\(.*(Replac|Subs|Evaluat|Deriv|Depend|Expand|Factor|Collect|Conjugate|Symbol|Value)`.
- `grep` over `docs/architecture/pymarty/**` for `transform|subs|free_symbols|depends_on|evaluate|deep_expand|simplify`.
- Terminal (read-only): `grep -rn "define OPT" src/csl`; `grep -rniE "subs\(|Subs[A-Z]|Simultaneous" src/csl/api tests/api python`; `git branch -a`; `git log --oneline -3 api/gamma`; `git ls-tree -r --name-only api/gamma | grep -E "csl/api|tests/api|python/pymarty"`; `git grep -n -iE "subs|simultaneous|evaluateScalar|free_symbols" api/gamma -- src/csl/api tests/api python`; `sed -n 55,70p python/examples/usage/symbolic.py`; `grep -rniE "free_?symbol|freeSymbol|getSymbols\(|collectSymbols|SubSymbols\("` over `src tests python`; `grep -rnE "\bReplaced?\(" tests/integration tests/units`; `grep -rnE "TEST\(.*(Expand|Factor|Collect)" tests/units tests/integration`; `sed -n 26,40p src/csl/options.h`.
- Reads: `src/csl/interface.cpp` (1-260, 280-420), `src/csl/interface.h` (1-420), `src/csl/replace.cpp` (25-210, 232-330), `src/csl/replace.h` (1-240), `src/csl/algo.cpp` (412-435), `src/csl/algo.h` (60-520), `src/csl/utils.cpp` (105-125), `src/csl/literal.cpp` (60-440, 486-500), `src/csl/parent.cpp` (190-310), `src/csl/error.h`, `src/csl/error.cpp` (1-200), `src/csl/abstract.h` (860-935), `src/csl/abstract.cpp` (555-565, 1346-1358), `src/csl/operations.cpp` (2075-2110, 3140-3200), `src/csl/evaluation.h`, `src/csl/options.h` (1-60), `tests/units/csl/csltestdata.h` (60-130), `tests/units/csl/test_numbers.cpp` (1-80, 523-545), `tests/units/csl/test_metric_delta_invariant.cpp` (94-112), `docs/architecture/pymarty/process/investigator.md`, `docs/architecture/pymarty/process/README.md`.
