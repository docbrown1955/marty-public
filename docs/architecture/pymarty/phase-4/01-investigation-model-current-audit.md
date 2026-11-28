Status: READY
Updated: 2026-11-21
Author: orchestrator (written from the findings of investigator "inv-global-state"; the subagent could not write files, the orchestrator re-checked the lines marked [V])

# Investigation: `Model::current` and global state during model building

## 1. Question

Where is `Model::current` (and the other process-wide state) read or written during model building, particle creation and Feynman-rule computation? Which API functions need a `Session` guard (plan WP4.1)?

## 2. Facts

1. [V] `Model` construction sets `Model::current = this` unconditionally; the destructor sets it to `nullptr` only if it points to the destroyed model. It never restores the previously current model. `src/marty/core/model.h:66, 74-84`.
2. [V] `mty::api::Session` snapshots `Model::current` and every flag of the X-macro lists `MTY_MARTY_OPTION_FLAGS` and `MTY_CSL_OPTION_FLAGS`. It sets `printErrors = false` for both namespaces, can set `Model::current` to a given model, and restores everything in its destructor. `src/marty/api/common/session.h:33-63`.
3. [V] `Session` is RAII, single-threaded, and not copyable or movable. It is an internal C++ guard; the Python `clean_state` fixture checks its effect (`docs/architecture/pymarty/testing-strategy.md`, and `tests/api` exemptions in `python/tests/parity_exemptions.txt:5`).
4. [V] `src/marty/core/mrtInterfaceCurrent.h` wraps core functions on `*mty::Model::current` with no check for null: `AddGroup` (line 59), `AddParticle` (64), `AddTerm` (69, 74), `GetParticle` (79), `PromoteGoldstone` (85, 90), `GetCoupling` (95), `GetYukawa` (100), `ComputeFeynmanRules` (47), and `Replaced`, `Rotate`, `BreakGaugeSymmetry`, `Refreshed`, `Init` (reported, lines 108-194 not individually re-checked).
5. Reported, not re-checked: particle creation helpers used in built-in models (`scalarboson_s`, `weylfermion_s`, ...) read `Model::current` implicitly; built-in model constructors (`thdm.h`, `sm.cpp`) call them directly. A facade that creates particles must do so with the target model explicitly, not through `Model::current`.
6. Reported, not re-checked: `mty::api::diracology` creates indices referencing the global `csl::Minkowski` space and `mty::dirac4`. `Session` does not snapshot those globals (`src/marty/api/diracology.cpp:40-60`).

## 3. Consequences for the facade

- Every `ModelHandle` method that calls the core must run inside a `Session` bound to its own model. Then two models in one process can be built in any interleaving, and `Model::current` is unchanged after each call (plan acceptance 4).
- Constructing a `Model` in the facade sets `current` as a side effect and does not restore the previous value. The facade's `ModelHandle` constructor must therefore wrap the construction in a `Session` that restores the previous `current` (the destructor of `Model` cannot restore it). **Opinion: this is the main risk of the phase; test it (`SessionRestoresCurrentModel`).**
- Options in `csl::option` and `mty::option` used by feynman rules or LaTeX output are restored by `Session` only if they are in the X-macro lists.

## 4. Options and trade-offs

- A. Wrap every public call in `Session(model)`. Cost: small overhead; matches `api-conventions.md` section 3. **Recommended.**
- B. Only wrap construction and the calls that read `current`. Cost: fragile when the core adds new reads. Not recommended.

## 5. Questions for the user

Captured in [02-questions.md](02-questions.md) (Q4).

## 6. Not verified

- Full line list for `mrtInterfaceCurrent.h:108-194`.
- The exact set of globals read by particle creation helpers (needs a spike, WP4.1 deliverable).
- Whether `Feynman rule` computation reads flags outside the X-macro lists (not checked).
