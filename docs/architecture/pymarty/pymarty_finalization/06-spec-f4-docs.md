Status: DRAFT (not frozen; revised after the F3 gate)
Updated: 2026-11-28
Author: main agent (orchestrator)
Depends on: F3 (API stable)

# F4 spec: documentation

Scope: a user guide for the Python package, one page per module generated from docstrings with executed examples,
migration notes from the C++ API, a known-limitations page, and a developer page listing the remaining gaps.
Source: [USER_FEEDBACK.md](../USER_FEEDBACK.md) "Doc", [01-inventory.md](./01-inventory.md) section D,
phase 8 WP8.1 and acceptance criteria 4 and 10.

## 1. Decisions (to record before G1 of F4)

- **Site generator:** Sphinx with autodoc, or MkDocs with mkdocstrings. Not chosen yet. The choice sets the build
  command used in CI.
- **Location:** a `docs/pymarty/` tree, separate from `docs/architecture/` (which holds planning documents). The
  Doxygen pages for C++ (`docs/doc/`, `src/doc/`) are not changed.
- **Executed examples:** doctest (already used by `test_docstrings.py`) for short examples, and the notebooks of F2
  rendered into the site for longer tutorials.

## 2. Steps

1. Getting-started page: install (wheel and source build, pointing to F5 for the Docker image), first calculation.
   This is the page that phase 8 criterion 11 links the examples from.
2. One page per module (eight modules after F3), generated from docstrings. Each docstring example is executed.
3. Tutorials: the ports of `gettingStarted`, the QED amplitude, the group-theory tour, the gamma algebra and the
   library evaluation (phase 8 WP8.1). They reuse the F2 notebooks; no new physics content.
4. Migration notes "from the C++ API to pymarty": collect the name tables of `phase-1` to `phase-7` documents into
   one reference (inventory section D, row "Migration notes"). Only copies are made; the phase documents are not
   edited.
5. Known limitations page: single-threaded API (F5 mutex), in-process loading risk, GRAFED and LaTeX as optional
   tools, platforms (Linux x86_64 only, no ARM), models that are not yet ported (F6 list).
6. Developer page: the remaining gaps from [01-inventory.md](./01-inventory.md) section E, kept in sync with the
   F6 plan. This page replaces the planning folder as the place users see the gaps.

## 3. Verification

- Documentation build with no warnings (criterion 4).
- Link check on the built site.
- Doctest coverage report: 100% of public functions and classes have a docstring with an executed example, and
  exceptions are listed with the reason they are raised (criterion 4).
- Docstring examples executed in CI, same as `test_docstrings.py`.
- The notebook tutorials executed (F2 CI job).

## 4. Acceptance

- Every public item of the API surface table (F3) has a page and a tested example.
- Known limitations page lists each item of criterion 10.
- Developer gaps page matches F6 and is reviewed by the maintainer.

## 5. Out of scope

- API changes (F3) or new features (F6). A missing docstring is fixed here; a missing function is not.
- The C++ Doxygen documentation (not part of this step).

## Open points

- Whether the site is hosted on GitHub Pages or only built in CI (decide with F5 release process).

## Amendments

(none)
