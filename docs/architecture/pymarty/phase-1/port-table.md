# WP5b port table: C++ unit tests `tests/units/csl/` to pymarty

Source: the 133 candidate rows of section 6 in
[01-investigation-tests-and-wiring.md](01-investigation-tests-and-wiring.md), re-checked against the
code. Scope rules are from [03-spec.md](03-spec.md) section 10.2.

## Columns and vocabulary

- **Class**: P (port), P+ (port with extra API), L1 (covered by the C++ L1 tests only), X (internal or out of scope).
  Corrections to the investigation are listed at the end.
- **Status**:
  - `ported`: a named Python test checks the same behaviour. The destination column names it.
  - `gap`: the feature is in scope (phase A5 to A8, A10, or the named exposed features), but no Python test checks the row, or only part of it. The missing assertions are noted.
  - `deferred to phase 2 (Ax)`: the spec defers this area (A1 to A4) or a later phase (A9).
  - `covered by L1 only`: the row tests internal mechanics with no Python-visible behaviour.
  - `n/a (reason)`: the feature is not exposed or is out of scope (A11, internal utility).
- **Destination**: `file.py::function` for ported rows. Partial coverage and reasons otherwise.

Rule: a row is `ported` only when a Python test checks the same behaviour. Gaps are reported, not filled.

## Table

| # | File | C++ test (Suite.Name) | Class | Status | Destination |
|---|------|------------------------|-------|--------|-------------|
| 1 | test_equality.cpp | csl_comparison.numbers | P | ported | `test_equality.py::test_numbers_compare_by_value` |
| 2 | test_equality.cpp | csl_comparison.variables | P | ported | `test_equality.py::test_constant_and_variable_of_same_name_differ` |
| 3 | test_equality.cpp | csl_comparison.sum_and_prod | P | ported | `test_operators.py::test_sum_and_product_equality_is_canonical` |
| 4 | test_equality.cpp | csl_comparison.indices | P | ported | `test_indices.py::test_index_hash_ignores_sign` |
| 5 | test_equality.cpp | csl_comparison.simple_tensors | P | ported | `test_tensors.py::test_tensor_equality_depends_on_order_and_rank` |
| 6 | test_literals.cpp | csl_literals.constant | P+ | gap | `constant` takes a name only, so complex-property symbols are not reachable from Python; partial: `test_operators.py::test_constant_and_variable_accept_any_name` |
| 7 | test_literals.cpp | csl_literals.variables | P+ | gap | `variable` takes a name only, so complex-property symbols are not reachable from Python; partial: `test_operators.py::test_constant_and_variable_accept_any_name` |
| 8 | test_numbers.cpp | csl_numbers.abstract | P | ported | `test_transform.py::test_depends_on_and_free_symbols` |
| 9 | test_numbers.cpp | csl_numbers.integers | P | ported | `test_numbers.py::test_integer_arithmetic_is_exact` (setValue not exposed) |
| 10 | test_numbers.cpp | csl_numbers.integer_constants | P | ported | `test_numbers.py::test_integer_constants` |
| 11 | test_numbers.cpp | csl_numbers.integers_complex | P+ | ported | `test_numbers.py::test_integer_complex_functions` |
| 12 | test_numbers.cpp | csl_numbers.floats | P | ported | `test_numbers.py::test_float_is_contagious` |
| 13 | test_numbers.cpp | csl_numbers.float_constants | P | ported | `test_numbers.py::test_pi_and_e_values` |
| 14 | test_numbers.cpp | csl_numbers.floats_complex | P+ | ported | `test_numbers.py::test_float_complex_functions` |
| 15 | test_numbers.cpp | csl_numbers.rationals | P | ported | `test_numbers.py::test_rationals_normalise_and_sign` |
| 16 | test_numbers.cpp | csl_numbers.rational_constants | P | ported | `test_numbers.py::test_rational_constants` |
| 17 | test_numbers.cpp | csl_numbers.rationals_complex | P+ | ported | `test_numbers.py::test_rational_complex_functions` |
| 18 | test_numbers.cpp | csl_numbers.complex | P+ | ported | `test_numbers.py::test_complex_numbers_full` |
| 19 | test_numbers.cpp | csl_numbers.evaluation | P | ported | `test_transform.py::test_float_of_real_number` |
| 20 | test_numbers.cpp | csl_numbers.expr_conv | P | gap | partial: `test_numbers.py::test_float_is_contagious` (`Expr(nullptr)` and `Expr(1LL)` not exposed) |
| 21 | test_numbers.cpp | csl_numbers.print | L1 | covered by L1 only | smoke test with no expected output; text output is in `test_printing.py::test_str_returns_text_output` |
| 22 | test_numbers.cpp | csl_numbers.operations1 | P | ported | `test_numbers.py::test_power_with_rational_float_complex_exponents` |
| 23 | test_numbers.cpp | csl_numbers.operations2 | P | ported | `test_numbers.py::test_mixed_type_arithmetic` |
| 24 | test_numbers.cpp | csl_numbers.operations3 | P | ported | `test_numbers.py::test_complex_arithmetic_with_mixed_types` |
| 25 | test_numbers.cpp | csl_numbers.errors | P | n/a (internal contract) | the C++ test calls `addition_own` with a symbol; Python operators accept symbols, so the error path is not reachable from Python |
| 26 | test_numbers.cpp | csl_numbers.derivative | P | ported | `test_numbers.py::test_derivative_of_numbers_is_zero` |
| 27 | test_latex.cpp | LatexNumbers.integers | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L19-20) |
| 28 | test_latex.cpp | LatexLiterals.variables | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L26-27) |
| 29 | test_latex.cpp | LatexLiterals.imaginaryUnit | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L32-34); `test_printing.py::test_latex_of_imaginary_unit_and_number` |
| 30 | test_latex.cpp | LatexSums.signs | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L41-43) |
| 31 | test_latex.cpp | LatexSums.negativeTermsOfAnyShape | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L51-53) |
| 32 | test_latex.cpp | LatexSums.bracketsDependOnMode | P | n/a (not exposed) | printing mode argument is not in the facade (`to_latex()` takes none); open decision |
| 33 | test_latex.cpp | LatexProducts.juxtapositionAndCoefficients | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L69-71) |
| 34 | test_latex.cpp | LatexProducts.dotBeforeBracketedSum | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L79) |
| 35 | test_latex.cpp | LatexFractions.negativePowersAndDivision | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L87-91) |
| 36 | test_latex.cpp | LatexFractions.rationalCoefficientGoesToDenominator | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L98-99) |
| 37 | test_latex.cpp | LatexNonCommuting.inversePowerOfCommutingFactorGoesToDenominator | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L106); class corrected from P+, no non-commuting symbol is used |
| 38 | test_latex.cpp | LatexNonCommuting.inversePowerOfLoneNonCommutingFactorGoesToDenominator | P+ | deferred to phase 2 (A2) | non-commuting symbols are deferred (spec 10.2) |
| 39 | test_latex.cpp | LatexNonCommuting.inversePowerStaysInNumeratorWithNonCommutingFactor | P+ | deferred to phase 2 (A2) | non-commuting symbols are deferred (spec 10.2) |
| 40 | test_latex.cpp | LatexNonCommuting.mixOfCommutingDenominatorAndNonCommutingNumerator | P+ | deferred to phase 2 (A2) | non-commuting symbols are deferred (spec 10.2) |
| 41 | test_latex.cpp | LatexPowers.bracesAndBases | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L145-150) |
| 42 | test_latex.cpp | LatexFunctions.commonFunctions | P | ported | `test_printing.py::test_latex_of_abs_and_common_functions` |
| 43 | test_latex.cpp | LatexOutput.noTrailingNewlineAtTopLevel | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (exact string compare) |
| 44 | test_latex.cpp | LatexMatrixChains.defaultSpaceUsesIndexNotation | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 45 | test_latex.cpp | LatexMatrixChains.extraIndicesArePrintedAsSuperscripts | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 46 | test_latex.cpp | LatexMatrixChains.scalarProductOfVectors | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 47 | test_latex.cpp | LatexMatrixChains.singleMatrix | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 48 | test_latex.cpp | LatexMatrixChains.severalMatricesInMatrixOrder | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 49 | test_latex.cpp | LatexMatrixChains.reorderedFactorsArePrintedInMatrixOrder | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 50 | test_latex.cpp | LatexMatrixChains.coefficientAndScalarFactorsStayOutside | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 51 | test_latex.cpp | LatexMatrixChains.orientationFollowsTheStoredIndices | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 52 | test_latex.cpp | LatexMatrixChains.nonCommutingFactorInOrderIsPrinted | P+ | deferred to phase 2 (A1, A2) | matrix printing and non-commuting symbols are deferred (spec 10.2) |
| 53 | test_latex.cpp | LatexMatrixChains.reorderedNonCommutingFactorsArePrintedTransposed | P+ | deferred to phase 2 (A1, A2) | matrix printing and non-commuting symbols are deferred (spec 10.2) |
| 54 | test_latex.cpp | LatexMatrixChains.nonCommutingFactorsInOrderArePrinted | P+ | deferred to phase 2 (A1, A2) | matrix printing and non-commuting symbols are deferred (spec 10.2) |
| 55 | test_latex.cpp | LatexMatrixChains.disabledSpaceFallsBackToIndexNotation | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 56 | test_latex.cpp | LatexMatrixChains.openChainFallsBackToIndexNotation | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 57 | test_latex.cpp | LatexTraces.singleMatrix | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 58 | test_latex.cpp | LatexTraces.productOfTwoInMatrixOrder | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 59 | test_latex.cpp | LatexTraces.productOfThree | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 60 | test_latex.cpp | LatexTraces.transposedFactor | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 61 | test_latex.cpp | LatexTraces.singleNonCommutingFactorIsPrinted | L1 | deferred to phase 2 (A1, A2) | weak L1 test; a port needs an output assertion first; feature deferred (spec 10.2) |
| 62 | test_latex.cpp | LatexTraces.nonCommutingTraceIsPrinted | L1 | deferred to phase 2 (A1, A2) | weak L1 test; a port needs an output assertion first; feature deferred (spec 10.2) |
| 63 | test_latex.cpp | LatexTraces.disabledSpaceFallsBackToIndexNotation | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 64 | test_latex.cpp | LatexComposite.chainMinusFraction | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 65 | test_latex.cpp | LatexComposite.sumRaisedToPowerWithFraction | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L385) |
| 66 | test_latex.cpp | LatexComposite.coefficientFunctionsAndDenominator | P | ported | `test_printing.py::test_latex_of_coefficient_with_abs_and_denominator` |
| 67 | test_latex.cpp | LatexComposite.fractionInExponent | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L403) |
| 68 | test_latex.cpp | LatexComposite.coefficientTimesChainOverRational | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 69 | test_latex.cpp | LatexComposite.negativeChainTerm | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 70 | test_latex.cpp | LatexComposite.imaginaryTimesChain | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 71 | test_latex.cpp | LatexComposite.traceAndFraction | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 72 | test_latex.cpp | LatexComposite.reorderedNonCommutingChainInsideSumIsTransposed | P+ | deferred to phase 2 (A1, A2) | matrix printing and non-commuting symbols are deferred (spec 10.2) |
| 73 | test_latex.cpp | LatexPrintLatex.dummyPairsAreRelabelledInOrder | P | ported | `test_tensors.py::test_dummy_pairs_relabelled_in_order` |
| 74 | test_latex.cpp | LatexPrintLatex.freeIndexNamesAreSkipped | P+ | deferred to phase 2 (A3) | LaTeX index names are deferred (spec 10.2) |
| 75 | test_latex.cpp | LatexNames.prefixedNameCarriesLatexForm | L1 | deferred to phase 2 (A3) | internal name helper; LaTeX names are deferred (spec 10.2) |
| 76 | test_latex.cpp | LatexPrintLatex.expressionWithoutDummiesIsUnchanged | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L41) |
| 77 | test_latex.cpp | LatexPrintLatex.relabelDoesNotModifyTheInput | P | ported | `test_tensors.py::test_relabel_does_not_mutate` |
| 78 | test_latex.cpp | LatexPrintLatex.freeIndicesAreRenamedToo | P+ | deferred to phase 2 (A3) | LaTeX index names are deferred (spec 10.2) |
| 79 | test_latex.cpp | LatexNames.kroneckerDeltaHasLatexForm | P+ | deferred to phase 2 (A3) | LaTeX index names are deferred (spec 10.2) |
| 80 | test_latex.cpp | EvaluatedAbbreviations.abbreviationIsReplacedByItsDefinition | X | n/a (out of scope) | abbreviation management is out of scope (A11) |
| 81 | test_latex.cpp | EvaluatedAbbreviations.nestedAbbreviationsAreExpandedFully | X | n/a (out of scope) | abbreviation management is out of scope (A11) |
| 82 | test_latex.cpp | EvaluatedAbbreviations.expressionWithoutAbbreviationsIsUnchanged | X | n/a (out of scope) | abbreviation management is out of scope (A11) |
| 83 | test_latex.cpp | EvaluatedAbbreviations.inputIsNotModified | X | n/a (out of scope) | abbreviation management is out of scope (A11) |
| 84 | test_latex.cpp | LatexSignedIndices.metricWithEqualSignsIsG | P+ | deferred to phase 2 (A4) | signed indices are deferred except the Minkowski flip (spec 10.2) |
| 85 | test_latex.cpp | LatexSignedIndices.freeDeltaIndicesAreRelabelledUpper | P+ | deferred to phase 2 (A4) | signed indices are deferred except the Minkowski flip (spec 10.2) |
| 86 | test_latex.cpp | LatexSignedIndices.unsignedSpaceKeepsSubscripts | P+ | deferred to phase 2 (A4) | signed indices are deferred except the Minkowski flip (spec 10.2) |
| 87 | test_latex.cpp | LatexConjugate.complexVariableUsesStarInLatexAndTerminal | P | gap | partial: `test_transform.py::test_conjugate_is_an_involution` (LaTeX star not asserted; terminal form is L1) |
| 88 | test_latex.cpp | LatexComposite.productOfSums | P | ported | `test_parity.py::test_golden_expressions_printing_matches_cpp` (golden L598) |
| 89 | test_latex.cpp | LatexComposite.productOfIndexedExpressions | P+ | deferred to phase 2 (A1) | matrix printing is deferred (spec 10.2) |
| 90 | test_relabel_indices.cpp | RelabelIndices.expressionWithoutIndicesIsUnchanged | P | ported | `test_tensors.py::test_relabel_expression_without_indices_is_unchanged` |
| 91 | test_relabel_indices.cpp | RelabelIndices.singleFreeIndexTakesFirstName | P+ | ported | `test_tensors.py::test_relabel_single_free_index_takes_first_name` |
| 92 | test_relabel_indices.cpp | RelabelIndices.distinctFreeIndicesTakeDistinctNames | P | ported | `test_tensors.py::test_relabel_distinct_free_indices_take_distinct_names` |
| 93 | test_relabel_indices.cpp | RelabelIndices.sameFreeIndexInSumKeepsOneLabel | P | ported | `test_tensors.py::test_relabel_repeated_free_index_in_sum_keeps_one_label` |
| 94 | test_relabel_indices.cpp | RelabelIndices.dummyPairSharesOneLabel | P | ported | `test_tensors.py::test_relabel_dummy_pair_shares_one_label` |
| 95 | test_relabel_indices.cpp | RelabelIndices.distinctPairsTakeNamesInOrder | P | ported | `test_tensors.py::test_relabel_distinct_pairs_take_names_in_order` |
| 96 | test_relabel_indices.cpp | RelabelIndices.poolWrapAroundAddsSuffixesFromOne | P | ported | `test_tensors.py::test_relabel_pool_wrap_around_adds_suffixes` |
| 97 | test_relabel_indices.cpp | RelabelIndices.sameNameDifferentIdsAreDifferentIdentities | P | ported | `test_tensors.py::test_relabel_same_name_different_ids_are_different` |
| 98 | test_relabel_indices.cpp | RelabelIndices.freeIndicesTakeNamesBeforeDummies | P | ported | `test_tensors.py::test_relabel_free_indices_take_names_before_dummies` |
| 99 | test_relabel_indices.cpp | RelabelIndices.spacesKeepTheirOwnSequences | P | ported | `test_tensors.py::test_relabel_spaces_keep_their_own_sequences` |
| 100 | test_relabel_indices.cpp | RelabelIndices.freeIndexOfOneSpaceAndDummyOfAnother | P+ | ported | `test_tensors.py::test_relabel_free_of_one_space_dummy_of_another` |
| 101 | test_relabel_indices.cpp | RelabelIndices.resetsAllIdsToZero | P+ | ported | `test_tensors.py::test_relabel_resets_all_ids_to_zero` |
| 102 | test_relabel_indices.cpp | RelabelIndices.inputIsNotModified | P | ported | `test_tensors.py::test_relabel_does_not_mutate` |
| 103 | test_relabel_indices.cpp | RelabelIndices.relabellingTwiceGivesTheSameLabels | P | ported | `test_tensors.py::test_relabel_twice_gives_same_labels` |
| 104 | test_relabel_indices.cpp | RelabelIndices.derivativeIndexIsRelabelledWithItsPair | P+ | gap | A7; derivative indices are in scope; no Python test |
| 105 | test_relabel_indices.cpp | RelabelIndices.sharedSubexpressionIsNotModified | L1 | covered by L1 only | internal traversal; Python-visible part is row 102 |
| 106 | test_relabel_indices.cpp | RelabelIndices.otherExpressionsSharingTheInputStayValid | P | ported | `test_tensors.py::test_relabel_does_not_mutate` (original expression keeps its free names after relabel) |
| 107 | test_relabel_indices.cpp | RelabelIndices.userIndicesKeepNameIdAndType | P+ | ported | `test_tensors.py::test_relabel_leaves_user_index_unchanged` |
| 108 | test_relabel_indices.cpp | RelabelIndices.repeatedSubexpressionInTheSameExpressionStaysValid | P | ported | `test_tensors.py::test_relabel_repeated_subexpression_stays_valid` |
| 109 | test_relabel_indices.cpp | RelabelIndices.fixedIndexIsNotRenamed | P+ | ported | `test_tensors.py::test_relabel_fixed_index_is_not_renamed` |
| 110 | test_relabel_indices.cpp | RelabelIndices.freeIndicesKeepTheirNamesWhenNotRelabelled | P+ | ported | `test_tensors.py::test_relabel_free_keeps_free_names_when_disabled` |
| 111 | test_relabel_indices.cpp | RelabelIndices.dummyPairsAvoidKeptFreeNames | P+ | ported | `test_tensors.py::test_relabel_dummy_pairs_avoid_kept_free_names` |
| 112 | test_relabel_indices.cpp | RelabelIndices.renamingAppliedToAnotherExpressionGivesSameLabels | L1 | covered by L1 only | the renaming map is not in the plan |
| 113 | test_relabel_indices.cpp | RelabelIndices.applyingRenamingWithoutMatchingIndicesIsUnchanged | L1 | covered by L1 only | renaming map not in the plan (A6) |
| 114 | test_relabel_indices.cpp | RelabelIndices.applyRenamingDoesNotModifyTheInput | L1 | covered by L1 only | renaming map not in the plan (A6); Python-visible part is row 102 |
| 115 | test_signed_refresh.cpp | SignedRefresh.metricWithFlippedIndexBecomesDelta | P+ | deferred to phase 2 (A4) | signed refresh is deferred except the Minkowski flip via `Index.flipped()` |
| 116 | test_signed_refresh.cpp | SignedRefresh.deltaWithEqualSignsBecomesMetric | P+ | deferred to phase 2 (A4) | signed refresh is deferred (spec 10.2) |
| 117 | test_signed_refresh.cpp | SignedRefresh.renamingFlipsMetricIntoDeltaInOutput | P+ | deferred to phase 2 (A4) | signed refresh is deferred (spec 10.2) |
| 118 | test_signed_refresh.cpp | SignedRefresh.renamingFlipsDeltaIntoMetricInOutput | P+ | deferred to phase 2 (A4) | signed refresh is deferred (spec 10.2) |
| 119 | test_signed_refresh.cpp | SignedRefresh.relabelledFlippedMetricPrintsUpperAfterRefresh | P+ | deferred to phase 2 (A4, A6) | signed refresh is deferred (spec 10.2) |
| 120 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.constructedElementsMatchTheirSigns | P+ | deferred to phase 2 (A4) | signed refresh is deferred (spec 10.2) |
| 121 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.directIndexWriteReselectsMetricAsDelta | L1 | covered by L1 only | internal mutation |
| 122 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.directIndexWriteReselectsDeltaAsMetric | L1 | covered by L1 only | internal mutation |
| 123 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.replaceFlipsMetricIntoDelta | L1 | covered by L1 only | in-place core call; Python sees the facade copy |
| 124 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.setIndexStructureReselectsParent | L1 | covered by L1 only | internal mutation |
| 125 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.renamingKeepsProductsConsistent | P+ | deferred to phase 2 (A4, A6) | signed refresh is deferred (spec 10.2) |
| 126 | test_metric_delta_invariant.cpp | MetricDeltaInvariant.refreshingAndFactoringKeepsConsistency | L1 | covered by L1 only | internal invariant |
| 127 | test_init_sanitizer.cpp | initSanitizer.create | X | n/a (internal) | C++ utility; no Python-visible behaviour |
| 128 | test_init_sanitizer.cpp | initSanitizer.value_management | X | n/a (internal) | C++ utility; no Python-visible behaviour |
| 129 | test_init_sanitizer.cpp | initSanitizer.equality | X | n/a (internal) | C++ utility; no Python-visible behaviour |
| 130 | test_init_sanitizer.cpp | initSanitizer.comparison | X | n/a (internal) | C++ utility; no Python-visible behaviour |
| 131 | test_init_sanitizer.cpp | initSanitizer.assign | X | n/a (internal) | C++ utility; no Python-visible behaviour |
| 132 | test_init_sanitizer.cpp | initSanitizer.operations | X | n/a (internal) | C++ utility; no Python-visible behaviour (missing from investigation section 6.8) |
| 133 | test_init_sanitizer.cpp | initSanitizer.math_function | X | n/a (internal) | C++ utility; no Python-visible behaviour (missing from investigation section 6.8) |

## Corrections to the investigation

- Row 37 (`LatexNonCommuting.inversePowerOfCommutingFactorGoesToDenominator`): class P+ becomes P. It uses no non-commuting symbol.
- Row 66 (`LatexComposite.coefficientFunctionsAndDenominator`): the description contains `|a|`, which broke the markdown table. Here the table has no description column, so the class P is parsed correctly.
- Rows 132 and 133 (`initSanitizer.operations`, `initSanitizer.math_function`) were missing from investigation section 6.8.
- Investigation rows labelled P+ that depend on A1 to A4 (matrix chains, traces, LaTeX index names, signed indices) are `deferred to phase 2` here, per spec 10.2. The Status column shows which ones.
- Row 25 (`csl_numbers.errors`): n/a, because the C++ test uses an internal path with a symbol.
- Investigation totals: the row labels give P 53 and P+ 56 (row 40 parsed as `a`). With row 37 as P and row 66 as P, the class totals are P 54, P+ 55, L1 13, X 11, which match the investigation's stated totals.
