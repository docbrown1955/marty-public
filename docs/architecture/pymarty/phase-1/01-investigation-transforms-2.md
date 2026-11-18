Status: READY
Updated: 2026-11-17
Author: investigator

# Investigation 2: CSL transforms exposed to Python without callbacks

Gap-filling for `01-investigation-transforms.md`, which left these six transforms as "not checked":
`factor`, `collect`, `simplify`, `conjugate`, `real`, `imag`.

Citation convention: `file:line` is a line read in this investigation. Items tagged **[E]** come from an
earlier read in the same investigation and were not re-checked in the final pass. Items tagged
**not verified** were not read.

## Question

For each of the six transforms: which C++ entry points exist, whether they mutate or return a new `Expr`,
which option flags they touch, what the existing tests pin, how they behave on no-op and complex-property
inputs, how they fail, and whether argument order matters.

## Facts

### Entry points (interface layer)

1. `Factor(Expr&, bool full)` mutates in place: `expression = OPT(expression, factor(full))`. `src/csl/interface.cpp:115-118`.
2. `Factored(const Expr&, bool full)` returns a new `Expr` via `OPT`. `src/csl/interface.cpp:120-122`.
3. Overloads `Factor(Expr&, Expr_info, bool)` and `Factored(const Expr&, Expr_info, bool)`. `src/csl/interface.cpp:125-133`.
4. `OPT(expr, func)` is `((expr->func).value_or(expr))`, so a `nullopt` result keeps the input. `src/csl/interface.cpp:29`.
5. `Collect(Expr&, std::vector<Expr> const&)` calls `expr->collect(factors, false)` and assigns only when the result has a value. `src/csl/interface.cpp:135-139`.
6. `DeepCollect` is the same with `true`. `src/csl/interface.cpp:141-145`.
7. `Collected(const Expr&, ...)` copies, then calls `Collect`. `src/csl/interface.cpp:147-150`.
8. `GetRealPart` = `OPT(expression, getRealPart())`. `src/csl/interface.cpp:337-339`.
9. `GetImaginaryPart` = `expression->getImaginaryPart()`, with no `OPT`. The method returns `Expr` (not `optional`), so `OPT` is not needed. `src/csl/interface.cpp:342-344`. This is style only, not a behaviour difference.
10. `GetComplexConjugate` = `OPT(expression, getComplexConjugate())`. `src/csl/interface.cpp:362-364`.
11. `Simplify` is declared as `Expr Simplify(const Expr &expr, int depth = -1);`. `src/csl/simplification.h:124`.
12. The Doxygen block says `depth ... Default=1: **max depth**`, while the signature default is `-1`. `src/csl/simplification.h:121` vs `:124`.

### Base-class defaults

13. `Abstract::getRealPart()` returns `nullopt`. `src/csl/abstract.cpp:645-647`.
14. `Abstract::getImaginaryPart()` returns `CSL_0`. `src/csl/abstract.cpp:650-652`.
15. `Abstract::getComplexConjugate()` returns `nullopt`. `src/csl/abstract.cpp:665-667`.
16. `Abstract::factor(bool)`, `factor(Expr_info, bool)`, and `collect(...)` all return `nullopt`. `src/csl/abstract.cpp:735-737`, `:739-741`, `:744-746`.
17. `AbstractBuildingBlock::factor(bool)` and `factor(Expr_info, bool)` return `nullopt`. `src/csl/buildingBlock.cpp:37-42`.
18. `AbstractBuildingBlock::getRealPart()` returns `nullopt`. `src/csl/buildingBlock.cpp:50`.

### Complexified (base of literals)

19. `AbstractLiteral : public Complexified`. `src/csl/literal.h:35`.
20. `Constant : AbstractLiteral` (`src/csl/literal.h:55`), `Variable : AbstractLiteral` (`:162`), `Imaginary : AbstractLiteral` (`:383`). Constant and Variable have no real/imag/conjugate override in `literal.h`. Their behaviour is the inherited `Complexified` one.
21. `Complexified::isComplexConjugate()` at `src/csl/buildingBlock.cpp:103`. `Complexified::setConjugated` at `:164`.
22. `Complexified::getRealPart()`: `nullopt` if `isReal()` (`:110`); `CSL_0` if purely imaginary; otherwise `real_s(Copy(this))`. `src/csl/buildingBlock.cpp:108-115`.
23. `Complexified::getImaginaryPart()`: `CSL_0` if real; `-CSL_I * copy()` if purely imaginary; otherwise `imaginary_s(Copy(this))`. `src/csl/buildingBlock.cpp:117-124`.
24. `Complexified::getComplexConjugate()`: `copy()` if `Real`; `-copy()` if `Imaginary`; otherwise a copy with `setConjugated(not isComplexConjugate())`. `src/csl/buildingBlock.cpp:126-137`.
25. `CSL_I` is `make_shared<Imaginary>()`. `src/csl/literal.h:536`.
26. `Imaginary` overrides `getRealPart` = `CSL_0` and `getImaginaryPart` = `CSL_1`. `src/csl/literal.cpp:507-514` [E]. Other overrides **not verified**.
27. `Constant::evaluate` returns `GetComplexConjugate(value)` when `conjugated` is set. `src/csl/literal.cpp:212-219` [E].
28. `Real` and `Imaginary` values of the complex property are used in `src/csl/buildingBlock.cpp:110-137`. The enum definition location is **not verified**.

### Abstract function and composite classes

29. `Sum : AbstractMultiFunc`. `src/csl/operations.h:34`.
30. `Polynomial : AbstractMultiFunc`. `src/csl/operations.h:182`.
31. `Prod : AbstractMultiFunc`. `src/csl/operations.h:287`. `Prod` declares `getRealPart` (`:342`) and `getImaginaryPart` (`:344`), but no `factor`, `collect`, or `getComplexConjugate`.
32. `Pow : AbstractDuoFunc`. `src/csl/operations.h:485`.
33. `ISum : Sum` (`src/csl/indicial.h:1016`), with a `factor(bool)` override at `:1059` (**not verified**). Body at `src/csl/indicial.cpp:3326` **not verified**.
34. `AbstractFunc` (`src/csl/scalarFunc.h:38`): `getRealPart` = `real_s(copy())` always (`src/csl/scalarFunc.cpp:101-103`). `getImaginaryPart` = `imaginary_s(copy())` always (`:106-108`). `getComplexConjugate` starts at `:122` [E] and recurses into the argument.
35. `AbstractMultiFunc` (`src/csl/scalarFunc.h:334`): `getRealPart` = `real_s(copy())` always (`src/csl/scalarFunc.cpp:774-776`). `getImaginaryPart` = `imaginary_s(copy())` always (`:779-781`). So `GetRealPart` and `GetImaginaryPart` return a wrapper node, not the input, for any `AbstractMultiFunc` that does not override them.
36. `AbstractMultiFunc::getComplexConjugate` (`src/csl/scalarFunc.cpp:795-811`): maps `getComplexConjugate` over `argument`. If at least one argument yields a value, it returns `Refreshed` copy with each argument replaced by its conjugate or left unchanged. Otherwise `nullopt`.
37. `AbstractMultiFunc::factor(bool)` (`src/csl/scalarFunc.cpp:871-892`): recurses only when `full`. Returns `Refreshed(res)` if any argument factored, else `nullopt`.
38. `AbstractMultiFunc::factor(Expr_info, bool)` (`src/csl/scalarFunc.cpp:894-915`): same shape as 37.
39. `AbstractMultiFunc::collect` (`src/csl/scalarFunc.cpp:917-`): recurses into arguments only when `full`. Tail **not verified**.
40. `AbstractDuoFunc` has `getRealPart` (`:430`), `getImaginaryPart` (`:435`), `getComplexConjugate` (`:451`), `factor` (`:504`, `:520`), and `collect` (`:536`) [E].
41. `AbstractFunc::factor(bool)` (`:188-199`), `factor(Expr_info, bool)` (`:201-212`), `collect` (`:214-`) [E].
42. `Sum::getRealPart` maps `GetRealPart` over arguments (`src/csl/operations.cpp:110-117`) [E]. `Sum::getImaginaryPart` at `:119-125` [E].
43. `Sum::factor(bool)` at `src/csl/operations.cpp:694-731` [E]. It returns `nullopt` unless every argument equals the factor or contains it. Helpers `gatherFactors` (`:596`) and `clearRedundantFactors` (`:638`).
44. `Sum::factor(Expr_info, bool)` at `src/csl/operations.cpp:732-778` [E]. If `full`, it factors each argument first. It returns a value only if all arguments matched.
45. `Sum::collect` at `src/csl/operations.cpp:779-786`. The check is `CSL_ASSERT_SPEC(!f->isIndexed(), CSLError::TypeError, "Indexed factors are not yet supported for the collect function")` (`:782-786`). No check that factors are symbols.
46. `Prod::getRealPart` at `src/csl/operations.cpp:1281-1299` (read). Multiplies out real and imaginary parts term by term, and always returns a value.
47. `Prod::getImaginaryPart` at `src/csl/operations.cpp:1301-1318` (read). Same structure.
48. `Polynomial::factor(bool)` at `src/csl/operations.cpp:3604`. The parameter is unnamed, so `full` is ignored. Size-3 arguments take a canonical quadratic route. Rest **not verified**.

### Numerical

49. `Complex` overrides at `src/csl/numerical.h:436`, `:438`, `:444` (class line **not verified**). Bodies at `src/csl/numerical.cpp:832-856` [E]: `getRealPart` returns `real`, `getImaginaryPart` returns `imag`, and `getComplexConjugate` returns `real` when `imag == CSL_0`, else `Complex(real, -imag)`.

### Option flags

50. Flag definitions: `src/csl/options.h:20-60` [E]. Ten flags: `errorStopsProgram`, `checkCommutations`, `freezeMerge`, `canonicalSumNumericalFactor`, `applySelfContractions`, `applyChainContractions`, `fullComparison`, `permissiveCovariantIndices`, `printIndexIds`, `printErrors`.
51. `fullComparison` is read at `src/csl/abstract.cpp:1353` [E] and `src/csl/simplification.cpp:194` [E] (inside `ruleO1`).
52. `canonicalSumNumericalFactor` is read at `src/csl/operations.cpp:2429`, `:2437` [E]. `freezeMerge` at `src/csl/operations.cpp:49-77`, `:1041-1074` [E]. These are in sum and product construction helpers, not in the transform bodies read here.
53. None of the transform bodies read in this investigation (`Factor`, `Collect`, `Sum::factor`, `Sum::collect` up to line 800, `Complexified::*`, `AbstractMultiFunc::*`, `Prod::getRealPart`/`getImaginaryPart`) reads an option flag directly.
54. Error printing: `src/csl/error.cpp:76-96` [E]. `CallErrorImpl` throws an enum. Printing is gated by `printErrors` (`:80`, `:92`) [E].

### Simplify

55. Repo-wide search for `(Deep)?Simplify(` in `src`, `tests`, and `examples`: the only `csl::Simplify` hit is the declaration at `src/csl/simplification.h:124`. Other hits are `sgl::Simplify` (`src/marty/sgl/simplify.cpp:154`, `src/marty/core/sglsimplify.cpp:145`), a different function in a different namespace.
56. `src/csl/algorithms/simplification.cpp` is 0 bytes.
57. Not checked: whether any other `simplify` member in `src/csl/` shadows the name. The grep in fact 55 matched only the capitalised `Simplify(` form.

### Tests

58. `Collect`, `Collected`, `DeepCollect`, `Simplify` (including `Deep`): 0 call sites under `tests/` (grep this pass).
59. `Factor(` or `Factored(` called directly under `tests/`: 1 hit, the macro at `tests/units/csl/csltestdata.h:93`. All other factor-named calls in tests are `DeepHardFactored` (see below). `DeepHardFactored` is a different function from `Factor`.
60. `ExprExpectation` defaults: `real(expr_)` (`tests/units/csl/csltestdata.h:65`), `imag(CSL_0)` (`:66`), `conjugate(expr_)` (`:69`), `factored(expr_)` (`:70`).
61. `TEST_EXPECTATION` asserts `GetRealPart` (`:87`), `GetImaginaryPart` (`:88`), `GetComplexConjugate` (`:91`), and `Factored` (`:93`). Macro starts at `:82`.
62. No test sets `.factored`, `.conjugate`, `.real`, or `.imag` to a non-default value outside `test_literals.cpp` (earlier grep [E]).

## Per-transform findings

### 1. factor

(1) Entry: `Factor` (mutating) at `interface.cpp:115-118` and `Factored` (new `Expr`) at `:120-122`. Expr_info overloads at `:125-133`. Method: `optional<Expr> factor(bool full)`. Overrides: `Sum` (`operations.h:149`, `:155`, [E]), `Polynomial` (`operations.h:275`, [E]), `ISum` (`indicial.h:1059`, **not verified**). The `AbstractFunc`, `AbstractDuoFunc`, `AbstractMultiFunc`, and `Vector` override lines **not verified**. `Prod`/`Pow` use the inherited base versions. The base `Abstract` returns `nullopt` (fact 16).

(2) Flags: none read in the bodies listed in fact 53.

(3) Expected results from tests: only the identity assertion `Factored(expr) == expr` for every `ExprExpectation` by default (`csltestdata.h:70`, asserted at `:93`). This runs through `csl_literals.constant` (`tests/units/csl/test_literals.cpp:5`, `TEST_EXPECTATION` at `:16`) and `csl_literals.variables` (`:19`, `TEST_EXPECTATION` at `:34`). No test asserts a non-identity factored result. Test name that produces a factored value: none found.

(4) No-op and ComplexProperty: `factor` has no ComplexProperty branch in the code read. Atoms (`AbstractBuildingBlock`, fact 17) return `nullopt`, so `Factored` returns the input. For `Sum`, no common factor gives `nullopt`, so identity (fact 43).

(5) Failure modes: none read that throw in the factor bodies. `Sum::factor(Expr_info)` returns `nullopt` on partial matches (fact 44), which is silent identity, not an error.

(6) Argument order: `Factor(Expr&, Expr_info, bool)` takes the expression first, then the factor, then `full` (`interface.cpp:125`). `Factored` has the same order. No `Collect`-style vector. Order dependence inside `gatherFactors` **not verified**.

### 2. collect

(1) Entry: `Collect` (mutating, `full=false`) at `interface.cpp:135-139`. `DeepCollect` (mutating, `full=true`) at `:141-145`. `Collected` (new `Expr`) at `:147-`. Method: `optional<Expr> collect(std::vector<Expr> const&, bool full)`. Bodies: `Sum` (`operations.cpp:779`), `AbstractFunc` (`scalarFunc.cpp:214`), `AbstractDuoFunc` (`scalarFunc.cpp:536`), `AbstractMultiFunc` (`scalarFunc.cpp:917`). `Prod` and `Polynomial` inherit `AbstractMultiFunc::collect`. Header override lines **not verified**. Base `Abstract::collect` returns `nullopt` (fact 16).

(2) Flags: none read in `Sum::collect` (read through line 800 only; the remainder is **[E]** and reads no flags).

(3) Expected results from tests: none. Grep count 0 for `Collect`, `Collected`, `DeepCollect` under `tests/` (fact 58).

(4) No-op: if `collect` returns `nullopt`, `Collect` leaves the expression unchanged (`interface.cpp:137-138`). No ComplexProperty branch in `collect`.

(5) Failure modes: indexed factor triggers `CSL_ASSERT_SPEC` with `CSLError::TypeError` (`operations.cpp:782-786`). The throw mechanism is `CallErrorImpl` (`error.cpp:76-96`, fact 54) [E]. The macro body **not verified**.

(6) Argument order and symbol-only: the factors vector is taken as a whole (`interface.cpp:135`). Only indexed factors are rejected (`operations.cpp:782-786`). Symbols are not enforced. The effect of factor order on the result **not verified** (the body after line 800 is **not verified**).

### 3. simplify

(1) Entry: `Expr Simplify(const Expr &expr, int depth = -1)` declared at `src/csl/simplification.h:124`, with Doxygen at `:118-123`. Returns a new `Expr` by signature. No mutating variant seen.

(2) Flags: none. `fullComparison` is read in `ruleO1` at `simplification.cpp:194` (fact 51), a rule helper, not `Simplify`.

(3) Expected results from tests: none. `Simplify(` has 0 call sites under `tests/` (fact 58). `simplified(` in tests refers to `sgl::Simplify` (fact 55).

(4) No-op: not applicable, because no definition was found.

(5) Failure modes: no definition exists (fact 55). Any TU that calls `csl::Simplify` would fail at link time. This is an inference from the grep, **not verified by building**.

(6) Argument order: `expr` then `depth`. Documented default conflicts (fact 12). The depth semantics **not verified** because no body exists.

### 4. conjugate

(1) Entry: `Expr GetComplexConjugate(const Expr&)` at `interface.cpp:362-364` (new `Expr`, no mutation). Method: `virtual optional<Expr> getComplexConjugate() const` at `abstract.h:999` (summary read). Bodies: `Complexified` (`buildingBlock.cpp:126-137`), `Complex` (`numerical.cpp:832-856` [E]), `AbstractFunc` (`scalarFunc.cpp:122`), `AbstractDuoFunc` (`scalarFunc.cpp:451`), `AbstractMultiFunc` (`scalarFunc.cpp:795-811`), `TensorElement` (`indicial.cpp:2263-2268` [E]). Other overrides (`Vector`, `TensorField`, `cslcomplex.h`) **not verified**. `Sum`, `Prod`, `Polynomial` inherit `AbstractMultiFunc`. `Pow` inherits `AbstractDuoFunc`. Constant and Variable inherit `Complexified` (facts 19-24).

(2) Flags: none read in the conjugate bodies. `Constant::evaluate` reads the `conjugated` state, not an option (`literal.cpp:212-219` [E]).

(3) Expected results from tests (literal values):
- `csl_numbers.integers_complex` (`tests/units/csl/test_numbers.cpp:42`): `GetComplexConjugate(CSL_2)` evaluates to `2` (`:54`).
- `csl_numbers.floats_complex` (`:79`): `GetComplexConjugate(CSL_PI)` evaluates to `M_PI` (`:87`).
- `csl_numbers.rationals_complex` (`:138`): `GetComplexConjugate(CSL_HALF)` evaluates to `0.5` (`:146`).
- `csl_numbers.complex` (`:149`): `GetComplexConjugate(make_shared<Complex>(-1,0))` evaluates to `-1` (`:217`). `GetComplexConjugate(complex_s(0,1)) == complex_s(0,-1)` (`:222`). `(0,0) -> (0,0)` (`:224`). `(1,0) -> (1,0)` (`:226`). `(1,1) -> (1,-1)` (`:228`).
- `csl_literals.constant` (`tests/units/csl/test_literals.cpp:5`): for `c1` (Complex), expected conjugate is `c1->copy()` with `setConjugated(true)` (`:14`).
- `csl_literals.variables` (`:19`): for `z1` (Complex), the same pattern (`:32`).
- `LatexConjugate.complexVariableUsesStarInLatexAndTerminal` (`tests/units/csl/test_latex.cpp:583`): `GetComplexConjugate(variable_s("c", Complex))` prints `c^{*}` in LaTeX (`:586`) and `c^(*)` on the terminal (`:589`).
- `PhysicsLatex.fermionMassTerm` (`tests/integration/src/test_physics_latex.cpp:16`, assertion `:21`): `\\overline{e}(X)e(X)`.
- `PhysicsLatex.gaugeBosonConjugate` (`:69`, assertion `:73`): `W_{\\mu}(X)^{\\dagger}`.
- `PhysicsLatex.gaugeBosonKineticSquare` (`:77`, assertion `:82`): `W_{\\mu}(X)W^{\\mu}(X)^{\\dagger}`.

(4) No-op and ComplexProperty:
- `Real`: `Complexified::getComplexConjugate` returns `copy()` (`buildingBlock.cpp:127-128`). So the result is a copy, not `nullopt`. `Complex` real literal `CSL_2` returns `2` (`test_numbers.cpp:53-55`).
- `Imaginary`: returns `-copy()` (`buildingBlock.cpp:129-130`). `CSL_I` conjugate should be `-CSL_I` (not run; see proposed test).
- `Complex`: copies and toggles `conjugated` (`buildingBlock.cpp:132-136`). Toggling twice should restore the original (`isComplexConjugate` `:103`, `setConjugated` `:164`). The `setConjugated` body is **not verified**.
- Nodes that do not override: `Abstract::getComplexConjugate` returns `nullopt`, so `GetComplexConjugate` returns the input unchanged (`abstract.cpp:665-667`, `interface.cpp:362-364`). Which concrete classes take this path **not verified**.
- Composite: `AbstractMultiFunc::getComplexConjugate` returns `nullopt` only if no argument produces a value (fact 36).

(5) Failure modes: none read in the conjugate bodies. `evaluateScalar()` on a complex value throws `CSLError` (`tests/units/csl/test_numbers.cpp:220`), which matters for Python numeric conversion, not for the transform itself.

(6) Argument order: unary, n/a.

### 5. real

(1) Entry: `Expr GetRealPart(const Expr&)` at `interface.cpp:337-339` (new `Expr`). Method: `virtual optional<Expr> getRealPart() const` at `abstract.h` (line not re-checked; fact 13 gives the default). Overrides: `Sum` (`operations.h:94`, body `operations.cpp:110`), `Prod` (`:342`, body `:1281`), `Complexified` (`buildingBlock.cpp:108`), `AbstractFunc` (`scalarFunc.cpp:101`), `AbstractDuoFunc` (`:430`), `AbstractMultiFunc` (`:774`), `Complex` (`numerical.h:436`, body `numerical.cpp:832`), `ISum` inherits `Sum`. The `Vector` override is **not verified**. `Polynomial` inherits `AbstractMultiFunc`.

(2) Flags: none read.

(3) Expected results from tests:
- `integers_complex` (`test_numbers.cpp:42`): `GetRealPart(CSL_2) = 2` (`:46`).
- `floats_complex` (`:79`): `GetRealPart(CSL_PI) = M_PI` (`:83`).
- `rationals_complex` (`:138`): `GetRealPart(CSL_HALF) = 0.5` (`:142`).
- `complex` (`:149`): `GetRealPart(complex_s(1,0)) = 1` (`:152`).
- `csl_numbers.operations3` (`:450`, assertions `:453-497` [E]): real parts of sums, products, and divisions with `cplx`. Exact values **not verified** in this pass.
- `csl_literals.constant` / `csl_literals.variables`: `GetRealPart(Complex)` = `RealPart(expr)` (`test_literals.cpp:10`, `:28`); `GetRealPart(Real)` = input (`csltestdata.h:65`).

(4) No-op and ComplexProperty:
- `Real` variable or constant: `nullopt`, so the input is returned (`buildingBlock.cpp:110-111`). Test default `real(expr_)` (`csltestdata.h:65`).
- `Complex` variable or constant: `real_s(copy)` (`buildingBlock.cpp:114`), not the input. Test expectation `RealPart(expr)` (`test_literals.cpp:10`).
- `Imaginary`: `CSL_0` (`literal.cpp:507-509` [E]).
- `AbstractFunc`, `AbstractMultiFunc`: always wraps in `RealPart` (fact 34, 35). Not the input.
- `Prod`: multiplies out terms and always returns a value (fact 46).

(5) Failure modes: none read.

(6) Argument order: unary, n/a.

### 6. imag

(1) Entry: `Expr GetImaginaryPart(const Expr&)` at `interface.cpp:342-344` (new `Expr`, no `OPT`, fact 9). Method: `virtual Expr getImaginaryPart() const` (returns `Expr`, not `optional`; `Abstract` default at `abstract.cpp:650-652`). Overrides: `Sum` (`operations.h:96`, body `operations.cpp:119`), `Prod` (`:344`, body `:1301`), `Complexified` (`buildingBlock.cpp:117`), `AbstractFunc` (`scalarFunc.cpp:106`), `AbstractDuoFunc` (`:435`), `AbstractMultiFunc` (`:779`), `Complex` (`numerical.h:438`). The `Vector` override is **not verified**.

(2) Flags: none read.

(3) Expected results from tests:
- `integers_complex` (`test_numbers.cpp:42`): `GetImaginaryPart(CSL_2) = 0` (`:47`).
- `floats_complex` (`:79`): `GetImaginaryPart(CSL_PI) = 0` (`:84`).
- `rationals_complex` (`:138`): `GetImaginaryPart(CSL_HALF) = 0` (`:143`).
- `complex` (`:149`): imaginary-part assertions exist in this test. Exact values and lines **not verified** in this pass.
- `operations3` (`:450`, assertions `:453-497` [E]): imaginary parts with `cplx`. Exact values **not verified**.
- `csl_literals.constant` / `variables`: `imag(CSL_0)` default (`csltestdata.h:66`); `ImaginaryPart(expr)` for Complex (`test_literals.cpp:11`, `:29`).

(4) No-op and ComplexProperty:
- `Real` variable or constant: returns `CSL_0` (`buildingBlock.cpp:119-120`). This differs from `GetRealPart`, which returns the input for Real.
- `Imaginary`: `CSL_1` (`literal.cpp:510-512` [E]).
- `Complex`: `imaginary_s(Copy(this))` (`buildingBlock.cpp:122`).
- Unsupported nodes: `Abstract` default returns `CSL_0` (`abstract.cpp:650-652`). So an unsupported node yields `0`, while `GetRealPart` returns the input for the same node. This is the asymmetry in open question 2.

(5) Failure modes: none read.

(6) Argument order: unary, n/a.

## Existing tests and data (summary table)

| Transform | Tests calling it directly | Literal expectations found |
|---|---|---|
| factor (`Factor`/`Factored`) | 0 direct; 1 macro call (`csltestdata.h:93`) | identity only (`csltestdata.h:70`) |
| collect (`Collect`/`DeepCollect`/`Collected`) | 0 | none |
| simplify (`Simplify`) | 0 | none (no definition) |
| conjugate (`GetComplexConjugate`) | many (see fact list) | `2`, `M_PI`, `0.5`, `-1`, `(0,-1)`, `(1,-1)`, `c^{*}`, `\\overline{e}(X)e(X)` |
| real (`GetRealPart`) | several (`test_numbers.cpp`, `csltestdata.h`; count not verified) | `2`, `M_PI`, `0.5`, `1` |
| imag (`GetImaginaryPart`) | several (same files; count not verified) | `0` |

## Proposed C++ tests to pin behaviour

Unrun. Expected values are proposals and must be confirmed by building and running.

**factor**
- F1: `Factored(x + y)` returns the input (`Sum::factor` returns `nullopt` with no common factor, `operations.cpp:694-731`).
- F2: `Factored(CSL_2 * x + CSL_2 * y, false)` yields a product with factor `2` (exact tree shape **to be confirmed**).

**collect**
- C1: `Collected(x + y, {z})` returns `x + y` unchanged (no `z` term).
- C2: `Collected(expr, {indexed_tensor(i)})` throws `CSLError` of type `TypeError` (`operations.cpp:782-786`). Exception type to be confirmed.

**simplify**
- S1: blocked: no definition exists. Propose a test only after a definition is decided. Candidate: `Simplify(x) == x` for atomic `x`.
- S2: blocked: depth default to be confirmed after resolving the doc mismatch (`simplification.h:121` vs `:124`).

**conjugate**
- J1: `GetComplexConjugate(GetComplexConjugate(z1)) == z1` for a Complex variable (involution through `conjugated` toggle, `buildingBlock.cpp:132-136`).
- J2: `GetComplexConjugate(CSL_I) == -CSL_I` (`buildingBlock.cpp:129-130`, `literal.h:536`).

**real**
- R1: `GetRealPart(variable_s("x")) == x` for a Real variable (`buildingBlock.cpp:110-111`).
- R2: `GetRealPart(CSL_I) == CSL_0` (`literal.cpp:507-509` [E]).

**imag**
- I1: `GetImaginaryPart(variable_s("x")) == CSL_0` for a Real variable (`buildingBlock.cpp:119-120`).
- I2: `GetImaginaryPart(CSL_I) == CSL_1` (`literal.cpp:510-512` [E]).

## Open questions for the user

1. `csl::Simplify` is declared (`src/csl/simplification.h:124`) but has no definition in the repo. Should it be exposed to Python, or removed from the spec?
2. The depth default is `-1` in the signature but `Default=1` in the doc (`src/csl/simplification.h:121` vs `:124`). Which is intended?
3. For unsupported nodes, `GetRealPart` and `GetComplexConjugate` return the input unchanged, but `GetImaginaryPart` returns `CSL_0` (`abstract.cpp:650-652`). Should the Python binding keep this asymmetry?
4. `Collect` accepts any non-indexed `Expr` as a factor (`operations.cpp:782-786`). Should the Python binding restrict factors to symbols?
5. `Factor` and `Collect` mutate in place in C++ (`interface.cpp:115-118`, `:135-139`), while `Factored` and `Collected` return new values. Should Python expose mutating or returning forms, or both?
6. `DeepCollect` has no test coverage (fact 58). Should it be exposed in the first Python phase?

## Not verified

- Body of `Sum::collect` after line 800, and of `AbstractMultiFunc::collect` after line 917.
- Body of `ISum::factor` (`src/csl/indicial.cpp:3326`).
- Body of `Polynomial::factor` after its first lines (`src/csl/operations.cpp:3604`).
- Body of `Abstract` and `Complexified` setter `setConjugated` beyond the line 164 entry (`src/csl/buildingBlock.cpp:164`).
- Which concrete classes reach the `Abstract` default for conjugate or imag (the default path at `abstract.cpp:650-667`).
- Whether `CSL_ASSERT_SPEC` throws `CSLError` directly.
- Exact line numbers for `test_numbers.cpp` `complex` assertions at `:157-161`.
- Definition of `cplx` in `test_numbers.cpp` `operations2`/`operations3`.
- `cslcomplex.h:76`, `:126` (class identity).
- System tests under `tests/system/` (not read; must not be run, per `AGENTS.md`).
- Whether `Sum::factor` or `Sum::collect` order-dependence affects results.
- Header line numbers in the tables not marked [E] (`scalarFunc.h`, `vector.h`, `buildingBlock.h`, `tensorField.h`, `cslcomplex.h`, `indicial.h:1059`), and the test counts in the summary table.
- Any build or run result: nothing was built or executed in this pass.
