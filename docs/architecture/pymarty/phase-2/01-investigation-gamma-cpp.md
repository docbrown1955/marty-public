Status: READY
Updated: 2026-11-20
Author: investigator

# Investigation: C++ gamma API facade and its tests (pymarty phase 2)

## 1. Question

What exactly does the existing facade `src/marty/api/gamma.{h,cpp}` and `gamma_api_error.h` expose (signatures and behaviour); do the throw sites sit at the lines the plan cites; how is the global 4D / symbolic-dimension flag implemented; which tests cover it and how are they wired in CMake; what does each planned C++ item need (alias and header, introspection, equals/less, scalars, deep copy, `ScopedSymbolicDimension` on `Session`, error translation to `mty::api::Error`); and do `src/marty/api/common/*.h` match `api-conventions.md` section 1 (and section 3 for `Session`)?

Method: read-only. Commands run: `cat`, `grep -n`, `git --no-pager log/show/branch/ls-tree`, `ls`, `wc -l`. Nothing was built, no `ctest` was run, `/home/gregoire/marty-public/build` was not touched. Searches covered `src/` (api, core, sgl, csl), `tests/` (integration, api, CMake), `python/`, `examples/` and the git history (`--all`, branch `origin/api/gamma`).

## 2. Facts

1. **Public surface (all in `namespace mty::gamma_api`).** `gamma.h`: `using Expr = sgl::GExpr;` (:14); `keepSymbolic4D(bool)` (:16); `enum class FierzBasis { Standard, Chiral }` (:18); `g(int,int)` (:21); `eps(int,int,int,int)` (:22); `gamma(int)` (:23); `gamma(int,int)` (:24); `gamma5()` (:25); `P_L()` (:26); `P_R()` (:27); `C()` (:28); `DMinko()` (:29); `imaginaryUnit()` (:30); `current(std::vector<Expr> const&, int iLeft, int iRight)` (:33); `trace(std::vector<Expr> const&)` (:34); `ordered(Expr const&)` (:36); `simplified(Expr const&)` (:37); `project(Expr const&, FierzBasis)` (:38); `applySingleFierz(chain1, chain2, FierzBasis = Chiral)` (:40-42); `applyDoubleFierz` (same shape, :44-46); `std::string generateString(Expr const&)` (:48); `std::string generateLatex(Expr const&)` (:49). The header has **no doc comments at all** and no Python names.

2. **Behaviour (`gamma.cpp`).** `g`/`eps`/`gamma(mu)`/`gamma(mu,nu)` build SGL index objects with Minkowski indices named `m<mu>` (`gamma.cpp:51-74`, `IndexManager::getMinkoIndex` at :420-428). `gamma5`/`P_L`/`P_R`/`C` are `gammaindex_s(5|6|7|8)` (:76-94). `DMinko`/`imaginaryUnit` are function-local `static const` handles (:34-44). `current` = `indexchain_s(gammas, d<iLeft>, d<iRight>)` with Dirac indices from `mty::dirac4` (:96-101, :430-437). `trace(g) = simplified(current(g,0,0))` (:103-106). `simplified` = DeepCopy, `sgl::Simplified`, `CSLSimplified`, `relative_chain_order`, `renameIndices`, `DeepRefreshed`, rename again (:137-144). `ordered` = DeepCopy, `sgl::OrderChains`, `simplified`, `relative_chain_order`, rename (:146-153). `project` converts each chain to the requested basis with `sgl::transform<IndexChain>` then `simplified` (:155-171). `applySingleFierz`/`applyDoubleFierz` take both arguments through `getIndexChainAssert`, apply `applyGeneralFierz[Twice]`, then `project(simplified(...))` and multiply by the CSL prefactors (:175-195). `generateString` is `operator<<` (:231-236). `generateLatex` is a recursive printer over `Sum`, `Prod`, `IndexChain`, `GammaIndex`, `MetricIndex`, `EpsilonIndex`, `CSLExpr` (:242-414).

3. **Throw sites: exactly five, all `GammaAPIError`, at the cited lines.** `gamma.cpp:200` (null expression in `getIndexChainAssert`, message "Invalid expression error (null)."); `:206` (not a 2-factor product and not a chain; message prints `expr`); `:216` (2-factor product whose factors are not `CSLExpr` + `IndexChain`); `:347` (unknown gamma-index kind in LaTeX); `:406` (unknown expression kind in LaTeX). `grep -n throw src/marty/api/gamma.cpp` returns only these five. The plan's ~200/206/216/347/406 are correct.
   **Bug at :211-219:** `auto expr = dynamic_cast<sgl::CSLExpr const *>(...)` (:211) shadows the parameter `expr` inside the `else` block, so the message at :216-219 streams a raw `CSLExpr const*` (an address), not the offending expression. Read-only conclusion; not run.

4. **Flag implementation: one global bool.** `keepSymbolic4D(bool symbolic)` assigns `sgl::option::keepEvanescentOperators = symbolic` (`gamma.cpp:46-49`). The flag is defined as `inline bool keepEvanescentOperators = true;` in `src/marty/sgl/sgloptions.h:27`, so the **default is symbolic (true)**. `mty::option::keepEvanescentOperators` is a reference alias to it (`src/marty/core/mrtOptions.h:67`). It is read by `src/marty/sgl/simplify.cpp:181` and `src/marty/core/sglsimplify.cpp:170`. The naming is inverted: `keepSymbolic4D(true)` means "keep evanescent operators" (symbolic d). No API function opens a `Session` around its computation: the gamma functions read the global directly.
   `Session` snapshots this flag: it is in `MTY_MARTY_OPTION_FLAGS` (`mrtOptions.h:95`, entry `X(sgl::option, keepEvanescentOperators)` at :122), which `Session` iterates (`src/marty/api/common/session.h:51`). Covered by `tests/api/src/test_session_api.cpp:107` `RestoresGammaSymbolicFlag` and by `OptionListCoversAllFlagsInHeaders` (:68).

5. **`gammaAPI.cpp`: 12 TEST cases, all in `marty_gamma_api`.** Lines: `equality` (:36), `metric_contractions` (:89), `epsilon_contractions` (:115), `gamma_contractions` (:176), `simple_simplification` (:245), `basis_projection` (:294), `simple_traces` (:356), `complex_traces` (:441), `chisholm_identities` (:558), `conjugation_identities` (:619), `simple_fierz_identities` (:707), `double_fierz_identities` (:806). Comparison goes through `sgl::sgl_to_csl` with a static `tensorSet` (`comparableExpr`, `COMPARE_SGL_EXPR`, `TEST_ALL_EPSILON_COMBINATIONS` near the top of the file). The only exception assertions are `EXPECT_THROW(trace({C()}), sgl::MathError)` and two siblings at :411, :414, :417. `trace({})` must equal `4` (:360-362), so an empty trace is valid, not an error. Every test starts with `keepSymbolic4D(false|true)` and most end by restoring `true`; there is no RAII.

6. **CMake wiring.** Facade: `src/marty/api/CMakeLists.txt:1-4` (`API_SOURCES` = `gamma.h gamma.cpp`), `:6` `add_library(marty-gamma-api OBJECT ${API_SOURCES})`. It is linked into `marty` and `marty-static` (`src/marty/CMakeLists.txt:18`, `:34`) and installed (`CMakeLists.txt:121`). `src/marty/api/common/*.h` are not listed in any target (header only). Integration test: `tests/integration/CMakeLists.txt:10` `ADD_INTEGRATION_TEST(gammaAPI)`; the helper (`:1-6`) links `marty` via `SETUP_MARTY_TEST_TARGET` and labels the gtest target `integration` via `SETUP_GTEST_TARGET` (`tests/CMakeLists.txt:5-11`, `:23-27`). `tests/api/CMakeLists.txt` (`ADD_API_TEST` list) does not include `gammaAPI`. Expected ctest names: `marty_gamma_api.<case>` (gtest_discover_tests, not run).

7. **Other tests that touch the error path.** `tests/api/src/test_error_api.cpp:94` `GammaApiErrorIsTranslated` (asserts only that the message is kept and the type is `mty::api::Error`). `python/src/module.cpp:196` `throw GammaAPIError(message)` in the `_debug.raise_error("gamma")` helper, checked by `python/tests/test_errors.py:87-90`. No C++ test asserts the concrete type of a `GammaAPIError` thrown by gamma functions.

8. **Git history / prototype.** The old pybinding is at `origin/api/gamma`: `src/pybinding/gamma_api.cpp`, `python/src/pymarty/gamma_api/`, `python/notebooks/sample_gamma_api.ipynb`, `examples/sampleGammaAPI.cpp`. `git show origin/api/gamma:src/pybinding/gamma_api.cpp` shows the introspection code the plan talks about: `expr->size()` and `expr->argument(index)` with a `std::out_of_range` check (lines 50-59), and every comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`) written as `sgl::sgl_to_csl(x, mty::defaultSGLTensorSet())` followed by the csl operator (lines 64-115). Commit `39f288f` is only a notebook output change. Local branch in use: `pymarty2`; `git log --all -- src/marty/api/gamma.cpp` shows the facade history.

9. **Name and header collisions to note.** `src/marty/core/diracology.h` (namespace `mty`, `defaultSGLTensorSet()` at :144, defined in `diracology.cpp:58`) already exists; `src/marty/api/` has no `diracology` anything. `gamma_api_error.h` is included by both `gamma.h` and `common/error.h` (`error.h:24-ish include`, `error.h` :22).

10. **Error types available.** `src/csl/api/error.h:32` `csl::api::Error : std::runtime_error`; `ValueError`, `TypeError`, `LookupError`, `NotImplementedError`, `StateError`, `ZeroDivisionError`, `OverflowError` are derived from it (:38-70). `src/marty/api/common/error.h:38-43` re-exports `Error`, `ValueError`, `TypeError`, `LookupError`, `NotImplementedError`, `StateError`; `BuildError` is defined locally (:46-49).

11. **Translation function.** `rethrowAsApiError()` (`common/error.h`, :~100-130) handles `Error` (unchanged), `mty::error::Exception`, `mty::error::Type`, `CSLError`, then **`GammaAPIError` → plain `mty::api::Error`** (:125-127), then `std::exception` → `Error` (:128), `...` → `Error`. It is used only by `tests/api/src/test_error_api.cpp:44` and `tests/api/src/test_terminating_calls_api.cpp:136,166`; the gamma facade does not call it.

12. **`sgl` exceptions are not core exceptions.** `src/marty/sgl/exceptions.h:32` `class Exception : public std::exception`; `:56` `class MathError : public Exception`. They do not derive from `mty::error::Exception`, so `rethrowAsApiError` would turn `sgl::MathError` into a generic `mty::api::Error`. That would break the three `EXPECT_THROW(..., sgl::MathError)` assertions at `gammaAPI.cpp:411-417` if the facade were wrapped.

13. **`GammaAPIError`.** Defined in the global namespace (`gamma_api_error.h:6`), derives from `std::exception` (not from `mty::api::Error`), has a **variadic template constructor** (`:8-9`, defined `:20-21`), and has **no include guard** (the file starts with `#include <exception>`).

14. **Items available for the plan.** `GExpr` is `std::shared_ptr<AbstractGammaSym>` (`src/marty/sgl/abstractgammasym.h:86`) with `size()` and `operator[]` (`:100-110`); `argument(i)` is virtual (`:46-48`); `GExpr(csl::Expr const&)` (`:94`); `operator*`/`operator+` with `csl::Expr` on either side (`:141-146`); `sgl::cslexpr_s(csl::Expr)` (`src/marty/sgl/cslexpr.h:72`); `sgl::DeepCopy` (`src/marty/sgl/sglinterface.h:29`); `sgl::sgl_to_csl(GExpr, TensorSet)` (`src/marty/sgl/cslconverter.h:33`). `GExpr` has **no** `operator==` / `operator<` (grep of `src/marty/sgl/*.h` finds only `operator<<`, abstractgammasym.h:155). `gammaAPI.cpp` uses `expr_s(4)` and `4 * g(0,1)` (:362, :372-ish), so an int coefficient converts implicitly.

15. **`Session`.** `src/marty/api/common/session.h:45-84`: ctor snapshots `Model::current` and the X-macro flags, sets `mty::option::printErrors` and `csl::option::printErrors` to false (:55-56), and sets `Model::current` if a model is given; dtor restores all of it. Single-threaded (:43). The only parameter is `mty::Model *model = nullptr` (:47). Non-copyable and non-movable (:61-64).

16. **Compatibility of `common/*.h` with `api-conventions.md` section 1.**
    - §1.1 (no templates in public signatures): `config.h` has a template, but at :243, inside the `private:` section (:214). `error.h`, `session.h`: OK. `gamma_api_error.h`: **violates** (variadic template ctor, :8-9).
    - §1.2 (plain data): OK in `config.h` (strings, optionals); no handles here.
    - §1.6 (typed errors): the hierarchy matches (`Error` / `ValueError` / `TypeError` / `LookupError` / `NotImplementedError` / `StateError` / `BuildError`). **Mismatches:** `GammaAPIError` maps to plain `Error`, not to a typed subclass (:125-127); `sgl::Exception` is not mapped (falls into `std::exception`, :128).
    - §1.7 (no output): `error.h` prints nothing; `session.h` silences printing by design (:55-56).
    - §1.9 (single-threaded documented): matches (`session.h:43`).
    - §1.10 (doc comment with Python name and one-line example on each public function): **not met** anywhere in `common/` (the file-level and class doc comments do not give Python names or examples), and **not met** in `gamma.h` (no comments).
    - §1.5 / §3 (Session): the snapshot covers the required state (flags, `Model::current`, the gamma flag; `session.h:51-52`, `mrtOptions.h:122`). **Mismatch with §3:** §3 says `Session` "applies the requested options" and that every API function that depends on global state "opens a `Session` internally". `session.h` takes no options struct (only an optional model, :47), and the gamma functions open no `Session`.

## 3. Existing tests and data

- `tests/integration/src/gammaAPI.cpp` (898 lines, 12 TEST cases, listed in fact 5). Covers equality, epsilon and metric contractions, gamma contractions, simplification, projection, traces (including the three sgl exception assertions), Chisholm identities, conjugation, single and double Fierz. It does **not** cover: `ordered`, `generateString`, `generateLatex` (no assertion on LaTeX text in this file: check with `grep -n generateLatex tests/integration/src/gammaAPI.cpp` before relying on this; not verified), the `getIndexChainAssert` error paths, null expressions, negative indices, or `FierzBasis::Standard` in `project` (only partly checked).
- `tests/api/src/test_session_api.cpp` (7 TEST cases): restores all flags, restores the gamma flag (:107), restores the model on exit and on exception, interleaved models.
- `tests/api/src/test_error_api.cpp` (9 TEST cases): hierarchy, translation of core and CSL enums, `GammaAPIError` translation (:94), `std::exception` (:102), nothing printed in quiet mode.
- No file under `tests/api/` or `tests/integration/` covers `diracology`; `python/examples/usage/` has `errors.py` and `symbolic.py` only (no `diracology.py`, the usage-example gate of the plan).

## 4. Options and trade-offs

**O1. Error type of the five throw sites and of `GammaAPIError`.**
- (a) Keep `GammaAPIError`, make it derive from `csl::api::Error` (include `src/csl/api/error.h`, which only needs `<stdexcept>`; no cycle). Costs: changes the class hierarchy of a header that is installed and used by `_debug.raise_error`; the variadic ctor still violates §1.1 unless rewritten.
- (b) Keep `GammaAPIError` and wrap every public gamma function in a try/`rethrowAsApiError` block. Costs: about 30 wrappers; still generic `Error` unless `rethrowAsApiError` maps `GammaAPIError` to a subclass.
- (c) Throw `mty::api::ValueError` (or `NotImplementedError`) directly at the five sites and keep `GammaAPIError` for compatibility. Costs: only five edits; `GammaAPIError` remains a non-API type. Opinion: (c) is smallest and matches the plan's "ValueError for invalid input".

**O2. `sgl::MathError` from `trace(C())` and siblings.**
- Keep passing through: no change to `gammaAPI.cpp:411-417` and to behaviour (§1.11 "behaviour stays intact"). Costs: the API leaks a non-API type.
- Translate to `NotImplementedError` / `ValueError`: matches the plan. Costs: the three assertions change and the change is visible to C++ users. Opinion: translate, and document it as a deliberate change in the test file.

**O3. `ScopedSymbolicDimension` implementation.**
- Built on `Session` (as the plan asks): restores everything on exit, also during unwinding. Costs: it also silences error printing for the scope (`session.h:55-56`) and takes a snapshot of about 40 flags on each use.
- A 3-line RAII guard that saves only `sgl::option::keepEvanescentOperators`. Costs: does not follow the plan's "built on Session" wording; §3 of the conventions wants one mechanism. Opinion: use `Session` as the plan says, and accept the printing side effect, or add a `Session` option to suppress the silencing if that matters.

**O4. Namespace alias syntax.** The plan writes `namespace mty::api::diracology = mty::gamma_api;`. A namespace-alias-definition takes a single identifier on the left (C++ grammar, [namespace.alias]), so this form should be rejected. The correct form is `namespace mty::api { namespace diracology = gamma_api; }` (from the grammar; not compiled). Forwarding header `src/marty/api/diracology/diracology.h` including `../gamma.h` is fine; note the basename clash with `src/marty/core/diracology.h` (only matters for includes from the same directory).

**O5. equals / less.** Option 1: convert with `sgl::sgl_to_csl(expr, mty::defaultSGLTensorSet())` (what the old binding did, `origin/api/gamma` `gamma_api.cpp:64-115`). Costs: the API then depends on `marty/core/diracology.h` (a core header, allowed from `marty/api`), and equality is only as good as csl canonicalisation. Option 2: a structural comparison of `GExpr` trees. Costs: new code, and must match `sgl_to_csl` results on the test cases. Opinion: option 1, since the test suite already relies on it.

**O6. Introspection.** `size` can be `expr->size()` (`GExpr::size`, `abstractgammasym.h:100`). `argument(expr, i)` must bounds-check and throw `LookupError` (the old binding threw `std::out_of_range`, which the plan wants gone). Costs: small.

**O7. `trace` of an empty list.** Currently valid and tested to equal 4 (`gammaAPI.cpp:360-362`). The plan's "hardening" item asks for validation of this input. Opinion: keep the result (it is correct and tested); do not add an error.

## 5. Questions for the user

1. Error type for the five sites: (a) make `GammaAPIError` derive from `csl::api::Error`, (b) translate at each entry point, or (c) throw `ValueError` / `NotImplementedError` directly and keep `GammaAPIError` for compatibility? Recommendation (c).
2. `sgl::MathError` from `trace({C()})` (`gammaAPI.cpp:411-417`): keep as is, or translate to `NotImplementedError` and change those three assertions?
3. `trace({})` returns 4 and is tested. Keep it (recommended), or make it an error as the plan's hardening list suggests?
4. Negative `mu`/`nu` indices: reject with `ValueError` (recommended if the behaviour is not already a clean error; see Not verified)?
5. `current(gammas, iLeft, iRight)` with user-chosen indices: the same integer index is shared across calls by design (contraction). Is an "index collision" error wanted, or is sharing intended?
6. `ScopedSymbolicDimension`: use `Session` as the plan says (silences error printing in scope), or a flag-only guard?
7. Keep `gammaAPI.cpp` under the `integration` label (current) or move the registration to `tests/api/CMakeLists.txt` with `ADD_API_TEST(gammaAPI)` (the plan's "API test source for parity")? Moving the file means changing the path in CMake only.
8. Null `Expr` handling: throw `ValueError` at every public entry point, or document that only the Fierz functions check it (today only `getIndexChainAssert` does, `gamma.cpp:199-201`)?

## 6. Not verified

- Nothing was compiled or run. Compile cleanliness with `-Wall -Wextra -Wpedantic` is unknown.
- Behaviour of null `Expr` in `simplified`, `ordered`, `project`, `generateString`, `generateLatex` (I expect a crash through `DeepCopy` or `operator<<`, not checked).
- Behaviour of negative indices (`getMinkoIndex(-1)` creates name `m-1`; `renameIndicesImpl` uses `std::atoi`, `gamma.cpp:495`): not run.
- The exact output of the shadowed message at `gamma.cpp:216-219` (read from the code only).
- Whether `gamma.cpp` compiles without `<cctype>` and `<algorithm>` (it uses `isdigit` and `std::any_of`, `gamma.cpp:455-457`, and includes only `<iostream>`, `<map>`, `<set>`).
- How `trace(C())` raises `sgl::MathError` (the origin inside SGL was not traced).
- The API to build a rational `csl::Expr` (num/den) for `from_rational`; `csl::api` has a `NumberKind::Rational` (`src/csl/api/expr.h:44`) but I did not read its constructors.
- Whether `GExpr::operator[]` / `argument(i)` bounds-check (not visible in the wrappers, `abstractgammasym.h:100-110`).
- Whether `generateLatex` has any test in `gammaAPI.cpp` (not checked with grep).
- The ctest names generated by `gtest_discover_tests` (assumed `marty_gamma_api.<case>`).
- Whether `namespace mty::api::diracology = mty::gamma_api;` fails to compile with GCC 12 (expected from the grammar; not compiled).
- Whether `src/marty/api/common/*.h` are included by any installed target (only `tests/api` includes them, as far as the grep showed; `CMakeLists.txt:121` install list not read in full).
- The `python/pymarty` package (only confirmed that it exists; not read for this topic).
