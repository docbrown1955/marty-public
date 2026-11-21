Status: READY
Updated: 2026-11-17
Author: subagent (investigator)

# Phase 1 investigation: global state and process termination

## Question

Which process-wide or global state can the CSL facade functions reach, and which code paths can terminate the process (`exit`, `abort`, `std::terminate`, `CSL_ASSERT` or other fatal paths, or a call to `Show()` that uses `system()`)?

Scope note: there is no CSL facade yet. `src/csl/api/` contains only `error.h`, and no `pow`, `Show`, or similar wrapper exists in `src/csl/api/` or `src/marty/api/`. The facade functions below are the CSL and MARTY core entry points that a facade would call.

## Facts

1. `CallErrorImpl` (both overloads) throws `CSLError`. It prints to `std::cerr` only when `csl::option::printErrors` is set, then throws. It never exits. Evidence: `src/csl/error.cpp:80-83` and `src/csl/error.cpp:91-94` (`throw error;` at :83 and :94).

2. `CSL_ASSERT`, `CSL_ASSERT_SPEC` and `CALL_SMERROR` route through `CallErrorImpl`, so they are catchable exceptions. Evidence: `src/csl/error.h:71-81`.

3. `pow_s` is the only fatal path found in ordinary CSL math. It raises `SIGSEGV`, which is a signal and not an exception, so no `try`/`catch` can intercept it. Evidence: `src/csl/operations.cpp:3107` (definition), `:3158-3161` (base is `oo`: `std::cerr` at :3159-3160, then `std::raise(SIGSEGV)` at :3161), `:3164-3170` (base is 0 and the numerical exponent is negative: `std::cerr` at :3168-3169, then `std::raise(SIGSEGV)` at :3170). The default action of `SIGSEGV` ends the process. Not run; see "Not verified".

4. The `pow_s` diagnostics are written to `std::cerr` unconditionally. They do not check `csl::option::printErrors`, so `mty::api::Session` (which sets `printErrors=false`) does not silence them. Evidence: `src/csl/operations.cpp:3159-3160`, `:3168-3169`; `src/csl/options.h:38-40` (the flag's comment says "CSL errors are not printed").

5. Division by zero in `fraction_s` is not fatal. It prints a warning unconditionally and returns `CSL_INF`. The `CSL_ASSERT_SPEC` that would throw is commented out. Evidence: `src/csl/operations.cpp:2519-2528` (warning at :2520-2522, the assert commented at :2523-2527, `return CSL_INF;` at :2528).

6. `throw 6;` (a bare `int`) sits in the "NOT FOUND IN" branch of `HardFactorImplementation`. An uncaught `int` would call `std::terminate`. The branch looks unreachable: the key is inserted by `posFactors[f]` at `src/csl/hardFactor.cpp:223`, and `linear_find` (`src/csl/hardFactor.cpp:171-184`) is re-run right after with the same `==`. Evidence: `src/csl/hardFactor.cpp:224-230`. The only `catch (int)` in `src/` is `src/csl/libraryevaluator.cpp:531`, which is not on this path.

7. `exit(123)` is in the `LibraryTensor` constructor. The header is included by `src/csl/csl.h:78` and `src/csl/librarygenerator.cpp:23`, and is copied into generated libraries (`src/csl/librarygenerator.cpp:791`). Its text is also emitted by `src/csl/librarytensor_hdata.h:86`. No CSL symbolic function constructs `LibraryTensor` (grep for `LibraryTensor` in `src/`: only `librarytensor.h`, `librarytensor_hdata.h`, and `librarygenerator.cpp`). So it is reachable from generated library code, not from the facade. Evidence: `src/csl/librarytensor.h:77-81`.

8. `exit(1)` in `scaleFactorA` is reached only for an id other than `aa0` or `aa00`. It is LoopTools-related and lives in MARTY core, not the CSL facade. Evidence: `src/marty/core/looptools_patch.h:128-136`. Callers were not checked.

9. The former `exit` sites in `src/marty/core/semiSimpleAlgebra.h` and `src/csl/librarygroup.cpp` are gone. A grep of `src/**` for `exit(`, `abort(`, `_Exit`, `quick_exit`, `std::terminate` finds no hit in either file. The remaining live hits are: `src/csl/librarytensor.h:81`, `src/marty/core/looptools_patch.h:136`, `src/marty-plugins/boltzmann-solver/libjsondata.cpp:52` (plugin), and `exit(1)` in both `src/grafed/jsonparser/jsonObject.h` and `src/grafed/gui/jsonObject.h` (lines 267, 283, 298, 314; GRAFED, outside the facade). There is no `abort`, `_Exit`, `quick_exit` or `std::terminate` anywhere in `src/`. Evidence: `tests/api/src/test_terminating_calls_api.cpp:16` (comment "Former exit() sites"), `docs/architecture/pymarty/spike-c-error-paths.md` (WP0.3 table).

10. The spike document is wrong on one point. It says `librarytensor.h` "is not included by any compiled unit". `src/csl/csl.h:78` includes it. Evidence: `docs/architecture/pymarty/spike-c-error-paths.md` (WP0.3 table, `librarytensor.h:81` row) versus `src/csl/csl.h:78`.

11. `Show()` is the only public path that calls `system()`. The `Show` overloads are in `src/marty/core/mrtInterface.cpp:244-331`. They reach `Drawer::launchViewer`, which writes a fixed `/tmp/graphs.json` (`src/marty/core/drawer.cpp:162`) and then runs `system("grafed-gui /tmp/graphs.json >/dev/null 2>&1 & sleep 0.5")` (`src/marty/core/drawer.cpp:163`). `enableApp` is `static constexpr bool enableApp = true;` (`src/marty/core/drawer.cpp:157`). The command has no interpolated user input. The path is fixed and predictable in a shared `/tmp`. `src/csl/` has no `Show(` call (grep).

12. `system()` in `src/csl/operations_monitor.h:18-34` is compiled only under `#ifdef MONITOR_OPERATIONS`. That macro is not defined in the repository (grep found only the header itself). The header also declares a global `inline const FileDeleter deleter;` at `:34`, whose constructor runs `system("rm sum_monitor.txt prod_monitor.txt")`. If the macro were defined, this would run at static initialisation.

13. The CSL option flags are process-wide mutable `inline bool` globals: `src/csl/options.h:28-42`. `errorStopsProgram` (`options.h:28`) has no reader and no setter in `src/` (grep). The X-macro `MTY_CSL_OPTION_FLAGS` at `src/csl/options.h:48` lists all flags, and `Session` snapshots them.

14. `mty::api::Session` snapshots and restores `Model::current` and all `csl::option` flags, sets `printErrors=false` during the call, and restores everything in its destructor. It is documented as single-threaded. Evidence: `src/marty/api/common/session.h:36-75`.

15. `Model::current` is a process-wide static pointer: `src/marty/core/model.h:66` `inline static Model *current = nullptr;`. It has no references in `src/csl/`. Readers are in MARTY core. The default arguments `*Model::current` in `src/marty/core/filters.h:154` (and 183, 260, 336, 365, 436) dereference a null pointer when no model is set. That is undefined behaviour; not run.

16. `Space` has a static flag and per-instance mutable state: `src/csl/space.h:61-64` `static bool applyMetric` (defined at `src/csl/space.cpp:50`); `src/csl/space.h:117` and `:119` per-instance mutable maps. Global `Space` objects are inline: `src/csl/space.h:438-479` (`DMinko`, `Minkowski`, `Euclid_R2`, `Euclid_R3`, `Euclid_R4`).

17. Other mutable process-wide state in CSL: `src/csl/abreviation.h:42` `inline std::map<std::string, std::vector<AbstractParent*>> abbreviationData;`; `src/csl/abreviation.h:57` `static std::map<std::string,size_t> id_name;`; `src/csl/comparison.cpp:26-31` `Comparator` static flags and maps `arbitrary` and `correspondance`; `src/csl/lock.h:161` `static inline std::map<ID_t,std::string> lockName;`; `src/csl/parent.cpp:95` `static std::string emptyName;`; `src/csl/abstract.cpp:35` `static string emptyString;`, `:474` `static Expr undef;`.

18. Shared constant expressions and allocators are inline globals: `src/csl/numerical.h:478-501` (`CSL_0`, `CSL_1`, `CSL_M_1`, `CSL_HALF`, and the allocators), `src/csl/typedef.h:58` `alloc_expr`. Thread safety was not checked.

19. MARTY API caches are static: `src/marty/api/gamma.cpp:30-31` (`diracIndices`, `minkoIndices`), `src/marty/sgl/tensorset.cpp:23` (static map), `src/marty/sgl/gammaindex.h:33` (`m_easyIndex`).

20. Stale generated documentation describes an older `exit`-based `callError`: `docs/doc/csl/html/error_8h_source.html` (around lines 182-185) shows `std::raise(SIGSEGV)` and `exit((int)error)` under `errorStopsProgram`. This does not match `src/csl/error.cpp`, which throws. The HTML is generated output and must not be edited.

21. The Python usage example specifies the intended behaviour: `0 ** -1` raises `MartyZeroDivisionError` and `oo ** 2` raises `MartyValueError`, and the process must not stop. Evidence: `python/examples/usage/symbolic.py:157-170`. No `pow` binding exists in `python/src/` (grep found no `pow`, `Show`, or `system(` match).

## Existing tests and data

- `tests/api/src/test_terminating_calls_api.cpp` covers the six dimension-mismatch throws in `semiSimpleAlgebra.h` and two `LibraryGroup` invalid-tag throws in `librarygroup.cpp`. It uses `mty::api::Session quiet`. It does not cover `pow_s` (either `SIGSEGV` case), `fraction_s`, `Show`, `Model::current`, or `LibraryTensor`. Registered at `tests/api/CMakeLists.txt:10`.
- `tests/api/src/test_error_api.cpp:129` (per an earlier read): checks that `CallErrorImpl` throws. Not re-read in this pass.
- `tests/api/src/test_session_api.cpp:118-175` (per an earlier read): checks that `Session` restores `Model::current` and the flags. Not re-read in this pass.
- No death tests were found: grep for `EXPECT_DEATH|ASSERT_DEATH|SIGSEGV` across `tests/` and `src/` finds none (`src/grafed/core/planargraph.h` raises `SIGSEGV`, GRAFED only).
- `tests/units/csl/test_latex.cpp:87-148` uses `pow_s` with exponents `-1` and `-2` on non-zero symbolic bases. No zero base or `oo` base is tested.
- `tests/units/csl/test_numbers.cpp:326-341` covers complex powers only.

## Options and trade-offs

1. Replace the two `std::raise(SIGSEGV)` calls in `pow_s` with a throw. This follows the pattern already used for the converted sites and keeps the CSL error model. Cost: needs an error kind for zero division and for `oo` (the Python spec asks for two kinds, `MartyZeroDivisionError` and `MartyValueError`). Opinion: preferred. Also gate the `std::cerr` lines on `printErrors`.
2. Guard in the future facade instead of `pow_s`. Cost: duplicated checks for each caller that reaches `pow_s` (many in `src/csl/operations.cpp`, `indicial.cpp`, `groupIndices.cpp`). Opinion: not preferred; keep the guard in one place.
3. `fraction_s` division by zero: keep returning `CSL_INF` with a warning (current, non-fatal), or throw. Cost of keeping: silent infinity in the API. Cost of throwing: changes numerical behaviour of existing callers. Opinion: decide with the user.
4. `csl::option::errorStopsProgram`: delete (it is inert, and the stale HTML describes an old behaviour), or wire it up. Cost of deleting: an API-visible flag disappears; there is no reader or setter in `src/`. Opinion: delete.
5. `LibraryTensor` `exit(123)`: convert to a throw. Cost: changes behaviour of generated libraries, which must then handle exceptions. Opinion: low priority for the facade; document it.
6. `Show()`: keep `system()` as the only system path, and make `/tmp/graphs.json` configurable or use a private temporary directory. Cost: a small API change. Opinion: low risk today because the command is fixed, but the shared `/tmp` path is predictable.
7. `Model::current` default arguments: keep, or check for null and throw. Cost of checking: a few lines in `filters.h`. Opinion: check for null, because the current default is undefined behaviour.

## Questions for the user

1. Should the `pow_s` zero-base and `oo`-base cases throw a `CSLError`? If so, do you want new kinds (zero division and value error), given the Python spec names `MartyZeroDivisionError` and `MartyValueError`?
2. Should `fraction_s` division by zero keep returning `CSL_INF` with a warning, or throw?
3. Should `csl::option::errorStopsProgram` be deleted (no reader or setter in `src/`)?
4. Should `LibraryTensor`'s `exit(123)` in generated libraries be converted to a throw, or left as is?
5. Should `Show()` use a configurable or private temporary path instead of `/tmp/graphs.json`?
6. The Doxygen HTML under `docs/doc/` is stale. Should it be regenerated by you (it is generated content, so I did not touch it)?

## Not verified

- Runtime behaviour of `std::raise(SIGSEGV)` (exit status, core dump): no build or run was done.
- Reachability of `throw 6` (`src/csl/hardFactor.cpp:230`): depends on whether `linear_map::operator[]` uses the same equality as `linear_find`. Not read.
- Whether any caller guards base 0 or `oo` before `pow_s`. No facade exists yet (grep in `src/csl/api/` and `src/marty/api/` for `pow`/`Show`: no match). The `pow_s` caller list in `src/csl` was truncated at 40 matches and was not enumerated in full.
- Whether `CSLError` (`src/csl/error.h:31-80`) already has a zero-division kind. The enum members were not re-read in this pass.
- `git log --all` and branches `api/gamma` and `pymarty` were not searched.
- Whether `Session` is the only route that sets `printErrors`. Not searched.
- Whether `LoopTools` callers reach `scaleFactorA` with an invalid id (`src/marty/core/looptools_patch.h:136`).
- `Show(` calls under `src/csl/`: grep result only.
- `MONITOR_OPERATIONS` is not defined anywhere in the repository: grep across the repository found only the header. Build files were not inspected line by line.
- The stale generated HTML line numbers (`error_8h_source.html`, around 182-185) are from an earlier read.
- Thread-safety of the process-wide statics and allocators (`numerical.h`, `typedef.h`): not checked.
