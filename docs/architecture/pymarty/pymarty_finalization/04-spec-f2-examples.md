Status: DRAFT (not frozen; revised after the F1 gate)
Updated: 2026-11-28
Author: main agent (orchestrator)
Depends on: F1 (paths in `usage_examples/`)

# F2 spec: usage examples

Scope: rewrite and consolidate the usage examples, make every example run in CI (C++ and Python), and
turn them into notebooks as decided in Q5. Source: [USER_FEEDBACK.md](../USER_FEEDBACK.md) "Usage examples",
[01-inventory.md](./01-inventory.md) section B, phase 8 WP8.2 and acceptance criterion 11.

## 1. Decisions

- **Q5 (answered by the user):** all examples become notebooks. This overrides the recommendation (a) in
  [02-questions.md](./02-questions.md). The README sentence "notebooks where LaTeX or diagrams help"
  ([README.md](./README.md), F2 row) must be updated to match.
- **Example list (to decide at G1 of F2):**
  - (a) the seven module examples of phase 8 criterion 11 (`errors`, `symbolic`, `group_theory`, `diracology`,
    `modeling`, `calculation`, `simulation`), each a notebook;
  - (b) the five examples proposed by USER_FEEDBACK (symbolic, diracology, group_theory, gettingStarted, complex SM).
  The two lists conflict with phase 8 criterion 11, which must be amended if (b) is chosen.

## 2. Current state (verified 2026-11-28, see inventory section B)

- Python: 7 scripts in `python/examples/usage/` run by `python/tests/test_usage_examples.py:16,34,43`.
- Python: one notebook, `python/notebooks/gthdm_investigation.ipynb`, run by no test.
- C++: 8 programs in `examples/`. `examples/CMakeLists.txt` has no `add_test`, so no test runs them.
- No `nbclient`, `nbexec` or notebook step in CI (`.github/workflows/python.yml`, `c-cpp.yml`).

## 3. Steps

1. Inventory the C++ examples: for each file, record what it computes and whether a Python example covers it.
   Evidence goes in `04-implementation-log.md` (created in F2).
2. Rewrite the Python examples as notebooks under `usage_examples/python/`. Each notebook prints results and
   shows LaTeX (`_repr_latex_`, already present in `python/src/symbolic.cpp:535`). Assertions are kept only where
   a value is checked against a reference. Remove the old scripts once the notebook replaces them.
3. Keep one C++ example per module that a user would copy, under `usage_examples/cpp/`. Each gets an `add_test`
   (or a CTest entry under the `api` label) so it is built and run in CI.
4. Add notebook execution to CI: a job that runs `usage_examples/python/*.ipynb` with `nbclient` (or `nbconvert
   --execute`), with the `example` marker or a `-m "not slow"` filter, following the rule that the system suite
   is never run in CI (AGENTS.md "Working rules").
5. Remove the old `test_usage_examples.py` checks that duplicate what the notebooks now run; keep a test that
   every notebook has a passing execution.
6. Update the paths in `AGENTS.md` and `docs/architecture/pymarty/*.md` that mention the examples.

## 4. Verification

- `ctest -N -L api` before and after to show the new C++ example tests are listed, and `ctest -R <example>`
  for each one (never an unfiltered `ctest`).
- Notebook execution: each notebook runs top to bottom with no error, in a clean kernel.
- `pytest --collect-only -m "not slow"` on `tests/python` before and after.
- Static checks: `ruff check` and `ruff format --check` on any Python code the notebooks import.

## 5. Acceptance

- Every example (C++ and Python) is run in CI and passes.
- No example depends on an output that changed without review (outputs are cleared in the notebooks, or
  compared when deterministic).
- Example list matches the decision in section 1, and phase 8 criterion 11 is amended if it differs.

## 6. Out of scope

- API changes (F3). Any example that needs a missing API is marked as a gap and listed in F6, not patched here.
- The `gthdm_investigation.ipynb` physics content is kept as it is, only moved and executed.

## Open points

- Whether notebook outputs are stored in git or cleared (decide at G1 of F2).
- Runtime budget of the notebook CI job (measure after step 2).

## Amendments

(none)
