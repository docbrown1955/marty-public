Status: READY
Updated: 2026-11-17
Author: investigator

# Phase 1 investigation: existing CSL tests to port, and wiring for `pymarty.symbolic`

Branch `pymarty`, HEAD `8180807` (Phase 0 is `eaa9ce6`). Read-only: no build, no pytest run, no edits outside this file.
`git status` before writing: only `docs/architecture/pymarty/phase-1/` untracked (this file and `01-investigation-core-classes.md`).
Line numbers come from reads and greps in this session. Candidate classes are suggestions for the spec, not decisions.

Candidate classes used in the tables:

- `P`: port to Python; the feature is already in the phase-1 table of [../phase-1-symbolic.md](../phase-1-symbolic.md).
- `P+`: port only after the spec adds a feature that the phase-1 table lacks (named in section 4).
- `L1`: C++ API test only, no Python exposure planned (reason in the row).
- `X`: exempt (reason in the row).

## 1. Question

Phase 1 acceptance criterion 2 requires a port table for the eight CSL unit test files of [../phase-1-symbolic.md](../phase-1-symbolic.md) section 4.
Which cases exist, what do they check, and which of them can be ported with the API the plan currently lists?
Also: which test-data, CMake and binding hooks exist, and where do the plan and the code disagree?

## 2. Facts: existing tests and data

- F1. Eight files exist in `tests/units/csl/` and are built with `ADD_UNIT_TEST` (label `units`): `tests/units/csl/CMakeLists.txt:1-8`.
- F2. 133 `TEST` cases in those files: equality 5, literals 2, numbers 19, latex 63, relabel_indices 25, signed_refresh 5, metric_delta_invariant 7, init_sanitizer 7 (counted with `grep -c '^TEST('`).
- F3. No golden data exists for symbolic work. `tests/api/data/` does not exist (`ls tests/api` shows `CMakeLists.txt` and `src/` only). `tests/units/csl/` has no data directory; expected values are inline string literals (for example `test_latex.cpp:18-21`, `test_latex.cpp:17` onwards). Shared test expressions are in `tests/units/csl/csltestdata.h:7-31` (`data::a`, `data::x`, `data::i1`, `data::T22`, ...).
- F4. Expansion, derivative and substitution are not covered by the eight files except `csl_numbers.derivative` (`test_numbers.cpp:523`). Tests using `Expand`, `Replace`, `Derive`, `DeepHardFactored` live in `tests/integration/src/` and `tests/system/src/` only. So the phase-1 names `ExpandBinomial`, `DiffChainRule`, `SubsSimultaneous`, `FactorRoundTrip` have no existing C++ unit counterpart; the spec must write them.
- F5. Mutating core calls inside tests, relevant to the "no observable mutation" rule: `a->setValue(4)` (`test_numbers.cpp:31`), `Replace(g, nu, nu.getFlipped())` (`test_metric_delta_invariant.cpp:102`), `Expand(product)` (`test_metric_delta_invariant.cpp:139`).

## 3. Facts: equality, ordering and hashing

- E1. Constants are compared by symbol identity, not by name. `EXPECT_NE(a, csl::constant_s("a"))` (`test_equality.cpp:28`) and `EXPECT_NE(x, csl::variable_s("x"))` (`test_equality.cpp:29`), where `data::a` is `constant_s("a")` (`csltestdata.h:9`). The phase-1 test name `SameNameSameSymbol` (`phase-1-symbolic.md` section 4) contradicts this CSL behaviour and must be rewritten.
- E2. Equality is canonical after construction, without an explicit simplify: `2 + a + 1 == sum_s({a, 3})` and `a + b + 2x == 3x + b - x + a` (`test_equality.cpp:33-37`). Source of `operator==`: `src/csl/abstract.h:1212` and `src/csl/abstract.h:1580` (declarations, earlier read by the other investigator; body not read here).
- E3. Ordering `operator<` (`src/csl/abstract.cpp:1345-1362`) and the simplification rule `ruleO1` (`src/csl/simplification.cpp:192-200`) depend on the global flag `csl::option::fullComparison` (`src/csl/options.h:36`, default `true`). Python-visible sort order therefore depends on global state. The flag is in `MTY_CSL_OPTION_FLAGS` (`src/csl/options.h:55`), so `clean_state` covers it.
- E4. No hash function exists for CSL expressions: a case-insensitive grep for `hash` in `src/csl/**/*.h` and `*.cpp` returns nothing. `operator<` exists, so `sorted()` is possible, `hash()` is not available from C++.
- E5. Printing has a bracket mode argument: `printLaTeX(0|1|2)` (`test_latex.cpp:57-62`), and a terminal print: `c->print(1, terminal)` gives `c^(*)` (`test_latex.cpp:583-589`).

## 4. Facts: API features used by the tests but absent from the phase-1 table

The phase-1 table (section 2 of the plan) lists `Space`, `Index`, `Tensor`, `delta`, `metric`, `epsilon`, `relabel_indices`. The following are used by the CSL tests and are not in that table:

- A1. Matrix printing flag `Space::setMatrixPrinting` / `getMatrixPrinting` and matrix-shaped tensors `Tensor{"M", {&V, &V}}` (`test_latex.cpp:208-311`, 18 cases).
- A2. Non-commuting symbols `setCommutable(false)` on constants and tensors (`test_latex.cpp:109`, `:117`, `:128`, `:281-296`, and others).
- A3. Names with a LaTeX part, `"alpha;\\alpha"`, for spaces, tensors and variables (`test_latex.cpp:449-512`), and the helper `prefixedNameWithLatex` / `getRegularAndLatexNames` (`test_latex.cpp:475-482`).
- A4. Signed spaces and the Minkowski space: `Minkowski`, `getFlipped()`, `flipSign()` (`test_latex.cpp:558-572`, `test_signed_refresh.cpp:30-79`, `test_metric_delta_invariant.cpp:58-142`).
- A5. Index inspection: `getID()`, `getType()` (Free or Dummy), `setValue()` on an index (`test_relabel_indices.cpp:148-167`, `:169-181`, `:340-362`).
- A6. Relabelling options and maps: `RelabelIndices(expr, keepFree)`, `IndexRenaming`, `ApplyIndexRenaming` (`test_relabel_indices.cpp:364-438`, `test_signed_refresh.cpp:30-79`). The plan only lists `relabel_indices(e)`.
- A7. Derivative nodes `TDerivative` (`test_relabel_indices.cpp:214-226`).
- A8. Modulus and argument of complex numbers, `GetComplexModulus`, `GetComplexArgument` (`test_numbers.cpp:42-56`, `:79-89`, `:138-148`, `:149-233`, `test_literals.cpp:5-17`). The plan lists `real`, `imag`, `conjugate` but not `abs`, `arg`, `mod`.
- A9. Complex-valued variables and constants, `ComplexProperty::Complex` (`test_literals.cpp:5-35`, `test_latex.cpp:583`). The plan has `constant("m", real=True)` but no complex property.
- A10. Division by zero gives `CSL_INF` instead of an error (`test_numbers.cpp:~108-109`, `intfraction_s(5, 0) == CSL_INF`). The spec must decide the Python behaviour.
- A11. Abbreviations `Abbrev::makeAbbreviation` and `Evaluated(e, eval::abbreviation)` (`test_latex.cpp:521-556`). The plan puts `Abbrev` management out of scope.
- A12. Matrix-printing tests that only check "no exception" (`EXPECT_NO_THROW`), so they assert no output: `test_latex.cpp:273-279`, `:292-299`, `:347-352`, `:354-361`. A port must decide what to assert.

## 5. Facts: wiring and layout

- W1. `src/csl/api/` contains only `error.h` (`src/csl/api/error.h:32-65`, namespace `csl::api`, header only, no CMake file).
- W2. The session guard is not CSL-only. `src/marty/api/common/session.h` includes `../../core/model.h` and `../../core/mrtError.h` (`session.h:19-25`). The Phase 0 spec placed a CSL-only part in `src/csl/api/`; it was not delivered. A CSL-only API using `Session` would break the layering rule ("CSL does not depend on MARTY", `AGENTS.md`).
- W3. `Session` snapshots `MTY_MARTY_OPTION_FLAGS` and `MTY_CSL_OPTION_FLAGS` (`session.h:37-45` for the save loop). The gamma 4D flag is `sgl::option::keepEvanescentOperators` (`src/marty/core/mrtOptions.h:122`), so it is covered by the same list.
- W4. The Python `clean_state` fixture compares the same two lists plus `Model::current` (`python/src/module.cpp:114-125` for `stateSnapshot`, `python/tests/state_guard.py:12-21`). It does not compare `printErrors` flags set by `Session`.
- W5. Error mapping: `mty::api::Error`, `ValueError`, `TypeError`, `LookupError`, `NotImplementedError`, `StateError` are aliases of the `csl::api` classes (`src/marty/api/common/error.h:38-43`). So a `csl::api::ValueError` thrown by a symbolic function is translated by `translateException` (`python/src/module.cpp:71-111`) without change.
- W6. The C++ API test macro only accepts a flat file: `ADD_API_TEST(NAME)` builds `src/${NAME}.cpp` (`tests/api/CMakeLists.txt:1-6`). The plan's `tests/api/symbolic/` directory (`testing-strategy.md` section 1, `phase-1-symbolic.md` section 4) is not supported by the macro. The parity checker does scan subdirectories (`python/tools/check_parity.py:42`, `rglob`).
- W7. There is one binding source: `python/CMakeLists.txt:20` is `pybind11_add_module(_pymarty src/module.cpp)`. `module.cpp` is 266 lines, with error classes, `_debug` hooks and `Config` only (`PYBIND11_MODULE` at `module.cpp:192`). The README layout (`docs/architecture/pymarty/README.md` section 5) says `python/bindings/<module>.cpp`. The two disagree.
- W8. The package copy rule copies only top-level files: `python/CMakeLists.txt:29-36` globs `pymarty/*.py`, `pymarty/*.pyi` and `py.typed` and `configure_file`s them. A subpackage `python/pymarty/symbolic/` would not be copied into the build tree, and the tests would not import it. A flat `python/pymarty/symbolic.py` plus `symbolic.pyi` is picked up.
- W9. The usage-example test already lists `symbolic`: `python/tests/test_usage_examples.py:21`. `test_every_module_has_a_usage_example` (`:44-52`) fails as soon as `pymarty/symbolic.py` exists without `python/examples/usage/symbolic.py`. So the example must land before, or with, the module file (gate G1 ordering).
- W10. Markers are registered and `--strict-markers` is on: `python/pyproject.toml:46-60` (`usage_example` at line 53). No marker change is needed.
- W11. Parity state, read-only run: `python3 python/tools/check_parity.py` gives `parity: OK, 30 C++ API tests: 9 with a Python counterpart, 21 exempted`. Counts checked by hand: 4 config, 8 error, 7 session, 11 terminating-calls tests; 9 markers; exemptions 3 single ErrorApi cases plus wildcards `SessionApi.*` and `TerminatingCallsApi.*`.
- W12. The checker ignores the CSL unit tests. It only matches `\w+Api` names (`check_parity.py:27`, `:42`), and `python/tests/parity_sources.txt` is empty (`parity_sources.txt:3`). So the port table of the eight CSL files is enforced by review only, not by `check_parity.py`.
- W13. CI: `.github/workflows/python.yml` runs `ctest -L api` and `ctest -L python` (lines 54-56, 95). The `manual-system` job is `workflow_dispatch` only (lines 120-122).

## 6. Port table: `tests/units/csl/`

Summary of candidates: P 54, P+ 55, L1 13, X 11 (total 133).

### 6.1 test_equality.cpp (5)

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 1 | `csl_comparison.numbers` (5) | Integer, rational and float equality across types; `3 != 2.5` | P |
| 2 | `csl_comparison.variables` (21) | Same symbol equal; `constant_s("a")` not equal to `data::a` (identity, see E1) | P (spec must document identity semantics) |
| 3 | `csl_comparison.sum_and_prod` (32) | Canonical sums and products compare equal (E2) | P |
| 4 | `csl_comparison.indices` (44) | Index equality by identity; unary `+i` | P |
| 5 | `csl_comparison.simple_tensors` (56) | Tensor element equality: name, index order, space | P |

### 6.2 test_literals.cpp (2)

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 6 | `csl_literals.constant` (5) | Complex constant: real, imag, modulus, argument, conjugate | P+ (modulus and argument, A8; complex constant, A9) |
| 7 | `csl_literals.variables` (19) | Complex variable: same accessors; prints `==` results to stdout | P+ (A8, A9) |

### 6.3 test_numbers.cpp (19)

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 8 | `csl_numbers.abstract` (5) | Numbers have no sub-symbols and do not depend on symbols | P (`depends_on`, `free_symbols`) |
| 9 | `csl_numbers.integers` (18) | Integer value, type, primary type; `setValue` mutates (line 31) | P (`setValue` is not exposed) |
| 10 | `csl_numbers.integer_constants` (34) | `CSL_0`, `CSL_1`, `CSL_2`, `CSL_M_1`, `CSL_M_2` | P (as `Expr(n)`) |
| 11 | `csl_numbers.integers_complex` (42) | Real, imag, modulus, argument, conjugate of 2 | P+ (A8) |
| 12 | `csl_numbers.floats` (58) | Float value, type, `setValue` | P |
| 13 | `csl_numbers.float_constants` (74) | `CSL_PI`, `CSL_E` | P (`symbolic.pi`, `symbolic.e`) |
| 14 | `csl_numbers.floats_complex` (79) | Complex accessors of `CSL_PI` | P+ (A8) |
| 15 | `csl_numbers.rationals` (90) | Sign and reduction normalisation; `6/3` is an integer; `5/0 == CSL_INF` (line 118-119) | P (division by zero: A10) |
| 16 | `csl_numbers.rational_constants` (127) | `CSL_HALF`, `CSL_THIRD` and negatives | P (`Fraction`) |
| 17 | `csl_numbers.rationals_complex` (138) | Complex accessors of `CSL_HALF` | P+ (A8) |
| 18 | `csl_numbers.complex` (149) | Complex construction, parts, argument, conjugate; `evaluateScalar` of a non-real complex throws `CSLError` (line 201) | P+ (A8) |
| 19 | `csl_numbers.evaluation` (235) | `Evaluated(..., eval::all)` on int, float, rational | P (`evaluate`) |
| 20 | `csl_numbers.expr_conv` (248) | `Expr` coercion from `0`, `1`, `2`, `0.5`, `1LL` | P (coercion order) |
| 21 | `csl_numbers.print` (259) | Prints number expressions; no assertion | L1 (print smoke test, no expected output) |
| 22 | `csl_numbers.operations1` (277) | Complex and rational arithmetic; `**` with int, float, rational exponents | P |
| 23 | `csl_numbers.operations2` (348) | `+ * /` on int, float, rational pairs | P |
| 24 | `csl_numbers.operations3` (450) | Complex combined with int, float, rational under `+ * /` | P |
| 25 | `csl_numbers.errors` (501) | Arithmetic between a number and a symbol throws `CSLError` | P (error class to choose: `MartyTypeError` or `MartyValueError`) |
| 26 | `csl_numbers.derivative` (523) | Derivative of numbers with respect to a symbol is 0 | P (`diff`) |

### 6.4 test_latex.cpp (63)

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 1 | `LatexNumbers.integers` (17) | `3`, `-3`, bracket mode 2 | P |
| 2 | `LatexLiterals.variables` (24) | Plain and `M_W;M_{W}` names | P |
| 3 | `LatexLiterals.imaginaryUnit` (30) | `i`, `2i`, `a + i\,b` | P (`I`) |
| 4 | `LatexSums.signs` (37) | Signs in sums | P |
| 5 | `LatexSums.negativeTermsOfAnyShape` (46) | Negative products, fractions, imaginary terms | P |
| 6 | `LatexSums.bracketsDependOnMode` (57) | Brackets with `printLaTeX(1)` vs `(2)` (E5) | P (mode 2 is not public: spec decides) |
| 7 | `LatexProducts.juxtapositionAndCoefficients` (65) | `2ab`, `-2a`, `-a` | P |
| 8 | `LatexProducts.dotBeforeBracketedSum` (74) | `a\cdot\left(b + c\right)` | P |
| 9 | `LatexFractions.negativePowersAndDivision` (82) | `\frac` for negative powers and division | P |
| 10 | `LatexFractions.rationalCoefficientGoesToDenominator` (94) | `a/2`, `a/(2b)` | P |
| 11 | `LatexNonCommuting.inversePowerOfCommutingFactorGoesToDenominator` (102) | Commuting factor moves to denominator | P+ (commutability, A2) |
| 12 | `LatexNonCommuting.inversePowerOfLoneNonCommutingFactorGoesToDenominator` (109) | Lone non-commuting inverse power | P+ (A2) |
| 13 | `LatexNonCommuting.inversePowerStaysInNumeratorWithNonCommutingFactor` (117) | `A^{-1}B` ordering | P+ (A2) |
| 14 | `LatexNonCommuting.mixOfCommutingDenominatorAndNonCommutingNumerator` (128) | `\frac{A^{-1}Bx}{y}` | P+ (A2) |
| 15 | `LatexPowers.bracesAndBases` (140) | Powers, nested powers, `\sqrt` | P |
| 16 | `LatexFunctions.commonFunctions` (153) | `abs`, `cos`, `exp` | P (`abs` is not in the plan list, A8) |
| 17 | `LatexOutput.noTrailingNewlineAtTopLevel` (162) | No trailing newline | P |
| 18 | `LatexMatrixChains.defaultSpaceUsesIndexNotation` (208) | Index notation when matrix printing is off by default | P+ (A1) |
| 19 | `LatexMatrixChains.extraIndicesArePrintedAsSuperscripts` (219) | `y^{T}\gamma^{\mu}x` | P+ (A1) |
| 20 | `LatexMatrixChains.scalarProductOfVectors` (229) | `x^{T}y` | P+ (A1) |
| 21 | `LatexMatrixChains.singleMatrix` (235) | `y^{T}M_{1}x` | P+ (A1) |
| 22 | `LatexMatrixChains.severalMatricesInMatrixOrder` (241) | Three matrices in order | P+ (A1) |
| 23 | `LatexMatrixChains.reorderedFactorsArePrintedInMatrixOrder` (249) | Factors stored out of order are printed in matrix order | P+ (A1) |
| 24 | `LatexMatrixChains.coefficientAndScalarFactorsStayOutside` (257) | Coefficients stay outside the chain | P+ (A1) |
| 25 | `LatexMatrixChains.orientationFollowsTheStoredIndices` (266) | `x^{T}M_{1}y` from `M_{ji}` | P+ (A1) |
| 26 | `LatexMatrixChains.nonCommutingFactorInOrderIsPrinted` (273) | `EXPECT_NO_THROW` only (weak, A12) | P+ (A1, A2; spec must add an output check) |
| 27 | `LatexMatrixChains.reorderedNonCommutingFactorsArePrintedTransposed` (281) | `x^{T}{M_{2}}^{T}{M_{3}}^{T}y` | P+ (A1, A2) |
| 28 | `LatexMatrixChains.nonCommutingFactorsInOrderArePrinted` (292) | `EXPECT_NO_THROW` only (weak, A12) | P+ (A1, A2) |
| 29 | `LatexMatrixChains.disabledSpaceFallsBackToIndexNotation` (301) | Index notation when the space flag is off | P+ (A1) |
| 30 | `LatexMatrixChains.openChainFallsBackToIndexNotation` (311) | Open chain: index notation | P+ (A1) |
| 31 | `LatexTraces.singleMatrix` (319) | `\mathrm{Tr}\left(M_{1}\right)` | P+ (A1) |
| 32 | `LatexTraces.productOfTwoInMatrixOrder` (325) | `Tr(M_1 M_2)` | P+ (A1) |
| 33 | `LatexTraces.productOfThree` (332) | Trace of three matrices, reordered | P+ (A1) |
| 34 | `LatexTraces.transposedFactor` (339) | `Tr({M_1}^T M_2)` | P+ (A1) |
| 35 | `LatexTraces.singleNonCommutingFactorIsPrinted` (347) | `EXPECT_NO_THROW` only | L1 (weak; port needs an output assertion first) |
| 36 | `LatexTraces.nonCommutingTraceIsPrinted` (354) | `EXPECT_NO_THROW` only | L1 (weak; same reason) |
| 37 | `LatexTraces.disabledSpaceFallsBackToIndexNotation` (363) | `{M_{1}}_{ii}` | P+ (A1) |
| 38 | `LatexComposite.chainMinusFraction` (372) | `-\frac{a}{b} + y^{T}M_{1}x` | P+ (A1) |
| 39 | `LatexComposite.sumRaisedToPowerWithFraction` (381) | `(a + 1/b)^2` | P |
| 40 | `LatexComposite.coefficientFunctionsAndDenominator` (389) | `\frac{2|a|\cos(b)}{c}` | P |
| 41 | `LatexComposite.fractionInExponent` (398) | `a^{b/c}` | P |
| 42 | `LatexComposite.coefficientTimesChainOverRational` (406) | `\frac{ay^{T}M_{1}x}{2}` | P+ (A1) |
| 43 | `LatexComposite.negativeChainTerm` (414) | `a - y^{T}M_{1}x` | P+ (A1) |
| 44 | `LatexComposite.imaginaryTimesChain` (422) | `i\,y^{T}M_{1}x` | P+ (A1) |
| 45 | `LatexComposite.traceAndFraction` (429) | Sum of a fraction and a trace | P+ (A1) |
| 46 | `LatexComposite.reorderedNonCommutingChainInsideSumIsTransposed` (439) | Transposed chain inside a sum | P+ (A1, A2) |
| 47 | `LatexPrintLatex.dummyPairsAreRelabelledInOrder` (449) | Dummy labels in order, `\alpha_{1}` suffix | P (relabel, with names of A3) |
| 48 | `LatexPrintLatex.freeIndexNamesAreSkipped` (463) | Free indices keep their own label | P+ (A3) |
| 49 | `LatexNames.prefixedNameCarriesLatexForm` (475) | `prefixedNameWithLatex` helper | L1 (internal name helper, A3) |
| 50 | `LatexPrintLatex.expressionWithoutDummiesIsUnchanged` (485) | `a + b` unchanged | P |
| 51 | `LatexPrintLatex.relabelDoesNotModifyTheInput` (492) | Input unchanged after relabel | P |
| 52 | `LatexPrintLatex.freeIndicesAreRenamedToo` (504) | Free index renamed by relabel | P+ (A3, A6) |
| 53 | `LatexNames.kroneckerDeltaHasLatexForm` (512) | `\delta_{\alpha\beta}` | P+ (A3) |
| 54 | `EvaluatedAbbreviations.abbreviationIsReplacedByItsDefinition` (521) | Abbreviation replaced | X (Abbrev management out of scope, A11) |
| 55 | `EvaluatedAbbreviations.nestedAbbreviationsAreExpandedFully` (530) | Nested abbreviations | X (A11) |
| 56 | `EvaluatedAbbreviations.expressionWithoutAbbreviationsIsUnchanged` (540) | No abbreviation: unchanged | X (A11) |
| 57 | `EvaluatedAbbreviations.inputIsNotModified` (547) | Abbreviation input unchanged | X (A11) |
| 58 | `LatexSignedIndices.metricWithEqualSignsIsG` (558) | `g^{\mu\nu}` on Minkowski | P+ (A4) |
| 59 | `LatexSignedIndices.freeDeltaIndicesAreRelabelledUpper` (566) | `\delta^{\mu\nu}` after `flipSign` | P+ (A4) |
| 60 | `LatexSignedIndices.unsignedSpaceKeepsSubscripts` (575) | Unsigned space keeps subscripts | P+ (A4) |
| 61 | `LatexConjugate.complexVariableUsesStarInLatexAndTerminal` (583) | `c^{*}` LaTeX; terminal form `c^(*)` is mode 1 (E5) | P (LaTeX part; terminal form is L1) |
| 62 | `LatexComposite.productOfSums` (592) | `\left(a + b\right)\cdot\left(c + d\right)` | P |
| 63 | `LatexComposite.productOfIndexedExpressions` (602) | Two spaces, one matrix-printing on, one off | P+ (A1) |

### 6.5 test_relabel_indices.cpp (25)

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 1 | `RelabelIndices.expressionWithoutIndicesIsUnchanged` (20) | No indices: unchanged | P |
| 2 | `RelabelIndices.singleFreeIndexTakesFirstName` (30) | Free index takes the first name, id 0 | P+ (A5) |
| 3 | `RelabelIndices.distinctFreeIndicesTakeDistinctNames` (42) | Two free indices, two names | P |
| 4 | `RelabelIndices.sameFreeIndexInSumKeepsOneLabel` (56) | `x_{alpha} + y_{alpha}` | P |
| 5 | `RelabelIndices.dummyPairSharesOneLabel` (67) | Dummy pair shares one label | P |
| 6 | `RelabelIndices.distinctPairsTakeNamesInOrder` (76) | Pairs take names in order | P |
| 7 | `RelabelIndices.poolWrapAroundAddsSuffixesFromOne` (87) | Name pool wrap-around: `alpha_1`, `alpha_2` | P |
| 8 | `RelabelIndices.sameNameDifferentIdsAreDifferentIdentities` (108) | Same name, different ids: different identities | P |
| 9 | `RelabelIndices.freeIndicesTakeNamesBeforeDummies` (120) | Free names are assigned before dummy names | P |
| 10 | `RelabelIndices.spacesKeepTheirOwnSequences` (133) | Two spaces, independent name sequences | P |
| 11 | `RelabelIndices.freeIndexOfOneSpaceAndDummyOfAnother` (148) | Free indices with a multi-space tensor (`cslIndex::Free`) | P+ (A5, A7 style type check) |
| 12 | `RelabelIndices.resetsAllIdsToZero` (169) | Output ids are 0 (A5) | P+ (A5) |
| 13 | `RelabelIndices.inputIsNotModified` (183) | Input string and id unchanged | P |
| 14 | `RelabelIndices.relabellingTwiceGivesTheSameLabels` (199) | Idempotent | P |
| 15 | `RelabelIndices.derivativeIndexIsRelabelledWithItsPair` (214) | Derivative node follows its pair (A7) | P+ (A7) |
| 16 | `RelabelIndices.sharedSubexpressionIsNotModified` (255) | Shared sub-expression unchanged; uses internal `ForEachNode` and index keys | L1 (internal traversal; Python-visible part is covered by row 13) |
| 17 | `RelabelIndices.otherExpressionsSharingTheInputStayValid` (280) | Other holders of the input still print the same | P (Python-visible part) |
| 18 | `RelabelIndices.userIndicesKeepNameIdAndType` (302) | User index identity kept (A5) | P+ (A5) |
| 19 | `RelabelIndices.repeatedSubexpressionInTheSameExpressionStaysValid` (323) | `sub * sub` keeps `sub` valid | P |
| 20 | `RelabelIndices.fixedIndexIsNotRenamed` (340) | Index with value 1 prints `x_{1}` (A5) | P+ (A5) |
| 21 | `RelabelIndices.freeIndicesKeepTheirNamesWhenNotRelabelled` (364) | `RelabelIndices(expr, false)` (A6) | P+ (A6) |
| 22 | `RelabelIndices.dummyPairsAvoidKeptFreeNames` (378) | With kept free names, dummies avoid them (A6) | P+ (A6) |
| 23 | `RelabelIndices.renamingAppliedToAnotherExpressionGivesSameLabels` (391) | Shared `IndexRenaming` map (A6) | L1 (renaming map is not in the plan) |
| 24 | `RelabelIndices.applyingRenamingWithoutMatchingIndicesIsUnchanged` (407) | `ApplyIndexRenaming` with no match (A6) | L1 (A6) |
| 25 | `RelabelIndices.applyRenamingDoesNotModifyTheInput` (424) | `ApplyIndexRenaming` does not modify input | L1 (A6; Python-visible part is covered by row 13) |

### 6.6 test_signed_refresh.cpp (5)

All five need the signed Minkowski space and index flipping (A4) and `DeepRefreshed`, so all are `P+`.

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 1 | `SignedRefresh.metricWithFlippedIndexBecomesDelta` (30) | Flipped metric refreshes to delta | P+ (A4) |
| 2 | `SignedRefresh.deltaWithEqualSignsBecomesMetric` (42) | Delta with equal signs becomes metric | P+ (A4) |
| 3 | `SignedRefresh.renamingFlipsMetricIntoDeltaInOutput` (55) | Renaming flips metric into delta in printed output | P+ (A4, A6) |
| 4 | `SignedRefresh.renamingFlipsDeltaIntoMetricInOutput` (68) | Renaming flips delta into metric | P+ (A4, A6) |
| 5 | `SignedRefresh.relabelledFlippedMetricPrintsUpperAfterRefresh` (79) | `g^{\mu\nu}` after relabel and refresh | P+ (A4, A6) |

### 6.7 test_metric_delta_invariant.cpp (7)

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 1 | `MetricDeltaInvariant.constructedElementsMatchTheirSigns` (58) | Constructed metric and delta match their index signs (internal check via `getParent_info`) | P+ (A4; the check itself is L1) |
| 2 | `MetricDeltaInvariant.directIndexWriteReselectsMetricAsDelta` (72) | Direct write to index structure view (C++ pointer access) | L1 (internal mutation) |
| 3 | `MetricDeltaInvariant.directIndexWriteReselectsDeltaAsMetric` (84) | Same, the other direction | L1 (internal mutation) |
| 4 | `MetricDeltaInvariant.replaceFlipsMetricIntoDelta` (96) | `Replace` flips metric into delta (in place, A5) | L1 (in-place core call; Python sees the result of the facade copy) |
| 5 | `MetricDeltaInvariant.setIndexStructureReselectsParent` (107) | `setIndexStructure` (internal) | L1 (internal mutation) |
| 6 | `MetricDeltaInvariant.renamingKeepsProductsConsistent` (117) | `ApplyIndexRenaming` and `RelabelIndices` keep products consistent | P+ (A4, A6) |
| 7 | `MetricDeltaInvariant.refreshingAndFactoringKeepsConsistency` (130) | Expand, factor, refresh keep consistency (in place, line 139) | L1 (internal invariant; Python sees row 6 style results) |

### 6.8 test_init_sanitizer.cpp (7)

All seven test `csl::InitSanitizer<T>`, an internal C++ utility (phase-1 plan section 4 exempts it). All `X`.

| # | Test (line) | Checks | Candidate |
|---|-------------|--------|-----------|
| 1 | `initSanitizer.create` (13) | Creation with name and value | X (internal C++ utility) |
| 2 | `initSanitizer.value_management` (25) | Read before assignment throws; reset | X (internal) |
| 3 | `initSanitizer.equality` (36) | Equality operators | X (internal) |
| 4 | `initSanitizer.comparison` (58) | Ordering operators | X (internal) |
| 5 | `initSanitizer.assign` (94) | Assignment does not copy the name | X (internal) |
| 6 | `initSanitizer.operations` (133) | Arithmetic | X (internal) |
| 7 | `initSanitizer.math_function` (146) | `std::pow` | X (internal) |

### 6.9 Totals

| File | Cases | P | P+ | L1 | X |
|------|------:|--:|---:|---:|--:|
| test_equality | 5 | 5 | 0 | 0 | 0 |
| test_literals | 2 | 0 | 2 | 0 | 0 |
| test_numbers | 19 | 14 | 4 | 1 | 0 |
| test_latex | 63 | 21 | 35 | 3 | 4 |
| test_relabel_indices | 25 | 14 | 7 | 4 | 0 |
| test_signed_refresh | 5 | 0 | 5 | 0 | 0 |
| test_metric_delta_invariant | 7 | 0 | 2 | 5 | 0 |
| test_init_sanitizer | 7 | 0 | 0 | 0 | 7 |
| Total | 133 | 54 | 55 | 13 | 11 |

Reading of the totals: with the current phase-1 table, 54 cases port directly. Another 55 need features from section 4 (A1 to A12), which the spec must either add to the phase-1 table or accept as `L1` with a reason. The acceptance target "at least 90% of the cases with an API counterpart are ported" (criterion 2) is reached only if most of the `P+` rows are added to the table.

## 7. Decisions for the spec (orchestrator Q&A)

1. Equality semantics. CSL compares constants by symbol identity (E1). Keep that and rewrite `SameNameSameSymbol`, or change the core? Recommendation: keep, document it, and rename the test to `SameNameDifferentSymbol`.
2. Hash. CSL has no hash (E4), and `operator<` depends on `fullComparison` (E3). Options: `__hash__ = None` plus `structural_key()`, as the plan's fallback says. Recommendation: `__hash__ = None`, and `sorted()` only if the spec decides ordering is part of the API.
3. Features of section 4 (A1 to A12). For each: add to the phase-1 table, move to a later phase, or classify its tests as `L1`. The matrix-printing and non-commuting features (A1, A2) cover 40 cases.
4. CSL-only session guard. W2: a CSL API cannot use `mty::api::Session`. Options: a CSL-only guard in `src/csl/api/` that snapshots `csl::option::*` only, or no guard for the CSL API (the flags it reads are in `MTY_CSL_OPTION_FLAGS`).
5. Test layout. Flat `tests/api/src/test_symbolic_api.cpp` (works with `ADD_API_TEST` as is), or extend the macro to `tests/api/symbolic/` as the plan says (W6).
6. Binding layout. Keep `python/src/module.cpp` and add files to `python/CMakeLists.txt:20`, or create `python/bindings/` as the README says (W7).
7. Python module layout. Flat `python/pymarty/symbolic.py` (picked up by the CMake glob, W8), or a subpackage (needs a CMake change).
8. Division by zero. `intfraction_s(5, 0)` gives `CSL_INF` (A10). Python should raise `MartyValueError`, return `float('inf')`, or expose `CSL_INF`. The spec decides.
9. Golden data. No golden data exists (F3). The 63 LaTeX cases of `test_latex.cpp` already contain expected strings, which can seed the 50-expression set of acceptance criterion 3. Values must be copied from the C++ test, never invented.

## 8. Not verified

- Nothing was built or run. The C++ tests were not executed, so the expected values in the tables are what the tests assert, not what the code outputs today.
- The identity semantics of constants (E1) are inferred from the `EXPECT_NE` lines. The body of `constant_s` and of `operator==` was not read.
- `check_parity.py` was run read-only with the system `python3`. The pytest suite was not run.
- `remotes/origin/pymarty` (`src/pymarty/simple_py.cpp`) and `api/gamma` (`python/src/pymarty/gamma_api/`) were checked only by grep. They expose gamma matrices, not `csl::Expr`. Their binding code was not read in full.
- The Phase 0 spec (`phase-0-foundations.md`) was not compared line by line with the commit `eaa9ce6`. Only the file list of the commit was read.
- `tests/integration/src/` and `tests/system/src/` were not checked for expansion or substitution expectations beyond the file names.
