# Investigation: indices and tensors (pymarty phase 1)

Status: READY
Updated: 2026-11-17
Author: investigator

Scope: read-only static investigation. No build was run and no test was run. Every fact below is from reading source or test files. Line numbers are from grep or from reading the cited range; where a line was only inferred from a nearby symbol, the symbol is given too.

## 1. Question

Facts needed for the phase-1 Python API covering:

1. Space creation.
2. `Index`.
3. `Tensor` with symmetries.
4. `delta`, `metric`, `epsilon`.
5. Repeated-index contraction via `ISum` / `IProd`.
6. Relabelling.
7. Existing unit tests with literal expectations.
8. Python handle lifetime (extends section 7.1 of `docs/architecture/pymarty/phase-1/01-investigation-core-classes.md`).

## 2. Facts

### Q1. Space creation

1. Three public constructors build a `Space`:
   - `Space(name, int dim, defaultIndexNames)` at `src/csl/space.cpp:63`.
   - `Space(name, Expr symbolicDim, defaultIndexNames)` at `src/csl/space.cpp:85`. It sets `dim(-1)` as the numeric dimension, so `getDim()` returns -1 for a symbolic space.
   - `Space(name, int dim, metricName, metric, defaultIndexNames)` (signed metric) at `src/csl/space.cpp:108`.
2. Every constructor asserts that the default index-name list is non-empty (`CSL_ASSERT_SPEC`, ValueError), per the summary of `space.cpp` constructors (`src/csl/space.cpp:63-140`).
3. `buildEpsilon()` is called for the int-dimension constructor (`src/csl/space.cpp:82`) and for the signed constructor (`src/csl/space.cpp:135`). It only builds epsilon for `2 <= dim < 5` (`src/csl/space.cpp:182`). The symbolic-dimension constructor does not call it (`src/csl/space.cpp:85-107`).
4. Default index names use the form `"name;latex"`. Usage: `Space S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}}` (`tests/units/csl/test_latex.cpp:514`). The parser that splits on `;` was not read.
5. Index creation from a space:
   - `Space::generateIndex(name)` returns `Index(name, this, getID(name))` (`src/csl/space.h:400`).
   - `Space::generateIndex()` with no argument cycles through the default names (`src/csl/space.h:414`).
   - `Space::generateIndices` at `src/csl/space.h:420-435`.
6. `Space::getID(name)` is defined at `src/csl/space.h:383`. `Space::refreshIndexName` is at `src/csl/space.h:378`. `Space::resetIndexNumber` is declared at `src/csl/space.h:372`; per the summary it clears IDs and resets the default-name iterator. Test `resetsAllIdsToZero` relies on IDs being 0 after relabelling (`tests/units/csl/test_relabel_indices.cpp:169-180`).
7. Name-to-character mapping: `Space::getSpecFromIndexName` at `src/csl/space.h:338-355`. It assigns characters starting at 1 and stops at `maxChar`. After `maxChar` it breaks out of the loop and returns `specIndices[t_name]` (`src/csl/space.h:349-355`), which for a new key is the `operator[]` default (0). No error is raised. Consequence (static only): more than about 127 distinct names in one space silently collapse.
8. Predefined spaces: `buildMinkowski()` at `src/csl/space.h:440`; `Euclid_R2`, `Euclid_R3`, `Euclid_R4` at `src/csl/space.h:464`, `:469`, `:474`; `Minkowski` at `src/csl/space.h:479`.

### Q2. Index

9. `Index` members (`src/csl/index.h`): `const Space *space = nullptr;` (`:84`), `ID_type id;` (`:86`), `char nameOrValue;` (`:91`). `ID_type` is `unsigned int` (`:78`). The `cslIndex::Type` enum `{Free, Dummy, Fixed}` is near `src/csl/index.h:51`.
10. Constructors in `src/csl/index.cpp`:
    - `Index()` at `:36` (calls `setSign(false)`).
    - `Index(char)` at `:41`, which calls `setValue`. The argument type is `char`, so a value like 256 is truncated before `setValue` sees it (the truncation is static inference from the type; not run).
    - `Index(name, const Space *space, unsigned short t_id)` at `:46-53`. The ID parameter is `unsigned short` while `ID_type` is `unsigned int` (`index.h:78`); IDs above 65535 would be narrowed (static).
11. `Index::getName()` is at `src/csl/index.cpp:55-57` and returns `space->getIndexNameView(...)` with no null check. A `Fixed` index or an index with no space would dereference a null pointer (static).
12. `Index::setID` at `src/csl/index.cpp:99`. `Index::setSpace` at `src/csl/index.cpp:104-112`; it calls `setValue(getValue())` at `:109` when the index is `Fixed`, which is where a fixed index is range-checked against a later-assigned space.
13. `Index::setValue(char)` at `src/csl/index.cpp:114-123`:
    - Check at `:116`: `if (t_value < 0 or (space and t_value >= space->getDim()))`.
    - Line `:117`: `std::cout << (int) t_value << " " << space->getName() << std::endl;` runs for any negative value. If `space` is null, this is a null-pointer dereference. The `(space and ...)` guard only protects the second clause, not the `t_value < 0` clause.
    - Then `CALL_SMERROR_SPEC(CSLError::IndexError, ...)`, and on success `nameOrValue = t_value; type = cslIndex::Fixed;`.
    - Static candidate for a crash: `Index(-1)` with no space. Not run.
14. `Index::compareWithoutSign` at `src/csl/index.cpp:212-219` compares `type`, `abs(nameOrValue)`, `id`, `space`.
15. `Index::testContraction(const Index&)` at `src/csl/index.cpp:221`:
    - Returns false if either index is `Fixed` (`:222-223`).
    - Returns false unless both indices are `Free` and have the same space, `abs(nameOrValue)`, and `id` (`:224-228`).
    - Permissive-covariance check at `:231`: `if (!csl::option::permissiveCovariantIndices && space->getSignedIndex())`.
    - If the two signs are equal (`not (getSign() xor t_index.getSign())`), it prints and throws `CALL_SMERROR(CSLError::RuntimeError)` at `:234`. So a lower-lower or upper-upper contraction in a signed space raises unless permissive mode is on.
16. `Index::exactMatch` at `src/csl/index.cpp:240`; it branches on `csl::option::permissiveCovariantIndices` at `:242`. The rest of its body was not read.
17. `Index::operator==(Index)` (sign-insensitive for Free/Dummy via `std::abs(nameOrValue)`) is at `src/csl/index.cpp:270-280` (from summary; not re-read this session). `Index::operator==(int)` at `src/csl/index.cpp:282` compares the raw stored `nameOrValue`.
18. `Index::operator<` at `src/csl/index.cpp:317`. It compares the `Space` pointers first (`if (space < otherSpace)` at `:320`), so ordering depends on heap addresses and is not reproducible across runs.
19. Sign: `setSign`/`getSign`/`flipSign`/`getFlipped` exist (`src/csl/index.h`, `getFlipped` near `:190`); `setSign(false)` is the default in `Index()` (`src/csl/index.cpp:38`).

### Q3. Tensor with symmetries

20. `TensorParent` holds `std::vector<const Space *> space;` (`src/csl/indicial.h:247`).
21. `Symmetry` class at `src/csl/symmetry.h:277`: members `int dim`, `bool mustGetSpan = true`, `std::vector<Permutation> permutation` (protected). Public methods include `getDim()`, `getNPermutation()`, `getPermutation()`, and `getSymmetryOf(int i, int j) const` (`src/csl/symmetry.h:340`).
22. `TensorParent::setFullySymmetric()` at `src/csl/indicial.cpp:1066-1069` sets `fullySymmetric = true` and `fullyAntiSymmetric = false`. It does not add entries to `symmetry`.
23. `TensorParent::setFullyAntiSymmetric()` at `src/csl/indicial.cpp:1071-1074` is the mirror image.
24. `TensorParent::addSymmetry(int i1, int i2)` at `src/csl/indicial.cpp:1093`:
    - Returns immediately if `fullySymmetric` (`:1095-1096`), so it is a silent no-op after `setFullySymmetric()`.
    - Clears `fullyAntiSymmetric` if set.
    - Clears `fullyAntiSymmetric` if set (`:1097-1098`).
    - Bounds check on `i1`, `i2` against `space.size()` throws `CSLError::IndexError` (`:1101-1105`).
    - Calls `symmetry.addSymmetry(Permutation(dim, {i1, i2}), 1)` (`:1106`). The `1` is the symmetry sign for a symmetric transposition.
25. `TensorParent::addAntiSymmetry(int i1, int i2)` starts at `src/csl/indicial.cpp:1108`. It returns immediately if `fullyAntiSymmetric`, clears `fullySymmetric` otherwise, and has the same bounds check. The rest of its body (the call that records the antisymmetric permutation and its sign) was not read.
26. Symmetry data is read through `TensorParent::getSymmetry` at `src/csl/indicial.cpp:999` and `TensorParent::getPermutation` at `src/csl/indicial.cpp:1042`. Bodies not re-read this session.
27. Hermitian and setSymmetry helpers exist: `getHermitianProperty` `src/csl/indicial.cpp:1284`, `addHermitianProperty` `:1331`, `setSymmetry` `:1388`. Not read in detail.
28. `TensorParent::checkIndexRequest` at `src/csl/indicial.cpp:1558`. A `CSLError::RuntimeError` throw at `:1550` lies just before this function, in code not read. The body dereferences a pointer around `:1578-1581` (summary). Static candidate for a crash.
29. `TensorParent::operator()` at `src/csl/indicial.cpp:1801-1825` calls `checkIndexRequest` at `:1806` and `:1825`.

### Q4. delta, metric, epsilon

30. Each `Space` owns its `delta`, `metric`, `epsilon` parent objects. The int constructor builds delta with `csl::make_shared<DeltaParent>(this)` in the member-initializer list (`src/csl/space.cpp:71`). Per the summary, `DeltaParent`, `MetricParent`, `EpsilonParent` store the `this` pointer of their owning `Space`.
31. Symbolic-dimension constructor builds delta with `DeltaParent(this, false)` (summary of `src/csl/space.cpp:85-107`).
32. `GetDelta`, `GetMetric`, `GetEpsilon` (pointer and reference overloads) are declared at `src/csl/interface.h:475-480` and defined at `src/csl/interface.cpp:461-488` (summary).
33. `DeltaParent::operator()(vector<Index>)` at `src/csl/indicial.cpp:2092`, calls `checkIndexRequest` at `:2095`. `RuntimeError` throws at `:2046` and `:2073` inside `DeltaParent` (`:2011-2114`).
34. `MetricParent::operator()(vector<Index>)` at `src/csl/indicial.cpp:1980`, calls `checkIndexRequest` at `:1986`. A `RuntimeError` throw at `:1961` is inside `MetricParent` (`:1870-1995`).
35. `EpsilonParent::operator()(vector<Index>)` at `src/csl/indicial.cpp:2170`, calls `checkIndexRequest` at `:2173`. Per the summary, it returns `CSL_0` for repeated indices (body range `:2123-2179`, not re-read this session).
36. Metric is signed: `Space` with the signed constructor (`src/csl/space.cpp:108`) and `Minkowski` (`src/csl/space.h:479`). In `Index::testContraction`, a signed space requires opposite signs for a contraction (`src/csl/index.cpp:231-234`).
37. `Space::buildMinkowski()` at `src/csl/space.h:440` builds the Minkowski metric.

### Q5. Repeated-index contraction via ISum / IProd

38. `ContractIndex` is declared at `src/csl/interface.h:471` and defined at `src/csl/interface.cpp:449-451` as `OPT(expression, contractIndex(index))`.
39. `Abstract::contractIndex(index)` at `src/csl/abstract.cpp:298-303`: copies the index, calls `setFree(false)`, and calls `replaceIndex(index, dummyIndex)`. It flips one Free index to Dummy in one expression. It does not sum anything.
40. `ISum::selfCheckIndexStructure()` at `src/csl/indicial.cpp:3034`: checks that all terms of a sum share the same index structure; it returns early if the sum is empty (`:3037-3038`). A `RuntimeError` throw at `src/csl/indicial.cpp:3066` lies inside this function's range (by position; not read in full).
41. `IProd` default constructor at `src/csl/indicial.cpp:3448`. `IProd(const csl::vector_expr &, bool explicitProd)` at `src/csl/indicial.cpp:3452-3465`:
    - If `not option::freezeMerge and not explicitProd and size() > 0`: calls `mergeProducts()`, `selfCheckIndexStructure()`, and, if `option::applyChainContractions`, `applyContractionChains()`.
    - Otherwise: calls `mergeProducts()` and `selfCheckIndexStructure(true)`.
42. `Prod::mergeProducts()` at `src/csl/operations.cpp:1084` (body not read).
43. `IProd::selfCheckIndexStructure(bool explicitTimes)` at `src/csl/indicial.cpp:3671`:
    - For each argument, if `not explicitTimes` and the argument is an indicial tensor, it tries `evaluate()` and replaces the argument if it succeeds (starting at `:3676`).
    - For each free index of an argument that matches an index of another argument (`*index == fooStruct[k]`, not Fixed), and where `not getFree() or testContraction(...)` holds, it contracts when `not explicitTimes`, `option::applySelfContractions`, and `hasContractionProperty(...)` hold. The contraction uses `ContractIndex` (`src/csl/indicial.cpp:3706`) and `contraction(...)`.
    - Whether the non-contraction branch turns a matched pair into dummies, and how the `explicitTimes=true` path differs, was not read beyond `:3720`.
44. `IProd::applyContractionChains()` at `src/csl/indicial.cpp:3812` (body not read).
45. `IProd::suppressTerm` at `src/csl/indicial.cpp:3516`; falls back to `Prod::suppressTerm` (`:3519`).
46. Test evidence for a repeated dummy in a plain product (not `IProd` explicitly): `printLatex(x(p) * y(p))` == `"x_{alpha}y_{alpha}"` (`tests/units/csl/test_relabel_indices.cpp:73`). The test checks labels only, not an evaluated sum.
47. No test in `tests/units`, `tests/integration`, or `tests/api` references `ISum`, `IProd`, `addSymmetry`, `addAntiSymmetry`, `setFullySymmetric`, `setFullyAntiSymmetric`, or `ContractIndex` (grep result from the earlier session; no hits in these three test directories).

### Q6. Relabelling

48. `RelabelIndices(const Expr &expr, bool relabelFree, IndexRenaming &...)` core at `src/csl/replace.cpp:545`. Two-argument wrapper `RelabelIndices(expr, relabelFree)` at `src/csl/replace.cpp:618-621`.
49. `ApplyIndexRenaming(const Expr &expr, const IndexRenaming &renaming)` at `src/csl/replace.cpp:624`. Declarations: `src/csl/interface.h` (`IndexRenaming` at `:382`, `RelabelIndices` at `:397`, `:407-409`, `ApplyIndexRenaming` at `:419`).
50. Behaviour (from the earlier read of `replace.cpp`, summary):
    - Works on a `WeakDeepCopy` of the input (input not modified; confirmed by test `inputIsNotModified`, `test_relabel_indices.cpp:183-196`).
    - Free indices are named first, then dummies.
    - Names come from a per-space default-name pool, cycled; collisions get a `_n` suffix (test `poolWrapAroundAddsSuffixesFromOne`, `test_relabel_indices.cpp:87-105`).
    - Relabelled indices get `setID(0)`.
    - `relabelFree=false` keeps free names (test `freeIndicesKeepTheirNamesWhenNotRelabelled`, `test_relabel_indices.cpp:364-375`).
    - Default upper variance for free indices (`result = +result;`, summary).
51. Fixed indices are not renamed (test `fixedIndexIsNotRenamed`, `test_relabel_indices.cpp:340-346`, expects `"x_{1}"`).

### Q7. Existing unit tests with literal expectations

52. `tests/units/csl/test_latex.cpp` (exact literals; test-suite names for the first case were not read):
    - Test around `:494-501` (name not read): `EXPECT_EQ(latex(expr), "x_{\\alpha}y_{\\alpha}")` (`:500`) and equality of `renamed->printLaTeX(0)` (`:501`).
    - `freeIndicesAreRenamedToo` at `:503-510`: `Index f = S.generateIndex("beta")` (`:508`); `EXPECT_EQ(printLatex(z(f)), "z_{\\alpha}")` (`:509`).
    - `LatexNames.kroneckerDeltaHasLatexForm` at `:512-518`: `Space S{"S",4,{"alpha;\\alpha","beta;\\beta"}}` (`:514`), `a=S.generateIndex("alpha")` (`:515`), `b=S.generateIndex("beta")` (`:516`), `EXPECT_EQ(printLatex(S.getDelta()(std::vector<Index>{a,b})), "\\delta_{\\alpha\\beta}")` (`:517-518`).
    - `LatexSignedIndices.metricWithEqualSignsIsG` at `:558-564`: `Minkowski.generateIndex("\\mu")` (`:560`), `("\\nu")` (`:561`); `EXPECT_EQ(printLatex((*Minkowski.getMetric())(...)), "g^{\\mu\\nu}")` at `:562-563`.
    - `LatexSignedIndices.freeDeltaIndicesAreRelabelledUpper` at `:566-572`: `nu.flipSign()` (`:570`); `EXPECT_EQ(printLatex((*Minkowski.getDelta())(...)), "\\delta^{\\mu\\nu}")` at `:571-572`.
53. `tests/units/csl/test_relabel_indices.cpp` (literals):
    - `singleFreeIndexTakesFirstName` (`:30-40`): name `"alpha"` (`:38`), ID `0u` (`:39`).
    - `sameFreeIndexInSumKeepsOneLabel` (`:56-62`): `printLatex(x(f) + y(f))` == `"x_{alpha} + y_{alpha}"` (`:62`).
    - `dummyPairSharesOneLabel` (`:67-73`): `printLatex(x(p) * y(p))` == `"x_{alpha}y_{alpha}"` (`:73`).
    - `sameNameDifferentIdsAreDifferentIdentities` (`:108-115`): `printLatex(x(f) * y(p) * x(p))` == `"x_{alpha}x_{beta}y_{beta}"` (`:115`).
    - `freeIndicesTakeNamesBeforeDummies` (`:120-128`): `printLatex(x(p) * y(p) * z(f))` == `"x_{beta}y_{beta}z_{alpha}"` (`:128`).
    - `fixedIndexIsNotRenamed` (`:340-346`): `"x_{1}"` (`:346`).
    - `dummyPairsAvoidKeptFreeNames` (`:378-386`): `"x_{beta}x_{beta}z_{alpha}"` (`:386`).
    - `renamingAppliedToAnotherExpressionGivesSameLabels` (`:391-404`): `"w_{alpha}x_{beta}x_{beta}"` (`:403-404`).
    - `resetsAllIdsToZero` (`:169-180`): `index.getID() == 0u` (`:180`).
54. `tests/units/csl/test_signed_refresh.cpp`:
    - `metricWithFlippedIndexBecomesDelta` (`:30-39`): compares `printLatex(refreshed)` with `printLatex(delta)` (`:38`) and `refreshed == delta` (`:39`).
    - `deltaWithEqualSignsBecomesMetric` (`:42-52`): same pattern (`:51-52`).
    - `relabelledFlippedMetricPrintsUpperAfterRefresh` (`:79-86`): `EXPECT_EQ(printLatex(refreshed), "g^{\\mu\\nu}")` (`:86`).
55. `tests/units/csl/test_metric_delta_invariant.cpp`: `countInconsistentMetricDelta(expr) == 0` (`:33`); tests `directIndexWriteReselectsMetricAsDelta` (`:72-81`), `replaceFlipsMetricIntoDelta` (`:96-104`), `setIndexStructureReselectsParent` (`:107`), `renamingKeepsProductsConsistent` (`:117`), `refreshingAndFactoringKeepsConsistency` (`:130`). Literal strings were not extracted beyond `:81`, `:93`, `:104`, which compare printLatex output against `deltaOn(...)`, `metricOn(...)` helpers.
56. `tests/units/csl/csltestdata.h:23-32` holds shared fixtures (summary; not re-read this session).
57. `tests/units/csl/test_numbers.cpp:31, 71` call `setValue` on constants, not on indices.

### Q8. Python handle lifetime (extends core-classes section 7.1)

58. `Index` holds a raw `const Space *` (`src/csl/index.h:84`). Nothing in `Index` keeps the `Space` alive.
59. `TensorParent` holds `std::vector<const Space *> space` (`src/csl/indicial.h:247`). Raw pointers again.
60. Each `Space` owns its delta, metric, and epsilon parent objects (see fact 30). `DeltaParent` and related classes store the owning `Space`'s `this` pointer. So a delta/metric/epsilon object must not outlive its `Space`.
61. Predefined spaces have static storage: `Minkowski` (`src/csl/space.h:479`), `Euclid_R2/3/4` (`:464`, `:469`, `:474`).
62. Spaces owned elsewhere in MARTY (not CSL):
    - `ColorSpace` is allocated with raw `new` in `src/marty/core/group.cpp:233-237` (per summary), and destroyed in the group destructor (`src/marty/core/group.cpp:143-146`, per summary).
    - `VectorSpace` at `src/marty/core/mrtInterface.cpp:95` and `src/marty/core/mrtInterface.h:299`.
    - `DiracSpace : csl::Space` at `src/marty/core/diracology.h:40`.
    - `std::unique_ptr<csl::Space>` in `src/marty/core/flavor.h:77`, created in `flavor.cpp:50-56`.
63. The `Space` copy constructor is not declared deleted in the files read (not verified; see section 6).
64. `Index::operator<` compares `Space` pointers (`src/csl/index.cpp:320`); a Python sort on `Index` objects is address-dependent.
65. `Space::resetIndexNumber()` (`src/csl/space.h:372`) clears IDs, so `Index` objects created before the reset can compare equal or unequal to new ones in ways the Python user does not expect (static reasoning from `getID`, `generateIndex`).

### Static findings (crash and error candidates; none reproduced)

66. `src/csl/index.cpp:117`: null dereference of `space` when `Index(char)` receives a negative value and no space is set (`Index(-1)`).
67. `src/csl/index.cpp:57`: null dereference in `Index::getName()` for a `Fixed` index or an index without a space.
68. `src/csl/indicial.cpp:1558-1581`: `checkIndexRequest` dereferences a pointer around `:1578-1581` (summary; not re-read this session).
69. `src/csl/index.cpp:116`: for a symbolic-dimension space, `getDim()` returns -1 (`src/csl/space.cpp:85-107`), so the check `t_value >= space->getDim()` rejects every non-negative fixed value (static inference; behaviour for symbolic spaces not traced further).
70. `src/csl/space.h:349-355`: silent collapse of name characters after the pool is exhausted.
71. `src/csl/index.cpp:46-49`: `unsigned short` ID parameter versus `ID_type = unsigned int` (`src/csl/index.h:78`).
72. `Index(char)` truncation (`src/csl/index.cpp:41`): `char` argument, so out-of-range integers wrap silently.
73. `src/csl/index.cpp:231-234`: a `RuntimeError` throw in `testContraction` for same-sign contraction in signed spaces (expected behaviour, not a crash; the Python layer should map it to an exception).

## 3. Existing tests and data

- Covered by unit tests with literal expectations:
  - Delta and metric printing with LaTeX names (`test_latex.cpp:512-518`, `:558-572`).
  - Repeated dummy labels in products (`test_relabel_indices.cpp:67-73`, `:108-115`, `:120-128`, `:378-386`).
  - Relabel input immutability (`test_relabel_indices.cpp:183-196`) and ID reset (`:169-180`).
  - Signed refresh of metric and delta (`test_signed_refresh.cpp:30-86`) and metric/delta consistency (`test_metric_delta_invariant.cpp:33-130`).
- Not covered by any unit, integration, or API test (checked by grep in the earlier session):
  - `ISum`, `IProd` (explicit construction, `explicitProd` flag, `applyContractionChains`).
  - `TensorParent::addSymmetry`, `addAntiSymmetry`, `setFullySymmetric`, `setFullyAntiSymmetric`.
  - `ContractIndex`.
  - `Symmetry` class methods.
  - `EpsilonParent` values (no literal expectation found in the files read; not exhaustively checked).
- No reference data files were read for this topic; `tests/system/data/output/` is only relevant for system tests, which must not be run (AGENTS.md).
- Test fixtures: `tests/units/csl/csltestdata.h:23-32` (not re-read this session).

## 4. Options and trade-offs

Opinion: the options below are suggestions, not facts.

- Option A (keep-alive by handle). Each Python `Index`, `Tensor`, `Delta`, `Metric`, `Epsilon` handle holds a reference to its owning Python `Space` object. Trade-off: prevents dangling `Space*` (facts 58-60), with a small reference-counting cost. Recommended.
- Option B (owner-only spaces). Python `Space` objects are the only owners; the binding raises if a `Space` is destroyed while handles exist. Trade-off: simpler, but the user must manage lifetimes manually. Less recommended.
- Option C (range check in the binding). Validate fixed-index values and dimensions in the Python layer before calling `Index(char)` (facts 13, 66, 72). Trade-off: duplicates a check that C++ already does, but avoids the null dereference at `index.cpp:117` and the `char` truncation. Recommended.
- Option D (defer `resetIndexNumber`). Do not expose it in phase 1 (fact 65). Trade-off: less API surface; users who need a fresh numbering must create a new `Space`.
- Option E (pair symmetries only). Expose `addSymmetry`, `addAntiSymmetry`, `setFullySymmetric`, `setFullyAntiSymmetric` (facts 22-25) with the Python-side check that indices are in range. Trade-off: general permutations are not covered. Requires reading `symmetry.cpp` and the rest of `addAntiSymmetry` before writing the spec.
- Option F (delay ISum/IProd). Keep phase 1 to `Expr` products and `ContractIndex`-free workflows (facts 38-47). Trade-off: no explicit sum API until the semantics are verified by tests. Recommended until the contraction branch at `indicial.cpp:3671-3720` is read in full.
- Option G (literal-based examples). Build the phase-1 usage example from literals already in tests (e.g. `\delta_{\alpha\beta}`, `g^{\mu\nu}`, `x_{alpha}y_{alpha}`), so that asserted values are backed by existing tests (facts 52-55).

## 5. Questions for the user

1. Should the Python binding raise an exception on negative or out-of-range fixed indices before reaching C++ (Option C), or is it acceptable to rely on the C++ check?
2. Should Python `Space` handles keep their C++ `Space` alive through every dependent handle (Option A), or should lifetimes be managed by the user (Option B)?
3. Is `resetIndexNumber` in phase-1 scope (Option D)?
4. Is the phase-1 symmetry API limited to pair symmetries and full (anti)symmetry (Option E), or does it need general permutations?
5. Are `ISum` and `IProd` in phase-1 scope, or only `Expr` products and their printing (Option F)?
6. Should the usage example assert printed LaTeX strings (as the existing tests do) or structural properties of the result?

## 6. Not verified

- `src/csl/symmetry.cpp` was not read (sign convention, `addSymmetry` body, `getSymmetryOf` body).
- Tail of `TensorParent::addAntiSymmetry` (after `src/csl/indicial.cpp:1115`) was not read: the sign passed for antisymmetry is not confirmed.
- `ISum` constructors (`src/csl/indicial.cpp:3000-3011`) and the body of `ISum::selfCheckIndexStructure` after `:3040` were not read.
- `IProd::selfCheckIndexStructure` after `:3720` (non-contraction branch, `explicitTimes=true` path) and `IProd::applyContractionChains` (`:3812`) were not read.
- `Prod::mergeProducts` (`src/csl/operations.cpp:1084`) was not read.
- `src/csl/index_new.h` and `src/csl/index_new.cpp` were not compiled or read in full; the summary notes suspicious code (`indices[0] == first;`, `operator+` without a return), and `index_new.cpp` is listed in `src/csl/CMakeLists.txt:45-46`. Compile status unknown.
- `Space` copy constructor and copy-assignment: not checked for `= delete`.
- Parsing of `"name;latex"` default names (the `;` split) was not read.
- `Space::getSpecFromIndexName` behaviour after the pool is exhausted was inferred from code, not run.
- No build was run and no test was executed. All crash candidates (facts 66-72) are static and unverified by run.
- Symbolic-dimension spaces: behaviour of `Index` fixed values and `getDim()` == -1 in each caller was not traced.
- `EpsilonParent::operator()` body (`src/csl/indicial.cpp:2123-2179`) was not re-read this session; "returns `CSL_0` for repeated indices" comes from the earlier read.
- `csltestdata.h` fixtures were not re-read this session.
- The `index.h` enum location (`cslIndex::Type`) is cited as "near line 51" from the earlier read; not re-grepped.
