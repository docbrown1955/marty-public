Status: READY
Updated: 2026-11-17
Author: investigator

# Phase 1 investigation: core classes for the `ExprHandle` facade

Branch `pymarty`, HEAD `8180807`. Phase 0 is already on this branch (commit `eaa9ce6`). Read-only; no build, no test run, no edits outside this file.

Line numbers are from reads in this session unless marked "earlier read".

## 1. Question

Which CSL classes and constructors must the `ExprHandle` facade wrap (numbers, constants and variables, operators, math functions)? What are their exact names and headers? How is `Expr` copied and shared, how is the node kind inspected, what canonicalisation happens for `sin(0)`, `exp(0)`, `log(1)`, `sqrt(4)`, and what happens on invalid input?

## 2. Facts

### 2.1 Expression handle and sharing

- F1. `class Expr : public std::shared_ptr<csl::Abstract>` at `src/csl/abstract.h:1494`. Copying an `Expr` copies the pointer; both handles share one node.
- F2. `Expr_info`, `Expr_c`, `unique_Expr`, `vector_expr` are the raw and unique pointer aliases in `src/csl/typedef.h` (earlier read).
- F3. `Constant::copy_unique()` makes a new `Constant` that shares the same `parent` (`src/csl/literal.cpp` ~200–235, earlier read). Copying a node does not copy its parent object.
- F4. `operator==(const Expr &a, const Expr &b)` is declared at `src/csl/abstract.h:1580`. Member `Abstract::operator==` is pure virtual at `src/csl/abstract.h:1212`, with wrappers at 1216–1218.

### 2.2 Node kind inspection

- F5. `enum class PrimaryType` at `src/csl/enum.h:31`; `enum class Type` at `src/csl/enum.h:47`; `enum class ComplexProperty` at `src/csl/enum.h:134` (`Real`, `Imaginary`, `Complex`, per earlier read).
- F6. Inspection is by `getPrimaryType()` and `getType()`. Used in the code at `src/csl/operations.cpp:3134–3135` (`leftOperand->isInteger()` and `rightOperand->getType() == csl::Type::IntFraction`) and at `src/csl/operations.cpp:3163–3165` (`getPrimaryType() == csl::PrimaryType::Numerical`).

### 2.3 Constructors the facade must wrap

Numbers (`src/csl/numerical.h`):

- `Expr float_s(long double)` line 468
- `Expr int_s(long long int)` line 470
- `Expr autonumber_s(long double)` line 472
- `Expr intfraction_s(long long int num, long long int denom)` line 474
- `Expr complex_s(Expr const &real, Expr const &imag)` line 476
- Singletons `CSL_0`, `CSL_1`, `CSL_2`, `CSL_M_1`, `CSL_M_2`, `CSL_HALF`, `CSL_THIRD`, `CSL_M_HALF`, `CSL_M_THIRD` at lines 485–501.

Literals (`src/csl/literal.h`):

- `constant_s(name, [value | Expr value], prop)` overloads at lines 500–507.
- `variable_s(name, [value | Expr value], prop)` overloads at lines 511–516 (only the first two are read; the third overload is inferred).
- `class Imaginary` at line 383; singleton `CSL_I` at line 536.
- `CSL_PI` (542), `CSL_E` (549), `CSL_INF` (561), `CSL_UNDEF` (570) are declared in `src/csl/literal.h` (earlier read, not re-read this session).

Operators and functions:

- `Expr pow_s(const Expr&, const Expr&)` `src/csl/abstract.h:1396`; `sqrt_s(const Expr&)` line 1398; `sqrt_s(int)` line 1400.
- `sum_s`, `prod_s`, `fraction_s`, `minus_` are in `src/csl/operations.cpp` (earlier read: 946/996, 2362/2467, 2517, 1023).
- `exp_s` `src/csl/mathFunctions.h:194`; `log_s` line 277; `sin_s` line 443. Other trigonometric and hyperbolic functions are in `src/csl/mathFunctions.cpp` (earlier read; not all declarations re-read).
- `real_s`, `imaginary_s` in `src/csl/cslcomplex.cpp` (earlier read: ~187, ~359).

Not found in the code: a `type_of` function (not verified, see section 6).

### 2.4 Constants and variables: identity

- F7. `constant_s(name, prop)` creates a new `ConstantParent` on every call and wraps it in a new `Constant` (`src/csl/literal.cpp:665–670`). `variable_s` does the same in the same file (`src/csl/literal.cpp`, `variable_s` overloads after line 670).
- F8. `Constant::operator==` returns `parent.get() == expr->getParent_info()` (`src/csl/literal.cpp:239–250`). `Variable::operator==` does the same at `src/csl/literal.cpp:475–485`. Equality is therefore object identity of the parent, not name equality.
- F9. The existing unit test asserts this: `EXPECT_NE(a, csl::constant_s("a"));` and `EXPECT_NE(x, csl::variable_s("x"));` at `tests/units/csl/test_equality.cpp:28–29`.
- F10. The `operator==` documentation in `src/csl/abstract.h` (~1200–1230, earlier read) describes name-based equality. It contradicts F8 and F9.

### 2.5 Name handling

- F11. `regularName` (`src/csl/abstract.cpp:92–120`, earlier read) rewrites characters silently. `""` becomes `"_"`, and a digit-leading name gets a `_` prefix. No validation error is raised.
- F12. `regularLiteral` (`src/csl/abstract.cpp` ~125–140, earlier read) escapes backslashes.

### 2.6 Canonicalisation of special values

- F13. `exp(0)`: `exp_s` (`src/csl/mathFunctions.cpp:297`) returns `CSL_1` for a numerical 0 (lines ~300–306) and `CSL_E` for 1 (line 308).
- F14. `log(1)`: `log_s` (`src/csl/mathFunctions.cpp:410`) returns `CSL_0` for `CSL_1` (lines ~412–413). `log(E)` returns `CSL_1` (line 416 is the `CSL_E` check). `log(exp(x))` unwraps to `x`. No case for `log(0)` (earlier read).
- F15. `sin(0)`: `sin_s` (`src/csl/mathFunctions.cpp:645`) returns `CSL_0` for `CSL_0` (lines ~648–650). Arbitrary arguments stay unevaluated.
- F16. `cos(0)`: `cos_s` (`src/csl/mathFunctions.cpp:516`) returns `CSL_1` for `CSL_0` (lines ~521–522).
- F17. `tan(pi/2)` returns `CSL_INF` (`src/csl/mathFunctions.cpp:795–796`, and 798, 804, 806). This is a value, not an error.
- F18. `sqrt(4)` is not canonicalised. `sqrt_s(const Expr&)` at `src/csl/operations.cpp:3194` is `pow_s(x, intfraction_s(1,2))`. The integer-base branch at `src/csl/operations.cpp:3134–3137` only fires when `num < 0` or `num > denom`. For `1/2` it does not fire, so the result is `Pow(4, 1/2)`. Inferred from code, not run.
- F19. `pow_s` special cases (`src/csl/operations.cpp:3158–3184`): `CSL_INF` base returns after `std::raise(SIGSEGV)` (3158–3161). `0` base with a negative numerical exponent raises `SIGSEGV` too (3164–3170). `x^0` gives `CSL_1` (3176). `0^0` gives `CSL_UNDEF` (3177). `0^x` gives `CSL_0` (3181). `1^x` gives `CSL_1` (3186). `x^1` gives `x` (3188).

### 2.7 Invalid-input behaviour

- F20. Errors are a bare enum: `enum class CSLError` at `src/csl/error.h:31`. It is not a `std::exception`.
- F21. `CallErrorImpl` (`src/csl/error.cpp:75–95`) prints to `std::cerr` if `csl::option::printErrors` is true, then does `throw error;` (lines 83 and 95). The Phase 0 commit `eaa9ce6` added the `printErrors` guard (`git show eaa9ce6 -- src/csl/error.cpp`).
- F22. `csl::option::printErrors` defaults to `true` (`src/csl/options.h:42`, added in `eaa9ce6`).
- F23. Phase 0 added `csl::api::Error : std::runtime_error` with `ValueError`, `TypeError`, `LookupError`, `NotImplementedError`, `StateError` in `src/csl/api/error.h:29–67`. Aliases are in `src/marty/api/common/error.h:36–43` under `mty::api`. The base class is in namespace `csl::api`, not `mty::api` as written in the plan.
- F24. `tests/api/src/test_error_api.cpp:90–91` maps a thrown `CSLError::TypeError` or `ValueError` to `mty::api::TypeError` or `ValueError`. The mapping is done at the boundary; `python/src/module.cpp:179` and `:182` use `CALL_SMERROR_SPEC(CSLError::TypeError | ValueError, ...)`.
- F25. Free-symbol numeric conversion does not throw. `Constant::evaluateScalar` (`src/csl/literal.cpp:200`, earlier read) returns `std::nanl("2")` via `CSL_UNDEF`. `Imaginary::evaluateScalar` (`src/csl/literal.cpp` ~548, earlier read) prints a warning to `std::cout` and returns 0.
- F26. Existing unit tests use `ASSERT_THROW(..., CSLError)` for wrong-type arithmetic on numbers (`tests/units/csl/test_numbers.cpp:503–520`).

### 2.8 Imaginary and complex

- F27. `Complexified` (`src/csl/buildingBlock.cpp` ~85–180, earlier read) provides `isReal`, `isPurelyImaginary`, `getRealPart`, `getImaginaryPart`, `getComplexConjugate`, `getComplexModulus`, `getComplexArgument`, `getComplexProperty` (default `Real`), `setConjugated`, `operator==`. No positivity property exists.
- F28. `real_s` returns its input if real, `CSL_0` if purely imaginary, otherwise wraps the value (`src/csl/cslcomplex.cpp` ~187, earlier read). `imaginary_s` follows the same pattern (~359).
- F29. `Imaginary::getPrimaryType()` returns `PrimaryType::Literal`; `getType()` returns `csl::Type::Imaginary` (`src/csl/literal.h` 383–420, earlier read).

### 2.9 Git and layout

- F30. `git log`: `8180807` (HEAD, `pymarty`, "Update AI process"), `eaa9ce6` ("API Phase 0 - error system & basic pymarty"), `e555778` ("Docs for pymarty"), `11017f3` ("Plan").
- F31. Phase 0 added `python/` (pybind module, `python/src/module.cpp`), `tests/api/`, `src/csl/api/error.h`, `src/marty/api/common/{config,error,session}.h`, and `.github/workflows/python.yml`.
- F32. Gamma API commits exist on branch `api/gamma`: `74c0d30`, `56fd60b`, `0740f42`, `c43ef79`, `3704f25`. Not on `pymarty`.

## 3. Existing tests and data

- `tests/units/csl/`: `test_equality.cpp` (identity semantics, F9), `test_numbers.cpp` (wrong-type `CSLError`, F26), `test_literals.cpp`, `test_latex.cpp`, `test_init_sanitizer.cpp`, `test_metric_delta_invariant.cpp`, `test_relabel_indices.cpp`, `test_signed_refresh.cpp`, `csltestdata.h` (shared `a`, `b`, `x`, `y`, `c1`, `c2`, `z1`, `z2`). Not all files were read this session.
- `tests/api/src/`: `test_error_api.cpp`, `test_terminating_calls_api.cpp` (expects `CSLError` for invalid `printFunctionStack*` arguments, lines 149–179), `test_config_api.cpp`.
- Not found: any unit test for `sqrt(4)`, `sin(0)`, `exp(0)`, `log(1)` special values, or for `pow_s` `SIGSEGV` cases. Searched for `sqrt_s(4)`, `sqrt(4)`, `exp_s(CSL_0)`, `log_s(CSL_1)`, `sin_s(CSL_0)` in `tests/units/csl`, `tests/api`, and `python/`: no matches.

## 4. Options and trade-offs

Error model. The plan (`api-conventions.md` 1.6) assumes `std::runtime_error`-derived errors. CSL throws a bare enum (F20). Phase 0 bridges this at the boundary (F24). Option A: keep the bridge; the facade catches `CSLError` and rethrows `csl::api` types. Option B: change CSL to throw typed errors. Option A is smaller and keeps CSL untouched (opinion: A).

Equality. The plan assumes same name means same symbol. CSL uses parent identity (F8), and the existing test asserts it (F9). Option A: keep identity; the facade stores the `Expr` and never recreates it from the name. Option B: intern constants and variables by name in the facade (`constant_s` cache). Option B changes behaviour the existing test asserts (opinion: A, unless the spec requires name equality).

Special values that abort. `pow_s` with `0^negative` or `CSL_INF` base calls `std::raise(SIGSEGV)` (F19). `SIGSEGV` cannot be caught as an exception. Option A: the facade checks the base and exponent before calling `pow_s` and raises `ValueError`. Option B: fix `pow_s` to throw `CSLError::ZeroDivisionError` or `ValueError`. Option A is local to pymarty; option B changes CSL behaviour for C++ users too (opinion: A for phase 1, record B as a follow-up).

Numeric conversion. A free symbol converted to a number returns NaN silently (F25). The facade should check for free symbols before `evaluateScalar` if an error is required.

## 5. Questions for the user

1. Equality: keep CSL identity (F8, existing test F9), or require same name to mean same symbol in the facade? Options: (a) identity, (b) name interning in the facade.
2. `pow_s` abort cases (F19): facade pre-checks that raise `ValueError` (A), or change CSL to throw (B)?
3. Does the phase 1 spec require `sqrt(4)` to evaluate to `2`? CSL returns `Pow(4, 1/2)` (F18). Options: (a) accept CSL's form, (b) add canonicalisation in the facade, (c) change CSL.

## 6. Not verified

- Whether `type_of` exists anywhere in `src/csl` (not searched this session).
- Full declarations in `src/csl/mathFunctions.h` (only `exp_s`, `log_s`, `sin_s` were read).
- `Variable::evaluateScalar` behaviour.
- `complex_s` with `CSL_I` parts.
- `intfraction_s(1, 0)` behaviour (ZeroDivision path).
- Expr copy semantics beyond `shared_ptr` sharing (no `copy()` / `DeepCopy` read).
- `abstract.h` `operator==` documentation at 1200–1230 (only the declarations at 1212–1218 were re-read).
- `api-conventions.md` section 1.6 wording (not re-read this session).
- The exact line of the third `variable_s` overload and of `variable_s` bodies in `literal.cpp`.
- `sqrt(4)` output, `sin(0)`, `exp(0)`, `log(1)` results: all inferred from source. No program was run.
- Whether `CALL_SMERROR_SPEC` maps every `CSLError` value (only two lines read in `python/src/module.cpp`).
- No build, no `ctest`, no `pytest` was run.

## 7. Orchestrator verification (re-read, no build)

Checked by re-reading the cited lines in the main agent's session:

- F7 OK: `constant_s(name, prop)` creates a new `ConstantParent` on each call (`src/csl/literal.cpp:665-670`).
- F8 OK: `Constant::operator==` ends with `parent.get() == expr->getParent_info()` (`src/csl/literal.cpp:239-250`).
- F13 OK: `exp_s` returns `CSL_1` for 0 and `CSL_E` for 1 (`src/csl/mathFunctions.cpp:297-310`).
- F18 OK for `sqrt_s(const Expr&)`: `pow_s(operand, intfraction_s(1, 2))` (`src/csl/operations.cpp:3194-3196`). The integer-base branch (`operations.cpp:3134-3137`) needs `num < 0 or num > denom`, so 1/2 does not trigger it. Verified later: `sqrt_s(int)` (`operations.cpp:3198-3200`) calls `sqrt_s(int_s(number))`, which calls `pow_s`. `pow_s` has no perfect-square branch (`operations.cpp:3122-3140`). `Integer::exponentiation_own` with an `IntFraction` exponent calls `pow_s` again (`numerical.cpp:414-440`), so the numeric `Pow::evaluate` path also keeps `Pow(4, 1/2)`. Result: `sqrt(4)` is `Pow(4, 1/2)` in both the expression and the evaluated form. Side finding: `pow_s(4, 3/2)` returns `4 * Pow(4, 1/2)` (integer part extracted, not fully simplified).
- F19 OK: `SIGSEGV` paths at `src/csl/operations.cpp:3158-3161` (base `oo`) and `3163-3170` (base 0, negative numerical exponent).
- F20 and F21 OK: `CallErrorImpl` throws the bare enum (`src/csl/error.cpp:83`, `:95`). The message is only written to `std::cerr`, and only if `printErrors` is true.
- F22 OK: `inline bool printErrors = true;` at `src/csl/options.h:42`.
- F23 OK in part: `csl::api::Error` and subclasses are in `src/csl/api/error.h:29-67` (namespace `csl::api`, confirmed).

Not re-checked: F9, F10, F11, F12, F14 to F17, F25 to F29.

### 7.1 Gap: index and space lifetime (no topic file yet)

The plan's open item "Space and Index lifetime" has no investigation file. Facts read by the main agent:

- `Index` stores a raw `const Space *space` (`src/csl/index.h:84`). `Index::getName()` reads the name through `space->getIndexNameView(...)` (`src/csl/index.cpp:55-57`).
- User-created spaces: `DeltaParent`, `MetricParent` and `EpsilonParent` keep `this` (`src/csl/space.cpp:71`, `:118`, `:184`), so a `Space` must outlive every tensor built on it.
- Static spaces: `Minkowski` is a function-local `static const Space` (`src/csl/space.h:440-453`). `Euclid_R2`, `Euclid_R3`, `Euclid_R4` are `inline const` objects (`src/csl/space.h:464-474`).
- Group-owned spaces: `SemiSimpleGroup::getVectorSpace` creates `ColorSpace` with raw `new` (`src/marty/core/group.cpp:233-237`), and the destructor deletes them (`src/marty/core/group.cpp:143-146`).
- Flavour spaces: `FlavorGroup` owns its space in a `std::unique_ptr<csl::Space>` (`src/marty/core/flavor.h:77`, `src/marty/core/flavor.cpp:50-56`).
- Possible null dereference (inferred from code, not run): `TensorParent::checkIndexRequest` builds its error message with `index->getSpace()->getName()` (`src/csl/indicial.cpp:1558-1575`). An `Index` with a null space (default-constructed) would reach that line and crash before the `IndexError` is thrown. Similarly `Index::setValue` (`src/csl/index.cpp:112-124`) dereferences `space` in its error branch when `space` is null and the value is negative.
- `src/csl/index_new.h` has `indices[0] == first;` (comparison, no assignment, in the constructor from a single `Index`) and an `operator+` with no `return` (`src/csl/index_new.h`, end of the class body). `index_new.h` is included only by `src/csl/index_new.cpp`, which is otherwise empty. The claim that it is unused is verified by grep. Whether it is compiled into a target was not checked.

These facts feed question 4 in `02-questions.md`.
