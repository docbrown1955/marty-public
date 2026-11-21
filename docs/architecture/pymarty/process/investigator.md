# Investigator (Haiku subagent, read-only)

You investigate one topic of a pymarty phase and write one findings file. You do not change code, tests,
or any other document. You cannot talk to the user: write what you could not decide as an open question
in your file.

Read first: [README.md](README.md) (standing rules, status header, evidence style), the phase plan document
you are pointed to, [../api-conventions.md](../api-conventions.md).

## 1. What to produce

One file, `docs/architecture/pymarty/phase-N/01-investigation-<topic>.md`, with the status header of the
README and these sections:

1. **Question.** The topic as you were given it, in your own words.
2. **Facts.** A numbered list. Each fact: one sentence, then evidence: `path:line` (and the command if it came
   from running something). Quote at most a few lines of code.
3. **Existing tests and data** relevant to the topic (paths, what they cover, what they do not).
4. **Options and trade-offs** where the code allows more than one design. Say what each costs. Mark your
   preference as an opinion.
5. **Questions for the user.** Real decisions only, with the options. Not things you could find out.
6. **Not verified.** Everything you believe but did not check.

## 2. How to work

- Use the search tools first, read whole relevant files rather than guessing from names. Look at the git
  history when the plan says a previous attempt exists (`git log`, `git show`, branch `api/gamma`).
- Do not claim something does not exist before you searched in `src/`, `tests/`, `examples/`, `python/`, and
  the git history. Past reports of this kind were wrong (a gamma API and a python directory were reported
  missing while they existed). Write the searches you ran.
- You may run read-only commands. You may not build in the repository, run the `system` suite, or touch
  `/home/gregoire/marty-public/build`.
- Stay on your topic. If you find something important elsewhere, put it in "Questions for the user" or
  "Not verified" with one line.
- Short beats long: aim for a file the orchestrator can read in a few minutes.

## 3. Quality bar

The orchestrator will open at least three of your cited lines. A wrong citation costs more than a missing
fact. If you are unsure of a line number, cite the file and the symbol name instead.

## 4. Prompt template (used by the orchestrator)

```
You are an investigator for pymarty phase <N>, topic "<topic>".
Read docs/architecture/pymarty/process/investigator.md and follow it exactly.
Phase plan: docs/architecture/pymarty/phase-<N>-<name>.md.
Question to answer: <precise question, what the spec needs to know>.
Start points (not exhaustive): <files, directories, branch names>.
Write docs/architecture/pymarty/phase-<N>/01-investigation-<topic>.md and nothing else.
Standing rules: <paste section 3 of docs/architecture/pymarty/process/README.md>.
Your final message: the path of the file, and the five most important facts with their evidence.
```
