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
| Index access | `getNIndices`, `getIndex`, `getIndexStructure`, `getFreeIndexStructure` | ported as `index_count`, `index(i)` and `index_structure`. `index_count` is 0 for non-indicial expressions (only `TensorElement` overrides `getNIndices`). `index(i)` raises `MartyLookupError` when out of range. `index_structure` is the raw CSL structure, with repetitions, and is empty for expressions without indices. `getFreeIndexStructure` stays covered by `free_indices`. |
| Numerical parts | `getNum`, `getDenom`, `getSign`, `getParity`, `getOrder` | `numerator` and `denominator` ported (rationals and integers; integers have denominator 1; other kinds raise `MartyTypeError`). `getSign` and `getOrder` are deferred (user decision): their `Abstract` default throws, and only `Commutator`, `Polynomial` and `Derivative` override them. `getParity` is unported. CSL `getNum` returns 0 silently on wrong kind (`src/csl/abstract.cpp:189-192`), so the facade raises instead. |
| Complex parts | `getRealPart`, `getImaginaryPart`, `getComplexArgument`, `getComplexModulus`, `getComplexConjugate`, `getTransposed`, `getPolynomialTerm` | Evaluated transforms already exist: `conjugate()`, `real_part()`, `imag_part()`, `modulus`, `argument`. The raw accessors are unported. `getTransposed` takes a `Space*` (needs `tensor.h`), and `getPolynomialTerm` needs a variable and an order. |
| Values and types | `getValued`, `getValue`, `getType`, `getPrimaryType`, `getName`, `getLatexName`, `getShape`, `getDim` | ported as `valued`, `value`, `type` (`ExprType`), `primary_type` (`PrimaryType`), `dim`, `shape`, `latex_name`. `getName` is not re-bound: the existing `name` property already covers it, but it raises `MartyTypeError` on non-symbols whereas CSL returns `""`. `value` raises `MartyTypeError` when not valued; `valued` is true for real numbers and valued constants or variables. |
| Symmetry and contraction | `getPermutations`, `getCanonicalPermutation`, `getContractedPair`, `getNContractedPairs`, `getContractionProperties`, `getNFactor` | unported |
| Boundaries and ranges | `getInfBoundary`, `getSupBoundary`, `getRegularExpression` | unported |
| Vector and subvector | `getSubVectorial`, `getVectorialModulus`, `getSubSymbols` | unported |
| Parents and points | `getParent`, `getPoint` | partial: `Expr.parent_id` ported (an integer identity, `None` without a parent). Parent objects are not exposed. `getPoint` deferred. `getParent` returns a null parent for a legitimate absence (`src/csl/abstract.cpp:260-263`). |
| Dependencies and libraries | `getAllDependencies`, `getLibDependency`, `getCommutable`, `getElementary` | unported |

### 2.2 Predicates

Ported as read-only properties: `is_building_block`, `is_arbitrary`, `is_indexed`, `is_real`,
`is_purely_imaginary`, `is_complex_conjugate`, `is_hermitian_conjugate`, `is_integer`, `is_an_operator`.

Unported:
- `isEmpty`: its `Abstract` default throws (`src/csl/abstract.cpp:623`), and only `Operator` overrides it.
- `isPolynomial` and the `has*` virtuals (`hasContractionProperty` takes an `Expr_info`, so it needs a
  design before porting).

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
| `TensorParent` | `src/csl/indicial.h:242+` | partial: `Tensor.name`, `spaces`, `fully_symmetric`, `fully_antisymmetric` ported. Unported: `getSymmetry`, `getPermutation`, `getTensor`, `getTrace`, `getDim`, `getType`, `getPrimaryType`, `isValued`, `isTraceLessIn` | `getTensor` and `getTrace` may be exempt from Python; see addendum. `getPermutation` and `getSymmetry` need a design for symmetry specs. |
| `TensorField` | `src/csl/tensorField.h:34-139` | partial: `TensorField.name`, `point_space`, `index_spaces`, `primary_type` ported. Unported: `getArgument`, `getOperand`, `getIndexStructure`, `getSubSymbols`, `getLatexPointSuffix`, `getComplexConjugate`, `getType` (CSL returns `TensorFieldElement` for the field parent, a value to clarify before exposing), `isIndexed` | |
| `Space` | `src/csl/space.h` | `name` and `dimension` ported | |
| `Tensor` | `src/csl/api/` | `__call__` and read-only getters above (ported) | |
| `TensorDerivative` | `d(mu, X)` | `__call__` only (ported) | Rule in `AGENTS.md`: `d(mu, X)(operand)` only. |

## 5. Callback-based algorithms

These take C++ function callbacks, so they cannot be ported by binding. Plan: Python duplicates
(rules section 6), or declarative C++ predicates when the predicate is simple.

| Algorithm | CSL reference | Status | Notes |
|-----------|---------------|--------|-------|
| `AnyOfLeafs`, `AllOfLeafs`, `AnyOfNodes`, `AllOfNodes` | `src/csl/algo.h` | ported (`symbolic_algorithms.py`) | Parity: `SymbolicAlgorithmsApi.GoldenValuesStable`. |
| `FindIfLeaf`, `FindIfNode`, `VisitEachNodeCut`, `VisitFirstOf*` | `src/csl/algo.h` | ported (`symbolic_algorithms.py`) | Same parity test. |
| `expand` (full `ExpandAll`-style) | `src/csl/interface.cpp` | ported (`Expr.expand`) | Non-callback. |
| `Copy`, `DeepCopy` | `src/csl/utils.h` | ported (`Expr.copy(deep=)`) | Non-callback. |
| `Refreshed`, `DeepRefreshed` | `src/csl/utils.h` | ported (`Expr.refresh(deep=)`) | Non-callback. `rebuilt` was removed: the algorithms rebuild sums, products and powers with the operators, and raise `NotImplementedError` for other nodes whose arguments change. |
| `ExpandIf` | `src/csl/operations.cpp` (`Prod::expand_if`, `Pow::expand_if`) | ported (`symbolic_algorithms.expand_if`) | Products and integer powers of sums. Other function nodes with arguments raise `NotImplementedError`: the `Exp`, scalar, duo and multi functions, vectorial and tensor-derivative overrides still need porting. |
| `DistributeIf` | `src/csl/interface.cpp:256-290` | ported (`symbolic_algorithms.distribute_if`) | The CSL mutating overload now writes the result back (`expr = Refreshed(finalRes)`). Golden parity in `SymbolicAlgorithmsApi.GoldenValuesStable`. |

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
| Built-in `THDM_Model<type>` (template; one variant to choose) | `src/marty/models/thdm.h` | unported | later (after 4) | Deferred by Q1=C (phase 4). Variant choice open (Q2, "see later"). |
| Built-in `MSSM_Model`, `PMSSM_Model` | `src/marty/models/mssm.h`, `pmssm.h` | known gap at end of phase 4 (see phase-4 implementation log) | later | Deferred by Q1=C (phase 4). `samplePMSSM` moves with PMSSM. |
| Built-in `NMFV` model | `src/marty/models/nmfv.h` | known gap at end of phase 4 (see phase-4 implementation log) | later | Field content only was ported, then dropped as too complex to validate. |

## 7. Maintenance

- Add a row when a feature is found in C++ and is not ported.
- Move a row to **ported** only in the change that adds the Python binding and its tests.
- Record every deferral in the Notes column with the reason and the phase.
- Keep this list in sync with `docs/architecture/pymarty/phase-1/port-table.md` for phase 1
  (`python/tests/symbolic/test_port_table.py`).
