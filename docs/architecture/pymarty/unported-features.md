# pymarty: unported features

Status: living document. Update it in the same change that ports, or defers, a feature (see the
checklist in [python-porting-rules.md](python-porting-rules.md), section 2, steps 1 and 11).

Status values:

- **ported**: exposed in `pymarty` and covered by tests.
- **unported**: exists in C++ (CSL or MARTY), not exposed in Python.
- **py-algorithm**: callback-based C++ algorithm, planned as a Python duplicate (rules section 6).
- **exempt**: deliberately not exposed (give the reason in the notes).
- **open**: needs a decision from the user before a status can be set.

Phase numbers refer to [phase-N plan documents](README.md).

## 1. Expression (`Expr`) basics

| Feature | CSL reference | Status | Phase / blocker | Notes |
|---------|---------------|--------|-----------------|-------|
| Arithmetic operators, `__call__` | `src/csl/Abstract.h` | ported | 1 | |
| `operand` | `Abstract::getOperand` | ported | 1 | Raises `MartyTypeError` on non-operators (addendum C1). |
| `free_indices`, `dummy_indices` | `src/csl/api/` | ported | 1 | |
| `name`, `complex_property`, `structural_key` | `src/csl/Abstract.h` | ported | 1 | |
| `constant`, `variable` | `src/csl/api/` | ported | 1 | |
| `evaluate` with eval mode | `csl::Evaluated`, `csl::eval::mode` | ported | 1 | `evaluate(mode=EvalMode.*)`, default `All`. Addendum decision F. |
| `replace` on expressions | `csl::Replace(Expr, Expr, Expr)` | ported | 1 | Expression only. Index and parent replacement are unported (see section 3). |
| `Expr.relabel_indices` | `csl::api::relabelIndices` | ported | 1 | Addendum decision A. Renaming dict is input and output. `apply_renaming` removed. |
| `deep_expand`, `simplify` | `src/csl/api/` | unported / open | 1 | Check the current facade for `simplify` before porting. |

## 2. Expression accessors (`Abstract.h`)

All of these are unported unless noted. Error model: rules section 8 (addendum decision C).

### 2.1 Getters, grouped by role

| Group | CSL getters | Status |
|-------|-------------|--------|
| Structure | `getArgument`, `getArguments`, `getNArgs`, `getOperand`, `getFactors`, `getTerm`, `getExponents`, `getNumericalFactor` | ported as `argument`, `args` (and `len(args)` for `getNArgs`), `operand`, `factors`, `term`, `exponents`, `numerical_factor`. `getSum` and `getProduct` are unported: they exist only on `Vectorial` (`src/csl/vector.h:144-146`), not on `Expr`. |
| Index access | `getNIndices`, `getIndex`, `getIndexStructure`, `getFreeIndexStructure` | unported |
| Numerical parts | `getNum`, `getDenom`, `getSign`, `getParity`, `getOrder` | unported. `getNum` returns 0 silently on wrong kind (`src/csl/abstract.cpp:189-192`). |
| Complex parts | `getRealPart`, `getImaginaryPart`, `getComplexArgument`, `getComplexModulus`, `getComplexConjugate`, `getTransposed`, `getPolynomialTerm` | unported |
| Values and types | `getValued`, `getValue`, `getType`, `getPrimaryType`, `getName`, `getLatexName`, `getShape`, `getDim` | unported |
| Symmetry and contraction | `getPermutations`, `getCanonicalPermutation`, `getContractedPair`, `getNContractedPairs`, `getContractionProperties`, `getNFactor` | unported |
| Boundaries and ranges | `getInfBoundary`, `getSupBoundary`, `getRegularExpression` | unported |
| Vector and subvector | `getSubVectorial`, `getVectorialModulus`, `getSubSymbols` | unported |
| Parents and points | `getParent`, `getPoint` | unported. `getParent` returns a null parent for a legitimate absence (`src/csl/abstract.cpp:260-263`). |
| Dependencies and libraries | `getAllDependencies`, `getLibDependency`, `getCommutable`, `getElementary` | unported |

### 2.2 Predicates

`isReal`, `isPurelyImaginary`, `isInteger`, `isIndexed`, `isPolynomial`, `isEmpty`, `isArbitrary`,
`isBuildingBlock`, and the other `is*` and `has*` virtuals in `src/csl/Abstract.h`. Status:
unported. Group them in the Python API by role (Python naming in rules section 4).

### 2.3 Setters (never ported)

`setName`, `setValue`, `setArgument`, `setOperand`, `setVariable`, `setIndexStructure`, `setParent`,
`setPoint`, `setSupBoundary`, `setInfBoundary`, `setComplexProperty`, `setConjugated`,
`setCommutable`, `setElementary`, `setAllDependencies`, `setEmpty`, `setVectorArgument`,
`setFullySymmetric`, `setFullyAntiSymmetric`, and others. Status: **exempt, never ported**. Each setter
mutates a shared CSL node. Python expressions are built as new (sub-)expressions instead (rules section 4,
principle 5). Decision: addendum C2.

### 2.4 Index operations

| Feature | CSL reference | Status | Notes |
|---------|---------------|--------|-------|
| Index replacement in an expression | `Abstract::replaceIndex`, `replaceIndexInPlace` (`src/csl/abstract.h:583-593`) | unported | Addendum decision B. |
| Indicial replacement | `ReplaceIndicial` (`src/csl/replace.h`) | unported | |
| Index renaming output map | `relabelIndices` (`src/csl/api/tensor.h`) | ported | Written into the optional Python dict. Addendum decision A. |

## 3. Replacement

| Feature | CSL reference | Status | Notes |
|---------|---------------|--------|-------|
| Replace expression by expression | `Replace(Expr, Expr, Expr)` (`src/csl/replace.h:30`) | ported | |
| Replace parent by expression | `Replace(Expr, Parent, Expr)` (`src/csl/replace.h:31`) | unported | |
| Replace parent by parent | `Replace(Expr, Parent, Parent)` (`src/csl/replace.h:32`) | unported | |
| Index replacement | see section 2.4 | unported | |

## 4. Parent objects

| Object | CSL reference | Accessor status | Notes |
|--------|---------------|-----------------|-------|
| `AbstractParent` | `src/csl/parent.h:93+` | unported (read-only getters: `getSpace`, `getBaseName`, `getEncapsulated`, `getFieldSpace`, `getTensor`) | |
| `TensorParent` | `src/csl/indicial.h:242+` | unported (getters: `getSpace`, `getSymmetry`, `getFullySymmetric`, `getFullyAntiSymmetric`, `getPermutation`, `getTensor`, `getTrace`, `getDim`, `getType`, `getPrimaryType`, `isValued`, `isTraceLessIn`) | `getTensor` and `getTrace` may be exempt from Python; see addendum. |
| `TensorField` | `src/csl/tensorField.h:34-139` | unported (getters: `getFieldSpace`, `getPoint`, `getArgument`, `getOperand`, `getIndexStructure`, `getSubSymbols`, `getLatexPointSuffix`, `getComplexConjugate`, `getPrimaryType`, `getType`, `isIndexed`) | |
| `Tensor` | `src/csl/api/` | `__call__` only (ported); no accessors | |
| `TensorDerivative` | `d(mu, X)` | `__call__` only (ported) | Rule in `AGENTS.md`: `d(mu, X)(operand)` only. |

## 5. Callback-based algorithms

These take C++ function callbacks, so they cannot be ported by binding. Plan: Python duplicates
(rules section 6), or declarative C++ predicates when the predicate is simple.

| Algorithm | CSL reference | Status | Notes |
|-----------|---------------|--------|-------|
| `ExpandIf` | `src/csl/` (see [phase-1-symbolic.md](phase-1-symbolic.md) section 1) | py-algorithm (decided, not written) | |
| `DistributeIf` | same | py-algorithm (decided, not written) | |
| Predicate-driven searches on accessors | same | py-algorithm (decided, not written) | Needs the accessors of section 2 first. |

## 6. Other CSL and MARTY features

| Feature | Reference | Status | Phase | Notes |
|---------|-----------|--------|-------|-------|
| Pseudo-integrals | `src/csl/pseudoIntegral.h`, `src/csl/pseudoIntegral.cpp` | unported (not yet investigated in depth) | open | Verify exposed API and whether it is needed for phase 1. |
| Multi-dimensional arrays to fill indexed tensors | not yet located | unported (not yet investigated) | open | Locate the CSL/MARTY reference first. |
| Abbreviation management | CSL abbreviations, `eval::abbreviation` | ported | 1 | `make_abbreviation` and `EvalMode.Abbreviation`. Port-table rows 80-83 are ported (`test_abbreviations.py`). |
| `ISum`, `IProd` (indicial sums and products) | `src/csl/` | unported | 1 | |
| General symmetry permutations | `src/csl/` | unported | 1 | |
| Signed and symbolic-dimension spaces | `src/csl/` | unported | 1 | |
| SymPy bridge | none in C++ | unported | later | Decide whether this belongs in pymarty at all. |
| Python pickling | none | unported | later | |
| Library generator | `src/csl/` library generation | unported | 6 | Phase 6 is `pymarty.simulation`. |
| Model and calculation layers | `src/marty/` | unported | 4 to 7 | Phases 2 to 7. |

## 7. Maintenance

- Add a row when a feature is found in C++ and is not ported.
- Move a row to **ported** only in the change that adds the Python binding and its tests.
- Record every deferral in the Notes column with the reason and the phase.
- Keep this list in sync with `docs/architecture/pymarty/phase-1/port-table.md` for phase 1
  (`python/tests/symbolic/test_port_table.py`).
