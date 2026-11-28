Status: READY
Updated: 2026-11-17
Author: investigator

# Phase 1 investigation: equality, hash, and printing of `csl::Expr`

Read-only investigation. Evidence is from reading code and grep/git output. No build, no test run.

## 1. Question

1. What do `==`, `isIdenticalTo`, `<`, and the other compare-like functions on `csl::Expr` mean
   (structural or mathematical, commutativity, numbers, indices, parent identity of constants and variables)?
   Is any existing hash or total order consistent with equality? Can a sound `__hash__` be defined, or must
   `Expr` be unhashable with a separate `structural_key()`?
2. How do expressions print (`print`, `operator<<`, `printLaTeX`, `printCode`)? Which stream do they use,
   which global flags do they depend on, how can a string be obtained without side effects, and is the
   output deterministic?

## 2. Facts

### Equality

1. `isIdenticalTo` is not present. Searches: `git log -S isIdenticalTo` over all history (empty); `git grep`
   at HEAD, `fe4a415`, and `20b1475` (empty); no match in `src/csl`, `src/marty`, `python`, `tests`, `examples`.
2. The free `operator==(Expr, Expr)` returns true on pointer identity, otherwise calls the virtual
   `Abstract::operator==(Expr_info)`: `src/csl/abstract.cpp:1318`. The virtual is pure, declared in
   `src/csl/abstract.h` (`Abstract::operator==`, ~line 1212) with a doc comment that says two objects with the
   same name compare equal "even if they are not mathematically equal".
3. Equality is structural, not mathematical. Existing test: `prod_s({2,x,x*x,y})/y*y != 2*pow_s(x,3)`
   in `tests/units/csl/test_equality.cpp`.
4. `Constant::operator==` (`src/csl/literal.cpp:239–252`) and `Variable::operator==` (`literal.cpp:475–488`)
   require the same type, same complex properties, and the same parent object: `parent.get() == expr->getParent_info()`.
   Two `Constant`s with the same name are unequal if they have different parent objects.
5. `constant_s` and `variable_s` create a new parent per call (`src/csl/literal.cpp:665–670` and `695–722`,
   `csl::make_shared<ConstantParent>(name)`). No interning was found. By reading, `constant_s("a") != constant_s("a")`.
   The existing test pins `x != variable_s("x")` (`test_equality.cpp`).
6. Ordering of constants and variables is by name, not parent identity: `alphaRule` at
   `src/csl/simplification.cpp:89–97` compares names, then conjugation. So `<` and `==` disagree for two
   distinct parents with the same name (neither less, but not equal).
7. Numeric equality is by value across types. `Float::operator==` (`src/csl/numerical.cpp:141`) and
   `IntFraction::operator==` (`numerical.cpp:631`) compare `evaluateScalar()`. `IntFraction::evaluateScalar` is
   `(num * 1.) / denom` (`numerical.cpp:526–528`). Tests pin `Float(0.) == Complex(0.,0.)`,
   `intfraction_s(3,2) == intfraction_s(6,4)`, and `int_s(3) == IntFraction(-9,-3)`.
8. `Integer::operator==` (`numerical.cpp:328–345`) compares `value == static_cast<int>(expr->evaluateScalar())`
   (line 338). `value` is `long long` (`numerical.h:84`), but `evaluateScalar()` returns `long double`. Converting
   an out-of-`int`-range `long double` to `int` is undefined behaviour in C++. By reading only: `Integer` equality
   is wrong for `|n| > INT_MAX`. Not run.
9. `Sum::operator==` (`src/csl/operations.cpp:908–934`) is order-independent: it matches terms by erasing from a
   copy of the list (`indicesLeft.erase`, line 934). `Prod::operator==` (`operations.cpp:2225–2300`) is also
   order-independent (multiset matching). `Pow::operator==` (`operations.cpp:3095`) is ordered on base and exponent
   (read earlier; not re-read for this report).
10. Equality depends on global mutable state. `Comparator::getDummyComparisonActive()` is checked at
    `operations.cpp:910`, `2227`, `2244`. `Prod::operator==` toggles it (`setDummyComparisonActive(false)` at 2246,
    `true` at 2266, 2271, 2277). `option::checkCommutations` is checked in the same function
    (`operations.cpp:2253`, `2299`). So the same two objects can compare differently under different flags.
11. `Index::operator==` (`src/csl/index.cpp:270–280`) compares type, `nameOrValue` (by absolute value for
    non-fixed indices), `id`, and `space`. `Index == int` (`index.cpp:282–285`) compares `static_cast<int>(nameOrValue)`.
12. Ordering (`Expr` `<`) depends on global `csl::option::fullComparison` (`src/csl/options.h`,
    `inline bool fullComparison`, default true). The free `operator<` in `abstract.cpp:1351` uses it.
    `Sum::insert` and `Prod::insert` use `<`/`>` on `Expr` (`operations.cpp:1702`, `1753`), so argument order
    depends on the flag.
13. Ordering on tensor points is pointer-based (appears; by reading). `class Tensor : public std::shared_ptr<TensorParent>`
    (`src/csl/indicial.h:638`) defines no `operator<`. `ruleO6` (`simplification.cpp:287`, used at 1112) calls
    `getPoint()` comparisons, which would fall back to `std::shared_ptr` comparison (raw address). Run-to-run order
    could differ. Not run.
14. No hash exists. Searches for `std::hash`, `hash<`, `hash_value`, `__hash__`, `hash(` in `src/csl`,
    `src/marty`, `python`, `tests`, `examples` returned nothing.

### Printing

15. `operator<<(std::ostream&, const Expr&)` calls `obj->print(1, fout)` (`src/csl/abstract.cpp:1211`). Mode 1 means
    no trailing newline.
16. `print(int mode, std::ostream& out = std::cout, LibraryMode libMode = NoLib)` is pure virtual in
    `src/csl/abstract.h:108–110`. The default stream is `std::cout`.
17. `printCode(int mode = 0, std::ostream& out = std::cout)` (`abstract.h:112`). Default implementation calls
    `CALL_SMERROR` (`abstract.cpp:86–90`). `printLaTeX(int mode = 0)` returns `std::string` (`abstract.h:143`).
    Default implementation calls `print()` and `CALL_SMERROR` (`abstract.cpp:170–174`).
18. `Constant::print` (`literal.cpp:129–154`) writes `endl` when `mode == 0`. It changes names when
    `libMode != NoLib` (backslash becomes underscore) and wraps conjugates in `conjq(`, `std::conj(`, or `conj(`
    depending on `libMode`. `printProp` appends `^(*)` for conjugates (`abstract.cpp:141–144`).
19. `Sum::print` (`operations.cpp:197–219`) and `Prod::print` (`operations.cpp:1383`) write to the given stream and
    `endl` when `mode == 0`. `Sum::printLaTeX` (`operations.cpp:233`) and `Prod::printLaTeX` (`operations.cpp:1483`)
    build strings, with no stream and no flags read directly.
20. `Constant::printCode` (`literal.cpp:156–176`) tests identity with the built-in constants (`*this == CSL_PI.get()`,
    etc.) and prints `CSL_PI` only then. A user constant named `pi` prints as `pi`.
21. Flags read in printing: `option::printErrors` gates CSL error output (`src/csl/error.cpp:80` and `92`).
    `option::printIndexIds` gates index id output (`src/csl/index.cpp:394`). No `fullComparison` or
    `checkCommutations` read was found in the `print`/`printLaTeX` bodies read (`Sum`, `Prod`, `Constant`,
    `Integer`). Output order inherits the ordering of point 12.
22. Side effects on `std::cout` that do not go through the stream argument:
    `Abstract::printExplicit` (`abstract.cpp:156–166`, always `cout`), the warning in `Constant::evaluateScalar`
    (`literal.cpp:550–551`), and debug output in a consistency routine (`operations.cpp:436–482`, function name not
    recorded).
23. Side-effect-free string output exists: `printLaTeX(mode)` returns a string. `print` and `printCode` accept any
    `std::ostream`, so an `std::ostringstream` avoids `std::cout`. `tests/units/csl/test_latex.cpp:587` already uses
    `std::ostringstream terminal;`. Printing a type that has no `printLaTeX` override calls `CALL_SMERROR`
    (`src/csl/error.h:80`), which reports on stderr when `printErrors` is on.
24. Printing is deterministic for a fixed tree and fixed flags (pure string building in the `printLaTeX` bodies). The
    tree's argument order may vary with point 13 (not run).

## 3. Existing tests and data

- `tests/units/csl/test_equality.cpp` (68 lines): pins from points 3, 5, 7, 9, 10. Covers `Float(0.) == Complex(0.,0.)`,
  `intfraction_s(3,2) == intfraction_s(6,4)`, `int_s(3) == IntFraction(-9,-3)`, `prod_s(...)/y*y != 2x^3`,
  `T22({i1,i2}) != T22({i2,i1})` (tensor index order matters). Does not cover hash, does not cover `Integer` above `INT_MAX`,
  does not cover `checkCommutations` or dummy-state effects on `==`.
- `tests/units/csl/test_latex.cpp`: `Expr(-3)->printLaTeX(2) == "\\left(-3\\right)"` (line 21), `(a+b)->printLaTeX(1)` and `printLaTeX(2)`
  (lines 61–62), conjugate `c^{*}` (line 586), `ostringstream` use (line 587). Golden strings are inline; no golden-file
  directory exists under `tests/api/`.
- `tests/units/csl/test_relabel_indices.cpp`: many `printLaTeX(0)` string comparisons (lines 25, 191–194, 386, 403, 417–421).
- `docs/architecture/pymarty/phase-1-symbolic.md` §2 and §5.3, §5.6; `docs/architecture/pymarty/api-conventions.md` §1.5,
  §1.7, §2.5 (plan, not implementation).

## 4. Options and trade-offs

Equality (`==`) keeps its current meaning in all options (decision D8: existing C++ behaviour unchanged).

Option A: `Expr` unhashable (`__hash__ = None`) with a separate `structural_key()`.
- Cost: `structural_key()` needs its own canonical form. It cannot reuse `printLaTeX` or `printCode`:
  the output shows names only, so two distinct constants named `a` would get the same key while `==` says they differ
  (points 4, 5). The key must also be order-independent for `Sum`/`Prod` and must encode numeric value across
  `Integer`, `IntFraction`, `Float`.
- Benefit: no false hash-equality claim. Reproducible if the key uses names; key collisions are then coarse but harmless.

Option B: a sound `__hash__` consistent with `==`.
- Cost, from the points above: name-based hashing is sound (equal parents imply equal names), but parent-pointer identity
  is not reproducible across runs. Numeric hashing must use the same value the equality uses (point 8 needs fixing first).
  `Sum`/`Prod` need an order-independent combine. Equality depends on `Comparator` dummy state and `checkCommutations`
  (point 10), so the hash is only consistent while those flags are fixed. Point 13 (pointer ordering) must not leak into keys.

Printing output for Python:
- Option P1: use `printLaTeX` for LaTeX and an `std::ostringstream` for `print`/`printCode`. Cost: `printLaTeX` on
  unsupported types still reports through `CALL_SMERROR`, and `print` of `Constant` may still touch `cout` through
  `printExplicit`/`evaluateScalar` warnings (points 17, 22).
- Option P2: as P1, plus the facade sets `option::printErrors` and checks for stray output (test-level check).

Opinion: Option A plus P1. The equality already depends on global state (point 10), so no hash can be consistent across
all flag states. A separate canonical key, with its own pins, is easier to test than a hash that must match `==`.

## 5. Questions for the user

1. Should `Expr` be unhashable (`__hash__ = None`) with a `structural_key()`, or should a hash be required (accepting that it
   is only valid under fixed `Comparator` and `checkCommutations` state)?
2. For `structural_key()`, should constants and variables be identified by name (coarse, may merge distinct
   `constant_s("a")` objects) or by parent identity (exact, not reproducible across runs)?
3. `Integer::operator==` casts to `int` (point 8). Fix it in phase 1 (changes existing C++ behaviour, against D8) or record it
   as a known limitation?

## 6. Not verified

- No build and no test run. All behaviour is from code reading. Points 8 (`Integer` UB), 5 (fresh parent per call), 13
  (pointer ordering of tensor points), and 24 (determinism) are not run.
- `Pow::operator==` (`operations.cpp:3095`) and the `Sum`/`Prod` `print` bodies beyond the lines cited were read in an earlier
  session and not re-read for this report.
- Whether `getMatrixChains` (used by `Prod::printLaTeX`) reads any global flag.
- The function containing the debug output at `operations.cpp:436–482` and whether it is reachable in normal use.
- The API `Session` setting of `printErrors` (not re-checked).
- `isIdenticalTo` absence: checked by `git log -S`, `git grep` at HEAD, `fe4a415`, `20b1475`, and a source grep. No other branch was grepped.
- Line numbers marked "~" are approximate. Cite by symbol name when in doubt.
