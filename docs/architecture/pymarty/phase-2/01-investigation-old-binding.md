Status: READY
Updated: 2026-11-20
Author: investigator

# Investigation: the old gamma pybind11 binding (branch `origin/api/gamma`)

Citations `origin/api/gamma:<path>:<line>` refer to the branch tip (`39f288f`, 557 commits on the branch).
Citations without a prefix refer to the working tree. Nothing was checked out, built, or tested.

## 1. Question

What did the old pybind11 binding for the gamma API contain (source, Python package, tests, notebook),
and how does it map onto the phase 2 plan (`../phase-2-diracology.md`, sections 3 and 4)? In particular:
the binding's direct `sgl::`/`csl::` calls (acceptance criterion 4), the `sgl_to_csl` equality code, and
old test assertions that the plan does not mention.

## 2. Facts

### 2.1 History and layout

1. The binding is one file, `src/pybinding/gamma_api.cpp` (260 lines), plus `src/pybinding/CMakeLists.txt`
   (builds `_pymarty` from `gamma_api.cpp` only). Created in `e46cce1` (2022-12-26, "Stub files for gamma_api"),
   then `0c2c534`, `e8ce87b`; last changed in `8f58f3d` ("Simple fierz methods for gamma API", +95/-30 vs `e8ce87b`).
   Evidence: `git --no-pager log origin/api/gamma --oneline -- src/pybinding/gamma_api.cpp`;
   `git --no-pager diff --stat e8ce87b origin/api/gamma -- src/pybinding/gamma_api.cpp`.
2. Commit `39f288f` ("All outputs in ntoebook") touches only `python/notebooks/sample_gamma_api.ipynb`
   (+54/-23). Evidence: `git --no-pager show --stat 39f288f`. Nothing else was changed by it.
3. The binding module is `_pymarty` with submodule `gamma_api` (`origin/api/gamma:src/pybinding/gamma_api.cpp:13-16`).
   The Python package is `python/src/pymarty/` with `gamma_api/__init__.py`, `gamma_api/__init__.pyi`,
   `gamma_api/print_utils.py`, and `pymarty/__init__.py:1` (`from pymarty._pymarty import gamma_api`).
   Current tree: `python/src/pymarty/` does not exist; the current package is `python/pymarty/` (wt).

### 2.2 Binding source (`origin/api/gamma:src/pybinding/gamma_api.cpp`)

4. `Expr` is `mty::gamma_api::Expr` (= `sgl::GExpr`, `src/marty/api/gamma.h:9,14`). Constructors: `py::init<int>` and
   `py::init<float>` only (lines 18-19). No `__hash__` is defined, and `__eq__` is defined, so pybind11 sets
   `__hash__ = None`: `build_user/_deps/pybind11-src/include/pybind11/pybind11.h:1467-1468` (vendored pybind11 2.11.2,
   `detail/common.h:12-14`). The old `Expr` is therefore unhashable.
5. Methods on `Expr`: `__str__` (generateString, l.20-23), `__copy__` (`expr->copy()`, l.24-25), `__deepcopy__`
   (`sgl::DeepCopy`, l.26-29), `to_latex` (l.30-33), `simplify` (l.34-37), `order` (l.38-41), `project(basis)`
   (l.42-48), `__len__` (`expr->size()`, l.49-50), `__getitem__` (l.51-60: explicit `std::out_of_range`, which
   pybind11 maps to `IndexError`, `detail/internals.h:394-396`; negative indices also fail because of the unsigned cast).
6. Comparisons: `__eq__`, `__ne__` for `Expr`, `int`, `double`; `__lt__`, `__gt__`, `__le__`, `__ge__` for `Expr` only
   (l.61-116). Each converts both sides with `sgl::sgl_to_csl(x, mty::defaultSGLTensorSet())` and compares
   `csl::Expr` values; for int/double the right side is `csl::Expr(right)`. Example: l.61-66.
7. Arithmetic (l.117-137): unary `-py::self`; `+ - * /` between `Expr`s (`py::self`), with `int`, and with
   `float`; reflected `int() op py::self` and `float() op py::self` for `+ - * /`. Division `/` is therefore present
   (l.121, 125, 129, 133, 137); the plan's section 1 list ("`+ - *`, unary minus, scalar coefficients") does not name it.
8. `FierzBasis` is a `py::enum_` with values `Standard`, `Chiral` and `.export_values()` (l.138-141). Because of
   `export_values()`, `Standard` and `Chiral` are also module-level names in `gamma_api`.
9. Module functions (l.142-199): `D` and `imaginary_unit` are module attributes set to `DMinko()` and
   `imaginaryUnit()` (l.142-143). `g(mu, nu)` (l.144-148), `eps(mu, nu, rho, sigma)` (l.149-155), `gamma(mu)` and
   `gamma(mu, nu)` as two overloads (l.156-166), `gamma5()`, `P_L()`, `P_R()`, `C()` (l.167-178),
   `current(gammas, left_index, right_index)` (l.180-189), `set_symbolic_dimension(keep_symbolic)` (l.190-193),
   `trace(gammas)` (l.194-199). Argument names are `mu`, `nu`, `rho`, `sigma`, `gammas`, `left_index`, `right_index`,
   `keep_symbolic`, `basis`.
10. `single_fierz` and `double_fierz` each have two overloads. The `Expr` overload (l.201-228) takes
    `left_current`, `right_current`, `basis=Chiral`, and returns a **dict** with keys `"rhs"` and `"lhs"`, where
    `lhs = sgl::DeepCopy(left) * sgl::DeepCopy(right)` (l.207-209 and l.220-222).
11. The `std::vector<Expr>` overloads (l.230-259, marked "important for overload resolution") use
    `.noconvert()` on both current arguments and build the currents with **hard-coded indices**:
    `current(left, 1, 2)` and `current(right, 3, 4)` (l.235-236 for single, l.250-251 for double). They also return
    a dict with `"rhs"` and `"lhs"` (l.237-239, l.252-254). The plan does not mention the list form, the dict, or the
    fixed indices.
12. Default basis for both Fierz functions is `FierzBasis::Chiral` (l.214, 227, 244, 259).

### 2.3 Direct `sgl::` / `csl::` calls in the binding (acceptance criterion 4)

Command: `git --no-pager show origin/api/gamma:src/pybinding/gamma_api.cpp | grep -n "sgl::\|csl::"`
gives 25 lines. Every one of them is listed here; none is in the C++ API:

| Lines | Call | Purpose |
|-------|------|---------|
| 28 | `sgl::DeepCopy(expr)` | `__deepcopy__` |
| 64, 65 | `sgl::sgl_to_csl(left/right, defaultSGLTensorSet())` | `__eq__(Expr, Expr)` |
| 69, 70 | `sgl::sgl_to_csl(left, ...)`, `csl::Expr(right)` | `__eq__(Expr, int)` |
| 74, 75 | `sgl::sgl_to_csl(left, ...)`, `csl::Expr(right)` | `__eq__(Expr, double)` |
| 80, 81 | `sgl::sgl_to_csl(left/right, ...)` | `__ne__(Expr, Expr)` |
| 85, 86 | `sgl::sgl_to_csl(left, ...)`, `csl::Expr(right)` | `__ne__(Expr, int)` |
| 90, 91 | `sgl::sgl_to_csl(left, ...)`, `csl::Expr(right)` | `__ne__(Expr, double)` |
| 96, 97 | `sgl::sgl_to_csl(left/right, ...)` | `__lt__` |
| 102, 103 | `sgl::sgl_to_csl(left/right, ...)` | `__gt__` |
| 108, 109 | `sgl::sgl_to_csl(left/right, ...)` | `__le__` |
| 114, 115 | `sgl::sgl_to_csl(left/right, ...)` | `__ge__` |
| 209 | `sgl::DeepCopy(left) * sgl::DeepCopy(right)` | `single_fierz` (Expr overload) |
| 222 | `sgl::DeepCopy(left) * sgl::DeepCopy(right)` | `double_fierz` (Expr overload) |
| 239 | `sgl::DeepCopy(l) * sgl::DeepCopy(r)` | `single_fierz` (list overload) |
| 254 | `sgl::DeepCopy(l) * sgl::DeepCopy(r)` | `double_fierz` (list overload) |

Further direct SGL-object and marty-namespace uses (no `sgl::` prefix): `expr->copy()` (l.25), `expr->size()`
(l.50, 53), `expr->argument(index)` (l.59), and `mty::defaultSGLTensorSet()` on every comparison line
(l.64-115; declared at `src/marty/core/diracology.h:144`). Includes at l.1-8 bring in `csl/options.h`,
`marty/api/gamma.h`, `marty/core/diracology.h`, `marty/core/mrtInterface.h`, `marty/sgl/sgl.h`.

Overall: 25 lines violate criterion 4 as written. The arithmetic operators (`py::self`) call GExpr operators
without naming `sgl::`; they are not counted above.

### 2.4 The `sgl_to_csl` equality code

13. The conversion is `sgl::sgl_to_csl(GExpr const&, TensorSet const&)` declared at
    `src/marty/sgl/cslconverter.h:33`. Equality and ordering are then `csl::Expr` operators on the converted values
    (l.64-115 above). So equality is "equal after conversion to CSL", not an SGL-level canonical comparison.
14. The old tests simplify before `==` in some places and not in others: `eps(0,1,2,3) == (-eps(0,2,1,3)).simplify()`
    (`origin/api/gamma:python/tests/test_simple_api.py:25`), while `eps(0,0,1,3) == 0` (l.27) is compared without
    `simplify()`. Whether the un-simplified comparison holds was not checked (no run).
15. The plan (section 2 "Comparison") moves this code into the API as `equals(a, b)` and `less(a, b)`. That matches
    the code above. The C++ facade in the working tree has no `equals`/`less` (`src/marty/api/gamma.h:16-49`
    lists only the existing functions).

### 2.5 Python package (`origin/api/gamma:python/src/pymarty/gamma_api/`)

16. `__init__.py:1-18` re-exports from `_pymarty.gamma_api`: `Expr`, `set_symbolic_dimension`, `g`, `eps`,
    `gamma`, `gamma5`, `P_L`, `P_R`, `C`, `D`, `imaginary_unit`, `current`, `single_fierz`, `double_fierz`,
    `trace`, `FierzBasis`. `print_latex` comes from `print_utils.py`. `__all__` is at l.21-39.
17. `print_utils.py:3` imports `IPython.display` at module import, so the old package requires IPython.
    `print_latex` (l.33-52) accepts one `Expr` or dict (the dict from `single_fierz` is expanded as `lhs`/`rhs`),
    two arguments, or `lhs=`/`rhs=`. Its error branch (l.49-52) concatenates a string with `help(print_latex)`,
    which returns `None`, so it raises `TypeError` instead of the intended `ValueError`. Not fixed by the old code.
18. The stub `__init__.pyi` disagrees with the binding in three places: `FierzBasis(enum.IntEnum)` (l.6; pybind11
    enums are not `IntEnum`), `Expr.__getattr__(self, i: int)` (l.17; the binding has `__getitem__`, gamma_api.cpp:51),
    and `Expr.__init__(expr: Union[Expr, int, float])` (l.11; the binding has int and float constructors only,
    gamma_api.cpp:18-19). The stub's     `__all__` (l.69-84) omits `D`, `imaginary_unit` and `print_latex`. The comparison and arithmetic
    operators are typed with `AnyExpr` (l.4, l.21-35).

### 2.6 Python tests (`origin/api/gamma:python/tests/`)

19. Files: `test_import.py`, `test_objects.py`, `test_simple_api.py`, `test_simplifications.py`, and
    `conftest.py.in` (adds `@PYMARTY_PATH@` to `sys.path`, l.3-5). Nothing else is in `python/tests/` on the branch
    (`git --no-pager ls-tree -r --name-only origin/api/gamma | grep python/tests`).
20. Test count: **9 test functions** (`def test_` grep): `test_import.py` 3, `test_objects.py` 2,
    `test_simple_api.py` 2, `test_simplifications.py` 2. The per-file lists are in section 3 below.
21. Old tests import from `pymarty.gamma_api` (e.g. `test_objects.py:5,39`). The new layout is `pymarty.diracology`;
    the plan's section 4 says "port ... adapted to the new layout". The old names used in the tests are
    `imaginary_unit` (`test_simplifications.py:9`), `FierzBasis.Chiral`/`Standard`, and keyword arguments
    `left_current=`, `right_current=`, `basis=` (`test_simplifications.py:52-56`, `:65-68`).

### 2.7 Notebook (`origin/api/gamma:python/notebooks/sample_gamma_api.ipynb`)

22. The notebook has 10 code cells with outputs and no errors (`execution_count` 6 to 10). Each output is a
    `text/latex` display produced by `print_latex`; `text/plain` is `<IPython.core.display.Latex object>`.
    Command: `git --no-pager show origin/api/gamma:<nb> | python3 -c` (JSON dump of cells).
23. Calls used by the notebook: `set_symbolic_dimension(False)` (cell 0, positional), `trace([...])` (cell 2),
    `g(0,1) * trace_result` and `.simplify()` (cell 3), `current([...], 0, 1) + current([...], 0, 1)` and `.order()`
    (cell 5), `.project(FierzBasis.Chiral)` (cell 6), `single_fierz([P_L()], [P_L()], basis=FierzBasis.Chiral)` (cell 8),
    `single_fierz([P_L()], [P_R()])` and `res["lhs"]`/`res["rhs"]` (cell 9), `double_fierz` with list arguments
    (cells 11-13).
24. The notebook runs a subset of the same calls, not the same set. It does not use `__len__`, `__getitem__`,
    `copy`, `deepcopy`, `to_latex` directly, comparisons, or `g`/`eps` contraction checks. It uses `set_symbolic_dimension`
    positionally, once. The notebook exercises the **list-form** Fierz overloads with hard-coded indices (cells 8-13), which the
    plan does not mention. Cells 2, 3, 5, 6 use `print_latex`, which the plan does not mention either.
    The notebook's latex outputs are stored (10 `text/latex` entries). They wrap `to_latex()` output in an equation
    environment, so they are not raw `to_latex` strings.

### 2.8 Plan coverage of the old tests (section 3 mapping)

25. Plan section 3 maps the `gamma.h` names, `D`/`I`, `current`/`trace`, `simplify`/`order`/`project`, Fierz,
    `keepSymbolic4D`, `to_latex`. Differences against the old names:
    - `D`, `I` (plan) vs `D`, `imaginary_unit` (old, gamma_api.cpp:142-143).
    - `FierzBasis.STANDARD`, `FierzBasis.CHIRAL` (plan) vs `FierzBasis.Standard`, `FierzBasis.Chiral` (old, l.139-140).
    - `single_fierz(c1, c2, basis=...)` (plan) vs old dict return `{"rhs", "lhs"}` and list overloads (l.201-259).
    - The plan has no `/` and no `print_latex`. The keyword `keep_symbolic=` matches the old binding (gamma_api.cpp:193).
26. Old assertions that the plan does not mention (or mentions only generically). `file:line` is in
    `origin/api/gamma:python/tests/`:
    - `test_simple_api.py:5-11`: `D.simplify() == D` when symbolic; `Expr(4) != D.simplify()`; reverse after
      `set_symbolic_dimension(False)`. Plan §4 names only scoped-switch cases.
    - `test_simple_api.py:19-20`: `g(0,1) == g(1,0)` (symmetry). Not in plan.
    - `test_simple_api.py:22`: `g(0,1) * g(1,2) == g(0,2)` (contraction). Not in plan.
    - `test_simple_api.py:25`: `eps(0,1,2,3) == (-eps(0,2,1,3)).simplify()` (antisymmetry). Not in plan.
    - `test_simple_api.py:26`: `eps(0,1,2,3) != eps(1,0,2,3)`. Not in plan.
    - `test_simple_api.py:36-41`: equality and inequality of currents `c1`, `c2`, `c3`. Not in plan.
    - `test_simplifications.py:14-15`: `Tr(g0 g1 P_L) = 2 g01` and `Tr(g0 g1 P_R) = 2 g01`. Not in plan.
    - `test_simplifications.py:16`: `Tr(gamma5) = 0`. Not in plan.
    - `test_simplifications.py:17-19`: `Tr(g0 g1 g2 g3 gamma5) = -4 i eps(0,1,2,3)`. Not in plan; the plan only has
      the sign convention for the eps contraction.
    - `test_simplifications.py:33`: `gamma(0) gamma(0)` with indices (0,1) simplifies to `4 * current([],0,1)`. Plan has
      `g(mu,mu)=D` but not `gamma(mu) gamma(mu) = D`.
    - `test_simplifications.py:34`: `C C = -1`. Not in plan.
    - `test_simplifications.py:35-36`: `P_L gamma5 = -P_L` and `P_R gamma5 = P_R` (after simplify). Not in plan
      (plan has `gamma5^2 = 1`, `P_L + P_R = 1` only).
    - `test_simplifications.py:37`: `P_R P_L` chain simplifies to `0`. Plan has `P_L P_R = 0` as an identity;
      this chain form is not mentioned.
    - `test_simplifications.py:38-43`: `g0 g1 g0 = -2 g1` on indices (2,3), and the assertion `!=` with indices
      swapped (order sensitivity). Not in plan.
    - `test_simplifications.py:44-46`: `current([gamma5, gamma0]).order() == -current([gamma0, gamma5])`. Not in plan
      as a value check.
    - `test_simplifications.py:47-49`: `project(Chiral)` of `gamma(0,1) gamma5` equals `current(g01 P_R) - current(g01 P_L)`.
      Not in plan as a value check.
    - `test_simplifications.py:65-71`: `single_fierz` with currents, `["rhs"] == Expr(1)/2 * ...` (with the 1/2
      factor). Plan has only "apply twice returns the original".
    - `test_simplifications.py:72-76`: `double_fierz` (Expr overload) `["rhs"] == 4 * current(...) * current(...)`. Plan says
      "apply twice returns original"; the 4-factor value is not mentioned.
    - `test_simplifications.py:78-82`: `double_fierz` list overload `["rhs"] == 4 * current([g0,P_L],1,2) * current([g0,P_L],3,4)`.
      Hard-coded indices 1..4; not in plan.
    - `test_simplifications.py:83-89`: `single_fierz` list overload `["rhs"] == Expr(1)/2 * current(...,1,4) * current(...,3,2)`.
      Hard-coded indices; not in plan.
    - `test_objects.py:45-50`: `Expr(0)`, `Expr(1.5)`, `D` are pairwise ordered (`<`, `<=`, `!=`), and reflexive
      (`_test_same`, l.10-16). Plan lists "ordering" in scope but has no case for scalar vs symbol ordering.
    - `test_objects.py:33`: `expr[-1]` raises `IndexError` (negative index). Plan §4 has only out-of-range.
    - `test_objects.py:4-7`: `FierzBasis.Chiral != FierzBasis.Standard`. Plan has the enum but no case.
    - Division `Expr(1)/2` used at `test_simplifications.py:59-61`, `:69`, `:87` (not an assertion but the value
      under test). `/` is not in the plan's arithmetic list.

## 3. Existing tests and data (old Python tests, branch `origin/api/gamma`)

| File | Test functions (line) | Assert lines (grep `^\s*assert`) | What they cover |
|------|-----------------------|----------------------------------|-----------------|
| `test_import.py` | `test_import_basics` (1), `test_import_objects` (5), `test_import_functions` (9) | 0 | Import names: `Expr`, `FierzBasis`, `set_symbolic_dimension`, `g`, `eps`, `gamma`, `P_L`, `P_R`, `gamma5`, `C`, `D`, `current`, `trace`, `single_fierz`, `double_fierz`, `print_latex` |
| `test_objects.py` | `test_basis` (4), `test_simple_expr` (38) with helpers `_test_same` (10), `_test_diff` (19), `_test_zero_len` (28) | 14 | `FierzBasis` inequality; reflexive/ordering on `Expr(0)`, `Expr(1.5)`, `D`; `len` and `IndexError` |
| `test_simple_api.py` | `test_set_symbolic_dimension` (1), `test_basic_expressions` (14) | 23 | Symbolic-dimension flag and `simplify`; `g`, `eps` symmetry/antisymmetry/contraction, current equality |
| `test_simplifications.py` | `test_traces` (1), `test_simplifications` (22) | 18 | Traces with `P_L`, `P_R`, `gamma5`, `eps`; `simplify`, `order`, `project`; `single_fierz`/`double_fierz` (Expr and list overloads) |

Coverage gaps in the old tests, relevant to the plan: no test of `set_symbolic_dimension` through the context
manager; no test for `copy`/`deepcopy`; no test of `*` with a float; no test of error types except `IndexError`;
no `to_latex` assertion (the notebook is the only LaTeX check); no test for `__hash__`.

## 4. Options and trade-offs

- **Fierz return value.** Keep the dict `{"rhs", "lhs"}` (old tests, notebook and `print_latex` depend on it;
  cheap, no rework) or return a small typed object (clearer, but breaks the old tests unless they are ported). Opinion:
  keep the dict for v1 (it is what the tests and notebook use), and document it in the stub as a `TypedDict`.
- **List overloads with fixed indices 1..4.** Keeping them preserves the old tests and notebook behaviour but hard-codes
  index values inside a binding; the plan's "no algebra in the binding" rule is better met by moving them into the API
  as a helper that takes indices. Opinion: keep the behaviour, but move the index assignment into the API (C++ addition).
- **Equality.** Old code: `sgl_to_csl` on both sides, unhashable. Plan keeps this (risk section). Cost: each comparison
  converts two expressions. Opinion: keep, since it matches the old tests exactly.

## 5. Questions for the user

1. Keep the dict return `{"rhs", "lhs"}` of `single_fierz`/`double_fierz` (old behaviour), or change it? (Tests and
   notebook use `["rhs"]` and `["lhs"]`.)
2. Keep the list overloads of `single_fierz`/`double_fierz` with fixed indices 1..4 (old tests `test_simplifications.py:78-89`,
   notebook cells 11-13)? If yes, the plan's section 3 needs a row for them.
3. Naming: plan has `FierzBasis.STANDARD`/`CHIRAL` and `I`; old code has `Standard`/`Chiral` and `imaginary_unit`
   (old tests use these). Which names are final? (Plan §3 vs old tests.)
4. Add `/` (division by a scalar) to the plan's arithmetic list? The old tests use it (`test_simplifications.py:59-61`).
5. `print_latex`: keep it in `pymarty.diracology` (needs IPython at import, `print_utils.py:3`), or move it to a notebook helper?
   The plan does not mention it, but the old tests import it (`test_import.py:15`).
6. Should the old assertions in section 2.8 be ported (all of them), or are some dropped? The orchestrator asked to decide.

## 6. Not verified

- The old binding was not built and the old Python tests were not run (per the standing rules, and no checkout).
- Whether `eps(0,0,1,3) == 0` holds without `simplify()` (`test_simple_api.py:27`), and whether `csl::Expr` ordering
  gives the values asserted in `test_objects.py:45-50`.
- Whether the notebook's stored latex outputs match current output (not compared).
- The constructors `Expr(int)`/`Expr(float)` are not traced into `sgl::GExpr`.
- Behaviour of `expr[-1]` (unsigned cast at gamma_api.cpp:53) was read, not run.
- The full history of `src/pybinding/gamma_api.cpp` between `e8ce87b` and `8f58f3d` was read only as a stat, not diff by diff.
- The integration test `tests/integration/src/gammaAPI.cpp` case count was not checked (outside this topic).
- The current `tests/api/` tree was only listed, not read.

## Searches run

- `git --no-pager log --all --oneline -- src/pybinding/gamma_api.cpp python/` and per-path `git log --all --format` (dates).
- `git --no-pager branch -a --contains <commit>` for `e46cce1`, `0c2c534`, `e8ce87b`: all only `remotes/origin/api/gamma`.
- `git --no-pager ls-tree -r --name-only origin/api/gamma` filtered for `python|pybinding|gamma`.
- `git --no-pager show origin/api/gamma:<path>` for the binding, the four test files, the package files, the notebook.
- `grep -n "sgl::\|csl::"` on the binding (25 lines, listed in 2.3).
- Working tree: `grep` for `sgl_to_csl`, `defaultSGLTensorSet`, `operator`, `size()`/`argument(` in `src/marty/sgl` and
  `src/marty/api/gamma.h`; `ls src/pybinding` (absent in the working tree); `git ls-tree HEAD` for `python/` and `src/pybinding`.
- pybind11 headers read under `build_user/_deps/` (not built, not modified).
