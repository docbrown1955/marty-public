# Spike B: two models in one process

Status: PARTIAL. Model-pointer lifetime is covered by tests; the audit of global state is not finished.

## Question

Can two `Model` objects be created, used and destroyed in one process (sequentially, interleaved, or after
an exception) without leaking state from one into the other?

## Experiment

- `tests/api/src/test_session_api.cpp`, tests `SessionApi.TwoModelsSequentialAndInterleaved` and
  `SessionApi.ModelDestructorClearsCurrent`, plus `RestoresCurrentModelOnExit` and
  `RestoresCurrentModelOnException`.
- Run: `ctest -L api` in `/tmp/pymarty-build` (modified tree, Release, g++-12).

## Result

- `ctest -L api`: 14/14 passed (includes the four Session tests above).
- `Model::current` is set by the constructor, cleared by the destructor when it points at the same object, and
  restored by `mty::api::Session` on scope exit (normal and exceptional).
- Known limitation: if a model is destroyed while a `Session` still holds its pointer, the restore writes back a
  dangling pointer. The API must not hand out a `Session` whose model outlives its owner.

## Not audited (open items)

- Static registries, the `csl::Abbrev` cache and allocator state were not read in this spike. Two models sharing
  these caches is not proven safe; this needs a dedicated audit before the Python layer claims multi-model support.
- No test yet builds two full models with different particle content and checks the generated output.
