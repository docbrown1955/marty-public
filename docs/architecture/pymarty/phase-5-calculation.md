# Phase 5: `pymarty.calculation` (tree level) and `pymarty.visualization`

Amplitude computation with options and filters, squared amplitudes, textual display, and an export
and launcher path for the GRAFED diagram viewer.

Prerequisites: Phase 4 accepted.

## 1. Scope

In scope, calculation:
- Insertions: `incoming`, `outgoing`, `anti`, chirality helpers (`Left`/`Right` as in `Incoming(Left("e"))`).
- `Order` (`TreeLevel` now, `OneLoop` accepted by the API but its use is validated in Phase 7), `FeynOptions` as
  a plain data object (topology, filters, fermion order and the other fields of `feynOptions.h` that
  tree-level examples use), the global `mty::option::*` flags exposed as a validated `options(...)`
  context manager.
- `Model.amplitude(order, insertions, kinematics=None, options=None)`.
- `Amplitude` (diagrams, expression, options used), `Diagram` (read access), `Model.squared(amp)`,
  `Model.squared(amp_left, amp_right)`.
- Kinematic variables: symbolic invariants `s_ij`, momenta symbols.
- Display: `amplitude.display()` returning text (`Display(...)` writes to streams today), abbreviations listing.
- Ghosts and Goldstones as particles in insertions (needed by `gg_to_tt`).

In scope, visualization (module `pymarty.visualization`):
- C++ JSON export of diagrams and Feynman rules without GUI and without process launch.
- Safe GRAFED launcher and locator.

Out of scope: Wilson coefficients and widths (Phase 7), loops (Phase 7), libraries (Phase 6),
notebook inline rendering (Phase 8).

## 2. Work packages

### Calculation
- WP5.1 `src/marty/api/calculation/`: `InsertionHandle` (name, direction, antiparticle, chirality),
  `FeynOptionsData` (plain struct mirroring `FeynOptions` with validation), `AmplitudeHandle`,
  `DiagramHandle`, functions `compute_amplitude`, `compute_squared_amplitude`, `display_text`,
  `abbreviations_text`. Variadic or initializer-list core overloads of `computeAmplitude` are
  hidden behind one vector-based function. The `Session` applies options and restores state, also when the
  computation throws.
- WP5.2 Global flags: list every `mty::option::*` flag (from the X-macro of Phase 0), give each a Python
  keyword name (snake case), expose `options(**flags)`; unknown name raises `ValueError` listing valid
  names; value types checked.
- WP5.3 Filters: `forbid_particles`, `force_particle`, `disable_particles`, topology, `fermion_order`
  as data objects converted to `filter::...` in C++.
- WP5.4 Kinematics symbols and squared amplitude facade.
- WP5.5 Examples port and tests.

### Visualization
- WP5.6 Spike: confirm whether `drawer::LatexLinker::saveMultiple` (in `src/marty/core/drawer.cpp`) runs
  LaTeX at export time or only stores the label strings, and which tools are required for pure JSON
  export. Document the JSON schema (`src/grafed/jsonparser`).
- WP5.7 C++ API `src/marty/api/visualization/`:
  - `diagrams_json(AmplitudeHandle)`, `rules_json(FeynmanRules)` returning `std::string`
  - `save_diagrams(obj, path)` writing the file with owner-only permissions
  - `find_grafed(Config)` (explicit path, `MARTY_GRAFED`, `PATH`, install prefix)
  - `launch_grafed(path_to_json, Config)` using `posix_spawn` with an argument vector, non-blocking,
    returns a handle with `pid`/`poll`/`wait`
  - The existing `Show()` keeps its signature but is reimplemented on these functions: no fixed
    `/tmp/graphs.json`, no `system()`. This removes the predictable-temp-file and shell-command risk
    for C++ users as well.
- WP5.8 Python `pymarty.visualization`: `export_json(obj)`, `save(obj, path)`, `grafed.available()`,
  `grafed.show(obj, wait=False)` (temporary file in a private directory, deleted when the viewer
  exits or on request), and a convenience `visualization.show(obj)` that uses the GUI backend if
  available and raises `BuildError`-style `MartyEnvironmentError` with install instructions otherwise.

## 3. Python spelling

| C++ | Python |
|-----|--------|
| `Incoming("e")`, `Outgoing(AntiPart("u"))` | `incoming("e")`, `outgoing(anti("u"))` |
| `Incoming(Left("e"))` | `incoming(left("e"))` |
| `Order::TreeLevel` | `Order.TREE` |
| `model.computeAmplitude(Order::TreeLevel, {Incoming("u"), ...})` | `model.amplitude(Order.TREE, [incoming("u"), incoming(anti("u")), outgoing("d"), outgoing(anti("d"))])` |
| `FeynOptions opts; opts.setTopology(Topology::Box); opts.addFilter(filter::forceParticle("t"))` | `FeynOptions(topology="box", filters=[force_particle("t")])` |
| `model.computeSquaredAmplitude(res)` | `model.squared(res)` |
| `Display(res)` | `res.display()` returns `str` |
| `mty::option::simplifyAmplitudes = false;` | `with options(simplify_amplitudes=False): ...` |
| `Show(res)` | `visualization.show(res)` (needs GRAFED); `visualization.export_json(res)` (no GUI) |
| `SaveDiagrams(file, res)` | `visualization.save(res, path)` |

Duck typing: insertions accept strings (particle names), `Particle` objects, or insertion objects;
`anti` accepts the same. Filter lists accept any iterable. Options are a dataclass-like object or any
`Mapping` (coerced). Particle names are looked up in the model, with a clear `LookupError`
listing close matches.

## 4. Test plan

L1 (`tests/api/calculation/`, `tests/api/visualization/`):
- Insertions: `InsertionDirectionAndAnti`, `ChiralityInsertion`, `UnknownParticleFails`,
  `UnbalancedProcessFails` (wrong number of legs or charge conservation violation, where the core
  detects it).
- Amplitudes: `QedComptonTreeLevelDiagramCount`, `QedEeToMumuTreeLevel` (both with the model built
  through the API; compared with the direct core calls: identical strings for amplitude and squared
  amplitude), `ScalarQedProcess` (as in `sampleSQED`), `QcdSextetProcess` (as in `sampleQCDSextet`),
  `GhostLegsInGgToTt` (as in `gg_to_tt`, symbolic part).
- Options: `FeynOptionsRoundTripToCore`, `FilterForceAndDisable`, `FermionOrder`, `TopologyEnumeration`,
  `GlobalOptionAppliedAndRestored`, `UnknownOptionFails`, `OptionRestoredAfterException`.
- Squared amplitudes: `SquaredAmplitudeMatchesCore`, `InterferenceOfTwoAmplitudes`,
  `FlavorGroupSymbolicNf` (from `test_flavor_groups.cpp`: result depends on symbol `Nf` and the
  numeric evaluation for Nf = 3 equals the numeric-flavour computation).
- Physics sanity check, tolerance 1e-9 with exact rationals where possible: for massless e+e- to mu+mu-
  in QED the spin-averaged squared amplitude equals `2 e^4 (t^2 + u^2) / s^2`, checked by substituting numeric
  kinematics into the symbolic expression (textbook result, written in the test as a literal formula).
- Display: `DisplayReturnsString`, `DisplayDoesNotPrintToStdout`.
- Visualization: `DiagramsJsonParsesWithGrafedParser` (round trip through `src/grafed/jsonparser`),
  `RulesJsonParses`, `SaveCreatesPrivateFile` (permissions), `LaunchUsesArgumentVector` (with a fake
  executable that records its argv; paths with spaces and shell metacharacters stay one argument),
  `FindGrafedOrder`, `NoFixedTmpFile` (two concurrent exports use different files), `ShowLegacyStillWorks`.

L2 (`python/tests/calculation/`, `python/tests/visualization/`): parity tests for all L1 cases plus
- duck typing: names versus particle objects versus insertion objects, mapping options, iterables
- `with options(...)` restores flags (also on exception); `clean_state` fixture
- amplitude and diagram iteration, `len`, `repr` stays short
- `display()` returns text; legacy C++ output goes to `sys.stdout`
- visualization with `fake_grafed` fixture: file written, launcher called once, exceptions when missing
- `visualization` import does not need Qt; works on a machine without GRAFED

L4 examples (fast, every PR): ports of `sampleQED`, `sampleSQED`, `sampleQCDSextet`,
`test_flavor_groups` scenarios; output of deterministic parts compared with C++ example output.

L5 (`slow`, run manually by the maintainer): symbolic parts of `Zmumu`, `gg_to_tt`, `EWGauge` (tree level, no library yet),
`SM_triplet`, `testConjugation` where they do not need a library or loops; compared to values the C++ programs
print (recorded once in golden files by the C++ side).

## 5. Acceptance criteria

1. For each scenario listed under "Amplitudes" the API result is string-identical to the same
   computation performed through the core directly (both executed in the same C++ test).
2. The textbook massless e+e- to mu+mu- check passes (formula as literal in the test).
3. All `mty::option::*` flags are reachable by keyword through `options(...)`, with a test that fails if
   the flag list changes without a matching Python name (built from the Phase 0 list).
4. Flags and `Model::current` are unchanged after any API call, including failing ones (state tests,
   sanitizer job).
5. `FeynOptions` filters and topologies used by `sample2HDM`, `C9_SM`, `BsMixingSM` and `EWGauge`
   (read from those C++ files) can be expressed in Python (listing in the PR, tested by constructing
   the options and comparing with the C++ ones).
6. Visualization: no `system()` and no fixed temp path in the new code (grep gate in CI for
   `system(` in `src/marty/api/`); the legacy `Show()` no longer uses `/tmp/graphs.json`. All launcher
   tests pass without a GUI.
7. JSON export of a diagram set round-trips through the GRAFED JSON parser and equals the file
   written by the legacy `SaveDiagrams` for the same amplitude (byte equality, or documented
   differences if the format has intentionally changed).
8. Python ports of `sampleQED`, `sampleSQED`, `sampleQCDSextet` and the Nf scenario run in CI with
   output matching C++. Time budget 1.5x C++ plus 1 s (Release).
9. `ctest -L 'units|integration'` green. System tests are not a criterion; the PR lists the ones that touch
   `Show` or `drawer.cpp` (by name, if any) for an optional manual run.
10. Parity, stubs, doctests, sanitizers and quality gates pass.

## 6. Exit artifacts

Stubs, docstrings, examples, option name table (C++ flag to Python keyword), JSON schema document for
GRAFED export, spike record for `LatexLinker`.

## 7. Risks and open questions

- The amplitude computation can be long and memory hungry, with no cancellation: documented. Python
  `KeyboardInterrupt` is not honoured until the call returns (Phase 8 evaluates cooperative checks).
- The global option flags interact (some imply others). The tests pin the combinations that the
  examples use.
- The GRAFED JSON may embed LaTeX labels compiled by the viewer: if export needs `latex`, the wheel and
  docs must say so (spike 5.6).
- Without GRAFED installed, users only get export; this is by design (decision D7).
