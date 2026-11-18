Status: READY
Updated: 2026-11-17
Author: subagent (investigator)

# Investigation: naming and duck typing (phase 1)

## 1. Question

Which name rules does CSL enforce for constants, variables, spaces, indices, tensors and abbreviations (empty names, characters, duplicates, reserved names)? Which C++ entry points accept which numeric kinds, so that the coercion order of `Expr(...)` in the plan (`docs/architecture/pymarty/phase-1-symbolic.md`, `03-spec.md:119`) maps onto real C++ calls?

## 2. Facts

1. CSL enforces no name rules at symbol creation. `constant_s` and `variable_s` overloads pass the name straight to a parent, with no check (`src/csl/literal.cpp:665-720`). The parent constructors take `std::string const &` (`src/csl/variableParent.cpp:91-103`, `:151-163`). The spec row "`ValueError` for an invalid name" (`03-spec.md:121`) therefore has no C++ rule to mirror. It needs a Python-side rule or must be dropped.

2. Name mangling for generated C++ is done by `Abstract::regularName`, which rewrites and never rejects (`src/csl/abstract.cpp:97-123`). An empty name becomes `_`. Letters, digits and `_` are kept. `+` becomes `p`, `-` becomes `m`, `~` becomes `s`. `[` and `]` are kept, which is not valid in a C++ identifier. Every other character becomes `_`. A leading digit gets a `_` prefix. The mapping is not injective: `"a+b"` and `"apb"` give the same identifier. `Abstract::regularLiteral` (`abstract.cpp:130-140`) escapes only `\`.

3. A second, different mangler is used for particle and mass names in the library generator. `LibraryGenerator::regularName` (`src/csl/librarygenerator.cpp:1263-1287`) drops every character that is not a letter, `_` or digit, and drops leading non-letters. So `"a-b"` gives `ab` here but `amb` under `Abstract::regularName`. The two manglers disagree, and `"a-b"` and `"ab"` collide in the library generator. It is used at `librarygenerator.cpp:419-489` and `:1085-1089`.

4. Duplicate names are allowed, and nothing checks them:
   - Constants and variables: each call creates a new parent, and `Constant::operator==` compares parent identity (`src/csl/literal.cpp:239-250`). This matches spec decision Q1 (`03-spec.md:22`).
   - Spaces: the `Space` constructors only assert that the default index names are non-empty (`src/csl/space.cpp:63-130`, asserts at `:75`, `:98`, `:123`). No name registry was found.
   - Abbreviations: the duplicate check in `Abbrev::addAbbreviation` is commented out (`src/csl/abreviation.cpp:108-120`). `find_opt` returns the first match.

5. Index identity is (space, name, id, type), not name alone. `Space::getID` gives an increasing id per name within a space (`src/csl/space.h:383-391`). `Space::generateIndex(name)` uses it (`space.h:400-416`). `Index::operator==` compares space, id, type and `abs(nameOrValue)` (`src/csl/index.cpp:270-280`). The test fixtures rely on this: `i1` and `i2` are both `generateIndex("i")` (`tests/units/csl/csltestdata.h:23-24`).

6. Index names are mapped to a per-space character code. `Space::getSpecFromIndexName` (`src/csl/space.h:338`) stores names in `nameIndices` and `specIndices` (`space.h:117-119`). It assigns codes from 1 up to the maximum of `char`. When all codes are used, the loop breaks and falls through to `specIndices[t_name]`, which default-inserts `0`. That path is not tested. Read only, not run.

7. Two parsing rules apply to names, and they are the only ones found:
   - `;` splits a LaTeX label: `getRegularAndLatexNames` (`src/csl/parent.cpp:24-37`, partial read). A `\;` escapes the split, and a trailing `;` does not split.
   - `_` splits a LaTeX subscript: `Space::getIndexLatexName` (`src/csl/space.cpp:52-60`).

8. Tensor constructors take the name and the spaces with no validation (`src/csl/indicial.cpp:871-955`). Duplicate tensor names are not checked.

9. The numeric entry points available to Python are `int_s(long long)`, `float_s(long double)`, `autonumber_s(long double)`, `intfraction_s(long long, long long)` and `complex_s(Expr const &, Expr const &)` (`src/csl/numerical.h:468-476`). There is no `std::complex` constructor and no rational class in the public API. Behaviour:
   - `float_s` returns an integer when the value is integral and within range (`src/csl/numerical.cpp:652-659`). So `Expr(2.0)` is the same as `Expr(2)`.
   - `intfraction_s(n, 0)` returns `CSL_INF` (`numerical.cpp:681-684`). Verified by `tests/units/csl/test_numbers.cpp:120`.
   - `complex_s(re, CSL_0)` returns `re` (`numerical.cpp:945-948`). A Python `complex` must therefore be built as `complex_s(float_s(re), float_s(im))`, because no C++ entry point takes `std::complex`.

10. Nothing in `python/` handles `__index__`, `__float__`, `__complex__`, `__marty_expr__` or `Fraction` for coercion yet (grep over `python/`). `python/src/module.cpp` only maps error kinds. The approved usage example already uses `Fraction` at `python/examples/usage/symbolic.py:16`, `:45` and `:100`.

### Searches run

- `src/csl/**`: name and constructor patterns (`regularName`, `regularLiteral`, `getRegularAndLatexNames`, `constant_s`, `variable_s`, `generateIndex`, `addAbbreviation`, `intfraction_s`, `float_s`, `complex_s`, `already exists`), plus reads of the cited ranges.
- `tests/units/csl/**`: throw checks, `regularName`, `regularLiteral`, duplicate-name fixtures. No test references `regularName` or `regularLiteral`.
- `python/**`: `__index__`, `__float__`, `__complex__`, `Fraction`, `__marty_expr__`, `numpy`, `ValueError`.
- `git --no-pager log` with name-related patterns, read-only. No commit adds name validation.

## 3. Existing tests and data

- `tests/units/csl/test_literals.cpp:5`: constant and variable printing for `a`, `c1`, `x`, `z1`. No invalid-name or duplicate test.
- `tests/units/csl/test_numbers.cpp:120`: `intfraction_s(5, 0) == CSL_INF`. `:503-520`: type-mismatch arithmetic throws `CSLError`, which is not a constructor check.
- `tests/units/csl/csltestdata.h:23-32`: duplicate-named indices (`i1`/`i2`, `mu1`/`mu2`) and tensors `T22`, `T33`, `T23`. These fixtures depend on same-name indices being distinct.
- `tests/units/csl/test_equality.cpp:67`: a tensor index-type check is commented out (`// EXPECT_THROW(T22({i1, I2}), CSLError)`). No such check exists.
- Not covered by any test: name characters, mangling in `regularName`, empty names, duplicate spaces, duplicate tensors.

## 4. Options and trade-offs

Name validation for `constant`, `variable`, `Space`, tensor:
- A. Python-side check only, for example reject `""`. Cheap, no C++ change. CSL keeps accepting everything else, and the spec wording becomes true for the empty name only.
- B. No check, as CSL does. Names are labels, and mangling happens only in generated code. Cheapest and consistent with Q1. Invalid names show up only in C++ output.
- C. A C++ validator in `src/csl/api/`. Consistent with the rule that logic lives in C++, but needs a spec amendment, and it introduces a rule CSL does not have.
- Opinion: A for the empty name, since CSL turns it into `_` and the result is meaningless. Otherwise B, and change the spec wording at `03-spec.md:121`.

Duplicate names:
- Constants and variables: already decided (Q1, CSL identity). No choice left.
- Spaces: allowed by CSL. Options are identity by object (as the handle model in the spec) or rejecting a repeated name in Python. Opinion: identity by object, no rejection, as for constants.

Coercion order of `Expr(value)`:
- Plan order: `Expr`, `__marty_expr__`, `__index__`, rational-like, `__complex__` (if imaginary part non-zero), `__float__`, with `bool` rejected.
- Opinion: reject `bool` before the other checks, because `bool` has `__index__`. Put `__index__` before `__float__`, so that large integer-like objects keep their exact value. Put `Fraction` before `__float__` for the same reason. Map to C++: `int` to `int_s`, `Fraction` to `intfraction_s`, `float` to `float_s`, `complex` to `complex_s(float_s(re), float_s(im))`.

## 5. Questions for the user

1. Empty or invalid names for `constant`, `variable`, `Space` and tensors: reject in Python (option A, empty name only), or accept as CSL does (option B)? The spec row `03-spec.md:121` needs one answer.
2. Should the library generator collision (fact 3: `"a-b"` and `"ab"` in `LibraryGenerator::regularName`) be recorded for Phase 6, or fixed now? Opinion: record for Phase 6, since phase 1 does not generate libraries.

## 6. Not verified

- Nothing was built or run. All facts come from reading the cited lines.
- `Space::getSpecFromIndexName` after 127 names in one space (fact 6): read only, not run.
- `getRegularAndLatexNames` past line 37 (fact 7) was not read to the end.
- `float_s` with `inf` or NaN falls through to `Float(value)` (`numerical.cpp:652-659`). What CSL does with a `Float` infinity was not checked.
- `intfraction_s(LLONG_MIN, -1)` negates `num` without an overflow check (`numerical.cpp:681-687`). Not checked whether this is reachable from Python.
- How pybind11 in `python/src` converts Python ints beyond `long long`, and how `__index__`/`__float__` are dispatched. No binding code exists yet to check.
- Index-related issues already noted by the indices investigation (`Index(char)`, `index.cpp:116`) were not re-verified here.
- Abbreviations are out of scope for phase 1 (`03-spec.md:55`, `:273`). Their duplicate behaviour is recorded but not analysed further.
- `git log --all` was searched with one pattern only. The `api/gamma` branch was not checked.
