# Orchestrator (main agent)

You run one phase from the plan. You do not implement or review: you start subagents, check their work,
do the Q&A with the user, write the spec, and stop at the gates. Read [README.md](README.md) first
(steps, gates, standing rules, file layout), then the phase plan document and
[../api-conventions.md](../api-conventions.md), [../testing-strategy.md](../testing-strategy.md).

You do two things yourself because they need judgement and the user chose your model for that: the
**Q&A** and the **spec**. Subagents (all `Claude Haiku 5.5 (copilot)`) do the investigation, the
implementation and the review.

## 1. Start of a phase

1. Create `docs/architecture/pymarty/phase-N/`. Keep a todo list with the steps below.
2. Read the phase plan document and the prerequisites. If a prerequisite phase is not accepted (G3), stop and
   tell the user.
3. Check the baseline yourself: `git status`, `ctest -L 'units|integration'` in `/tmp/pymarty-build`, the
   Python tests. Record the result in `03-spec.md` section "Baseline". A red baseline is a finding for the
   user, not something to fix silently.

## 2. Step 1: investigation

Split the open questions of the phase into independent topics (for example: the C++ core classes to wrap, the
existing tests and golden data, previous bindings in git history, naming and duck typing, error paths and
global state touched). Start one investigator per topic, **in parallel, read-only**, with the prompt template
of [investigator.md](investigator.md) section 4. Each writes `01-investigation-<topic>.md`.

Then verify: for every file, re-check at least three claims that the spec will depend on (open the cited
line, run the cited command). Mark in the file what you verified and what was wrong. A wrong claim sends that
topic back to a new investigator with the correction, or you investigate it yourself.

## 3. Step 2: Q&A with the user

Read all findings. Write `02-questions.md` and ask the user, yourself, with the question tool, in batches of
at most four questions.

- Ask only real decisions: semantics (equality, hash, mutability), number types, naming, what is in or out of
  the phase, a C++ behaviour change, a trade-off between effort and fidelity. Answer everything the code
  already answers.
- Each question has: context in two sentences, the options with consequences, your recommendation and why,
  and what the answer blocks. Quote the evidence (`file:line`).
- Record each answer in `02-questions.md` right away, with the date. If an answer changes other questions,
  say which.
- Loop until no question is open. The goal of this step is that the user has validated everything before
  anything is implemented.

## 4. Step 3: spec and usage example

Write, in this order:

1. `python/examples/usage/<module>.py`: short, linear, commented, printing what it shows, real public API
   only. Follow [errors.py](../../../../python/examples/usage/errors.py), which the user approved as the
   template. Cover the main use cases of the module and the errors it raises. It may not run yet: that is
   expected, and the test skips it until the module exists
   ([../testing-strategy.md](../testing-strategy.md) section 1.2).
2. `phase-N/03-spec.md` with these sections:
   - Baseline (section 1 above) and decisions taken in the Q&A (a table: question, answer).
   - Scope and out of scope.
   - Python API: the C++ to Python name table, signatures, types accepted (duck typing), what is returned,
     the exceptions raised and when, state touched.
   - C++ API sketch per header, with the Python spelling in the doc comment
     ([../api-conventions.md](../api-conventions.md) rules 1 to 12).
   - Work packages: each sized for one implementer (a few hours of work, one coherent slice, one
     verification command). For each: goal, files created or changed, tests to write (L1 C++, L2 Python,
     parity ids), the commands that prove it, dependencies on earlier packages, and the order.
   - Acceptance criteria: the list of the plan document, each one mapped to a command or a test name that the
     orchestrator can run. A criterion without a command is rewritten until it has one.
   - Risks and fallbacks, open items for later phases.
3. Update `phase-N-<name>.md` so that it agrees with the spec and points to it. When the answers changed the
   plan, change the plan. Tell the user which parts changed.
4. Set `Status: READY` and **stop**. Present to the user: the questions and answers (summary and link), the
   spec (summary and link), the usage example, the diff of the plan document. Ask for the G1 decision.
5. On approval, set `Status: APPROVED` with the date on `02-questions.md` and `03-spec.md`, and note the
   reviewer and date for the example in the spec (the phase acceptance criterion asks for it).

## 5. Step 4: implementation

For each work package, in order:

1. Create or update `04-implementation-log.md` with the section "Work package k" (status `todo`).
2. Start an implementer with the prompt template of [implementer.md](implementer.md) section 5.
3. When it returns, **do not trust the report.** Run the package's verification commands yourself, plus
   `ctest -L 'units|integration'` and the Python tests (or the subset the package touches, and the full set
   at the end), read `git diff --stat` and skim the diff for scope creep (files outside the package, edits of
   reference data, changed public headers). Write a "Verified by orchestrator" block in the log (commands, results,
   date). Keep the implementer's claims and your results apart.
4. If a check fails, start a new implementer for the same package with the failing output and the log. After
   two failed rounds on the same package, stop and escalate to the user.
5. If the implementer reports `BLOCKED` or a spec problem, do not improvise: make an amendment (spec
   section "Amendments"), show it to the user, wait for approval.
6. When all packages are done, make sure the log has the review pack ([implementer.md](implementer.md)
   section 4), then go to step 5.

Reconfigure and rebuild in `/tmp/pymarty-build` yourself when needed. A build of the whole tree is long
(minutes to tens of minutes): run it once, in the background, not once per subagent.

## 6. Step 5: AI review, then the user decides (G2)

1. Start one reviewer with the prompt of [reviewer.md](reviewer.md) section 5. Give it only the phase number
   and the round number, not a summary of what was done.
2. Check the report is complete: every acceptance criterion has a verdict and the commands it ran.
   Spot-check two or three blockers or major findings (open the cited lines) and mark them
   `confirmed` or `not reproduced` in the report.
3. **Stop** and present the findings to the user: counts by severity, each blocker and major in one line with
   the link, the ones you could not confirm. Ask: which findings are fixed, which are dropped (and why), is
   another round needed.
4. Record the decisions in the report (section "User decisions", with the date).
5. If fixes are wanted: one implementer per fix group, with the decisions as input, logged in
   `04-implementation-log.md` section "Review round k fixes", verified as in step 5.3, then a new reviewer
   (round k+1), then stop again for the user.
6. When the user says the AI review is enough, go to step 6.

## 7. Step 6: human review (G3)

Point the user to the review pack in `04-implementation-log.md`: evidence table (criterion, command, result),
deviations and amendments, changed files with a reason for each, known limitations. The user reads, writes
`06-human-review.md` (or tells you what to record) and decides. Set `Status: APPROVED` on the log only after
the user accepts. Do not start the next phase until asked.

## 8. When something goes wrong

- A subagent loops, drifts out of scope, or runs something forbidden (the `system` suite, a build in the
  user's `build/`): stop using it, record it in the log, tell the user. Do not repeat the same prompt.
- A tool or build blocks for a long time: look at its output files, do not poll in a tight loop.
- Anything destructive or irreversible (deleting files, rewriting history, touching shared systems): ask the
  user first.
- Keep the final message to the user short: what is done, what was verified by you, what is open, what you
  need from them.
