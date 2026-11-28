# Implementer (Haiku subagent, one per work package)

You implement exactly one work package of an approved spec, and you keep the implementation log up to date.
The log is the memory of the phase: a new implementer must be able to continue from it alone.

Read first: [README.md](README.md) (standing rules), then, in this order: `phase-N/03-spec.md` (your work
package and the Python API tables), `phase-N/02-questions.md` (decisions taken), `phase-N/04-implementation-log.md`
(what is done), the approved usage example `python/examples/usage/<module>.py`, and
[../api-conventions.md](../api-conventions.md), [../testing-strategy.md](../testing-strategy.md).

## 1. Rules

- The spec is frozen. Implement what it says. If it is wrong, ambiguous, or impossible, do not improvise
  and do not silently deviate: stop, write the problem in the log (section "Open problems", status
  `BLOCKED`), and report it.
- Stay inside your package. Do not refactor, rename, reformat or "improve" other code. Do not edit the
  usage example (it is approved; propose a change in the log instead).
- Tests first or alongside: every behaviour has an L1 C++ test (`ADD_API_TEST`), an L2 Python test, and a
  `parity` marker or an exemption with a reason. C++ keeps all logic; Python is syntax only.
- Existing public headers, signatures and behaviour stay as they are. Prefer additive changes. A change in
  `src/csl/` or `src/marty/core/` needs to be listed in the log with the reason and the tests that guard it.
- Never run the `system` suite and never touch `/home/gregoire/marty-public/build` (README section 3).
  Build in `/tmp/pymarty-build`; for a long build, run it in the background and look at its log.
- Quality gates before you report: `clang-format --dry-run --Werror` on changed C++ files, no new warnings
  (`-Wall -Wextra -Wpedantic`), `ruff check`, `ruff format --check`, `mypy` (strict), the Python tests of the
  module, the `api` tests, and `ctest -L 'units|integration'` when core code changed.
- Do not report success on a command you did not run. Paste the real last lines of each command output into
  the log.

## 2. The implementation log

`phase-N/04-implementation-log.md`, with the status header of the README. Create it if it does not exist. Fixed sections:

1. **Packages.** A table: package, status (`todo`, `in progress`, `done`, `BLOCKED`), implementer round, date.
2. **Work package k** (one section per package, written by you): what you did, files created or changed
   (path and reason), decisions you had to take within the spec (and why), commands run with the result
   (last lines of output), what is left.
3. **Verified by orchestrator.** Written by the orchestrator, never by you.
4. **Deviations and amendments.** Every difference from the spec, with the user's approval reference.
5. **Open problems.** Anything unfinished, flaky, or suspicious, with a way to reproduce.
6. **Evidence table.** Acceptance criterion, command or test name, result, date. Update the rows your
   package touches.
7. **Review round k fixes** (when applicable).

Write the log as you go, not at the end: if you run out of context, the next implementer starts from it.
When you are close to your limit, stop at a coherent point, set the package `in progress`, and describe the
exact next step.

## 3. Reporting

Your final message is short: package, status, the verification commands and their last lines, what is
unfinished, and anything you could not verify. Do not describe the code at length; it is in the log and the
diff.

## 4. Review pack (last package only)

When your package is the last one, also write the section **Review pack** at the top of the log: the
evidence table complete, the list of changed files with one line of reason each, deviations, known
limitations, and how to run everything (commands). The user reads this section first.

## 5. Prompt template (used by the orchestrator)

```
You are the implementer for pymarty phase <N>, work package <k>: "<title>".
Read docs/architecture/pymarty/process/implementer.md and follow it exactly.
Spec: docs/architecture/pymarty/phase-<N>/03-spec.md. Log: docs/architecture/pymarty/phase-<N>/04-implementation-log.md.
Round: <1|2 for the same package, with the failing output pasted below when it is a retry>.
Verification commands for this package: <commands>.
Standing rules: <paste section 3 of docs/architecture/pymarty/process/README.md>.
Final message: status, commands run with last lines of output, what is unfinished.
```
