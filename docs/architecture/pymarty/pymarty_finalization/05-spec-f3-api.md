Status: DRAFT (not frozen; revised after the F2 gate)
Updated: 2026-11-28
Author: main agent (orchestrator)
Depends on: F1, F2

# F3 spec: API polish

Scope: make the public Python API consistent, typed and documented before any gap is filled (F6). This
step changes public names and structure, so it is reviewed as an API surface, not as a move.
Source: [USER_FEEDBACK.md](../USER_FEEDBACK.md) "API design", [01-inventory.md](./01-inventory.md) section C,
phase 8 WP8.1 and acceptance criteria 3 and 4.

## 1. Decisions

To be recorded in [02-questions.md](./02-questions.md) before G1 of F3:

- **Namespace classes:** remove the five classes `group_theory`, `modeling`, `calculation`, `simulation`,
  `diracology` declared in `python/pymarty/_pymarty.pyi:362,435,703,812,846` (now `src/pymarty/...` after F1).
  Replace them with modules (one shape for every module, as set in the USER_FEEDBACK layout).
- **Module shapes:** three modules are single files and five are packages (inventory section A). Choose one
  shape. The recommended choice is a package with `__init__.py` for every module. This changes import paths,
  so the decision needs a deprecation path (F5 policy).
- **Naming and errors:** keep `infinity` (C++ `oo`) and `replace` (C++ `subs`) as in AGENTS.md. Check every public
  error type in `python/pymarty/errors.py` against one convention.
- **Visualisation (answer Q6d):** the API states clearly which functions need GRAFED, Qt 5 or LaTeX tools, and
  which work without them. `visualization.latex()` needs no tool; `render_png` needs latex and dvipng;
  `show` and `grafed` need `grafed-gui`. The message and docstring of each case are checked against this list.

## 2. Steps

1. Measure the baseline: `mypy --strict` error count on the package and on the examples (phase 7 says 74,
   not re-verified). Record it in `05-implementation-log.md`.
2. Remove the namespace classes and their stub entries. Update every caller (tests, notebooks, examples) to the
   module form. Run the stub sync rule from AGENTS.md: `_pymarty.pyi`, `symbolic.pyi` and `symbolic.py` must agree.
3. Apply the module-shape decision to the eight modules that differ.
4. Duplication audit: list each Python builder that duplicates a C++ model (phase 4 log, line 120), and the
   thin layer rule from `python-porting-rules`. Remove duplicates that are not needed for the port.
5. Typing: bring `mypy --strict` to zero errors on the package, the tests and the tools. No `Any` in public
   signatures except documented duck-typed protocols (phase 8 criterion 3).
6. Output conventions: `__repr__`, `_repr_latex_` and printed CSL text (`INF` per AGENTS.md) follow one rule.
7. Visualisation messages: align them with section 1, so a missing tool is reported with the feature name and
   the install hint, and no import fails because a tool is absent.

## 3. Verification

- `mypy --strict pymarty tests tools` at zero errors (or the baseline, if the decision is to accept it, recorded
  as an amendment).
- `ruff check .` and `ruff format --check .` in `python/` paths (updated after F1).
- Full Python fast suite: `pytest -m "not slow"` from the build directory, with the count compared to F1.
- `python/tools/check_parity.py` (now `tests/python/tools/`) from the repo root.
- Docstring examples executed by `test_docstrings.py`.
- Tests for the visualisation messages: with `grafed-gui` and LaTeX absent, `import pymarty` and `latex()` still work.

## 4. Acceptance

- No namespace class remains in any stub or public module.
- `mypy --strict` is clean (criterion 3).
- The optional-dependency behaviour (Q6d) is stated in the docstring of each visualisation function and tested.
- A short API surface table (module, public names, optional tools) is written in `05-api-surface.md` for F4 to use.

## 5. Out of scope

- New features and ports (F6). Wilson API refactor (F6, Q7).
- Packaging, `doctor` and the wheel (F5).
- Documentation pages (F4), except the docstrings themselves.

## Open points

- Whether the module-shape change needs a compatibility shim for `pymarty.modeling.models` (D1 keeps this path,
  so no shim is expected).
- The duplication audit may find C++ models that F1 moved (D2); decide case by case.

## Amendments

(none)
