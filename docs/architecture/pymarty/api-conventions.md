# API conventions (C++ API layer and Python layer)

These rules are part of the acceptance criteria of every phase. Reviewers check them.

## 1. C++ API layer

Location: `src/csl/api/` and `src/marty/api/<module>/`, built as object libraries linked into
`csl` / `marty` (and their static versions), like the existing `marty-gamma-api`.

1. **Python neutral.** No pybind headers. No templates, variadics, macros or `initializer_list` in
   public signatures. Use `std::string`, `std::vector`, `std::map`, `std::optional`,
   `std::complex<double>`, `enum class`, small structs.
2. **Plain data in and out.** Numbers: `int64_t`, `double`, `std::complex<double>`, and
   `Rational{numerator, denominator}` for exact rational results. Never return a symbolic object
   where an exact number is meant (for example a Casimir): the API converts and fails with
   `ValueError` if the value is not rational.
3. **Value handles with explicit ownership.** Handle classes (`ExprHandle`, `ModelHandle`,
   `ParticleHandle`, ...) are cheap to copy. A child handle keeps its parent alive (a particle keeps
   its model alive).
4. **No observable mutation of shared nodes.** Functions return new objects. If a core function
   mutates in place (`Expand(ref)`, `Evaluate(ref)`), the facade deep-copies first.
5. **No hidden global state.** An API function must leave `Model::current`, `mty::option::*`,
   `csl::option::*` and the gamma API 4D flag as it found them. Behaviour that today depends on a
   global flag becomes an explicit options struct argument, applied through the `Session` guard
   (section 3).
6. **Typed errors.** Every API function reports failures by throwing a subclass of
   `mty::api::Error : std::runtime_error` (for CSL-only code: `csl::api::Error`, the MARTY class
   derives from it). Subclasses: `ValueError`, `TypeError`, `LookupError`, `NotImplementedError`,
   `StateError`, `BuildError`. The message is complete and never printed to stderr by the API.
   Core exceptions (the bare `mty::error::Type` enum thrown by `CallHEPError`, `std::exception`,
   `GammaAPIError`) are translated at the API boundary. Nothing may call `exit`, `abort` or
   `std::terminate` on an API path.
7. **No side effects unless requested.** No files, processes or output streams unless the function
   is documented as doing so and takes the path/target explicitly. Printing functions return
   `std::string`; stream variants may exist for C++ users only.
8. **No shell.** Processes are started with an argument vector (for example `posix_spawn`), never
   `system()` with a composed string. File names and library names are validated
   (`[A-Za-z_][A-Za-z0-9_]*` for names, absolute paths canonicalised).
9. **Thread safety.** Not thread safe. The API documents "single threaded, one `Session` at a time".
   The bindings hold the GIL for the duration of each call (releasing it is a Phase 8 item, together
   with a process-wide API mutex).
10. **Python spelling in the header.** Each public function has a doc comment containing its
    intended Python name and a one-line example. The doc comment is the contract between the C++
    test and the Python test.
11. **Compatibility.** Existing public headers, signatures and behaviour stay intact. New code is
    additive (`gamma.h` stays as is and `mty::api::diracology` aliases it).
12. **Style.** `.clang-format`, `-Wall -Wextra -Wpedantic` clean, comments only where they clarify.

## 2. Python layer

1. **Syntax only.** The bindings and `src/pymarty` contain: coercion of arguments, operators,
   `__iter__`/`__len__`/`__getitem__`, `__repr__`/`__str__`/`_repr_latex_`, keyword arguments,
   default values, exception translation, stream redirection, and re-exports. Any `if` that decides
   *physics or algebra* belongs to C++.
2. **Duck typing.** Functions accept protocols, not concrete classes:
   - *Numbers*: anything with `__index__` (exact integer), `fractions.Fraction`-like
     (`numerator`/`denominator`), `__complex__`, `__float__`. Tried in this order: exact first, so
     `Fraction(1, 6)` is never turned into a float.
   - *Expressions*: any pymarty object, number, or object with a `__marty_expr__()` method returning
     a symbolic expression. Strings are only used where documented as names (`"g"` as a symbol name
     in `constant("g")`, particle names in `model.particle("e")`), never parsed as formulas.
   - *Sequences and mappings*: any iterable / `collections.abc.Mapping`; results are `tuple`, `list`,
     `dict` or immutable mapping views as documented.
   - *Callables*: none required by the API. Callback-taking C++ functions are not exposed.
   - Return values are standard Python types (`int`, `Fraction`, `complex`, `str`, `tuple`, `dict`)
     or pymarty objects.
   - Type stubs use `typing.Protocol` for these inputs (`SupportsExpr`, `SupportsRational`, ...).
3. **Exceptions.** One base `pymarty.MartyError`. Each subclass also derives from the matching
   built-in (`MartyValueError(MartyError, ValueError)`, `MartyTypeError(..., TypeError)`,
   `MartyLookupError(..., LookupError)`, ...) so duck-typed callers catching built-ins keep working.
   The message comes from C++. Pybind default translation of `std::exception` is never relied on.
4. **Output.** Legacy C++ code that prints to `std::cout` is wrapped by an output redirect so text
   goes through `sys.stdout` (visible in notebooks and capturable with `capsys`). New API functions
   return strings instead.
5. **Immutability and equality.** Objects are treated as immutable values. `__eq__` returns a `bool`
   with the C++ API's documented meaning (structural comparison). `__hash__` is defined only where
   it is consistent with `__eq__` (Phase 1 decides for `Expr`). Never define `__eq__` returning a
   symbolic equation object.
6. **Naming.** `snake_case` functions and methods, `CapWords` classes. C++ `computeAmplitude` becomes
   `Model.amplitude(...)`, `Incoming("e")` becomes `calculation.incoming("e")`. Each phase document
   includes the C++ to Python name table.
7. **Context managers instead of global switches.** Example: `with symbolic_dimension():`,
   `with options(simplify_amplitudes=False):`. They use the `Session` guard, and restore state on
   exit even on exceptions.
8. **Independence.** `group_theory` and `diracology` do not import the other pymarty modules (tested
   by importing them in a subprocess with the other modules blocked).
9. **Lazy heavy work.** Importing `pymarty` must not run computations, create files, or start
   processes.

## 3. State management: the `Session` guard

Provided by Phase 0 in `src/marty/api/common/session.h` (CSL-only part in `src/csl/api/`).

- Constructing a `Session` snapshots `Model::current`, all `mty::option::*` and `csl::option::*`
  flags, and the gamma-API symbolic-4D flag.
- It applies the requested options and sets `Model::current` to the given model when relevant.
- The destructor restores the snapshot (also during stack unwinding).
- Every API function that depends on global state opens a `Session` internally. No API caller needs
  to know about it. Python context managers expose it for user-visible switches.
- The list of option flags used by the snapshot is generated from one list (X-macro in
  `mrtOptions.h` / `csl/options.h`) so that a new flag cannot be forgotten. A test fails if a flag
  declared in the headers is not covered.

## 4. Handles, ownership and lifetime

- Python objects hold C++ handles by value (`shared_ptr` inside). Python never holds raw pointers.
- Relationships that must keep objects alive are expressed in C++ (handle contains the parent) and not
  with `py::keep_alive`, so the C++ API tests cover them too.
- Reference cycles are avoided: a model does not hold handles to its Python wrappers.
- Tests: drop the model, run `gc.collect()`, use the particle (must still work); drop everything,
  check with a weak reference or a live-object counter exposed under `pymarty._debug` that nothing
  leaks.

## 5. Naming of tests and traceability

- C++: `TEST(<Module>Api, <Behaviour>)` in `tests/api/<module>/`.
- Python: `test_<behaviour>` in `python/tests/<module>/` marked
  `@pytest.mark.parity("<Module>Api.<Behaviour>")`.
- Details and the checker are in [testing-strategy.md](testing-strategy.md).
