# Spike C: error paths in the C++ core

Status: DONE for the static audit. Runtime behaviour is covered by `tests/api/src/test_error_api.cpp` (see the Phase 0 report for the run result).

## Question

Which exceptions escape the core in practice (enum, `std::exception`, `GammaAPIError`), and which
paths terminate the process?

## Experiment

Read-only grep over `src/csl` and `src/marty` (excluding `src/marty/ufo`, which has no exit calls):

- `throw` of enum values: `CallHEPError` (`src/marty/core/mrtError.h`), `CallErrorImpl`
  (`src/csl/error.cpp`, used by `CSL_ASSERT`, `CALL_SMERROR`).
- Catch sites of error types: `grep catch` over the same trees.
- Termination: `exit(`, `std::exit(`, `abort(`, `_Exit(`, `quick_exit(` (text as written in code).

## Result

### Throw paths (before WP0.2)

| Source | Thrown object | Caught by | Terminates? |
|--------|---------------|-----------|-------------|
| `CallHEPError(kind, spec)` / `HEPAssert` | `mty::error::Type` enum value (by value) | nothing in the tree catches the enum by value | no |
| `CallErrorImpl` (csl) / `CSL_ASSERT` | `csl::CSLError` enum value | nothing in the tree catches it by value | no |
| `GammaAPIError` (`src/marty/api/gamma_api_error.h`) | `std::exception` subclass | none in core | no |
| `sgl::Exception` | class in sgl | `src/marty/core/diracology.cpp:323` | no |
| std exceptions | `std::exception` family | `src/marty/lha/*` (only handled as std) | no |

Catch of an `int` also exists: `libraryevaluator.cpp:531`.

### Termination paths (exit/abort)

Grep hits: 11. Live calls: 10. One hit is text written into generated C++ (not executed).

| Location | Call | Kind |
|----------|------|------|
| `src/csl/librarytensor.h:81` | `exit(123)` after a stderr message | live |
| `src/csl/librarytensor_hdata.h:86` | `exit(123);\n` written into generated code | generated text |
| `src/csl/librarygroup.cpp:354`, `:382` | `exit(1)` | live |
| `src/marty/core/looptools_patch.h:136` | `exit(1)` | live |
| `src/marty/core/semiSimpleAlgebra.h:984, 999, 1041, 1059, 1078, 1098` | `std::exit(1)` after `std::cout` message (`operator+`/`operator*` on `std::vector` dimension mismatch and similar) | live (6) |

No `abort`, `_Exit`, or `quick_exit` calls were found.

### Consequences for the API

- Any of the 10 live `exit` sites reachable from an API call kills the Python process. These must be
  turned into throws (or the API must avoid those paths) before WP0.3 can claim AC5. Priority:
  `semiSimpleAlgebra.h` (6 sites, dimension checks reachable from user input) and `librarygroup.cpp`.
- The enum-by-value throws are not catchable with a typed `catch`. WP0.2 adds
  `mty::error::Exception : std::runtime_error` (`mrtError.h`) and `csl::api::Error` hierarchy, and
  `rethrowAsApiError()` translates the enum forms.
- The CSL `CallErrorImpl` path loses the function name and line that `CallHEPError` keeps; the
  translated message is the CSL spec text only.

## Decision

1. Keep the enum types for C++ compatibility; add typed translation at the API boundary only.
2. Convert the live `exit` sites to throws (done in WP0.3, see the table below). Two sites are
   kept on purpose and documented as non-reachable from the API.
3. Document the uncaught-termination message change of `CallHEPError` as a behaviour change (it now
   throws `mty::error::Exception`, whose `what()` is the message).

## WP0.3 result: per-site table

Reachability from an API call is judged from the callers, not only from the file name.

| Site | Before | After | Reachable from the API? | Test (`tests/api/src/test_terminating_calls_api.cpp`) |
|------|--------|-------|-------------------------|-------------------------------------------------------|
| `semiSimpleAlgebra.h` `operator+(vector, vector)` | `std::cout` message, `std::exit(1)` | `CallHEPError(ValueError, ...)`, same text, printed to stderr unless quiet | yes (dimension checks on user vectors) | `TerminatingCallsApi.VectorSumDimensionMismatchThrows` |
| `semiSimpleAlgebra.h` `operator-(vector, vector)` | same | same | yes | `.VectorDifferenceDimensionMismatchThrows` |
| `semiSimpleAlgebra.h` `operator*(vector, vector)` | same | same | yes | `.VectorDotProductDimensionMismatchThrows` |
| `semiSimpleAlgebra.h` `operator*(matrix, vector)` | same | same | yes | `.MatrixVectorProductDimensionMismatchThrows` |
| `semiSimpleAlgebra.h` `operator*(vector, matrix)` | same | same | yes | `.VectorMatrixProductDimensionMismatchThrows` |
| `semiSimpleAlgebra.h` `operator*(matrix, matrix)` | same | same | yes | `.MatrixProductDimensionMismatchThrows` |
| `librarygroup.cpp` `printFunctionStackDefinition` | `std::cerr` message, `exit(1)` | `CALL_SMERROR_SPEC(CSLError::ValueError, ...)`, same text without the trailing newline | yes (library generation) | `.LibraryGroupDefinitionWithInvalidTagThrows` |
| `librarygroup.cpp` `printFunctionStackBody` | same | same | yes | `.LibraryGroupBodyWithInvalidTagThrows` |
| `looptools_patch.h:136` `exit(1)` | unchanged | unchanged | no: the header is shared by C (LoopTools) and C++ and the branch is the unreachable `default` of an enum switch | none |
| `librarytensor.h:81` `exit(123)` | unchanged | unchanged | no: never compiled; the file is the template from which `librarytensor_hdata.h` is built (see below) | none |
| `librarytensor_hdata.h:86` | text of generated code | unchanged | not an `exit` of the library; see Phase 6 | none |

Corrections to the earlier audit:

- `librarytensor.h` is not a live call. The file is not included by any compiled unit; it is the
  readable source of the string literal in `librarytensor_hdata.h`. It was left untouched.
- The `semiSimpleAlgebra.h` messages went to stdout; they now go to stderr like every other core
  error, and nothing is printed in quiet mode (`Session`).
- `CSLError` carries no message. `CALL_SMERROR_SPEC` prints the text (when not quiet) and then
  throws the bare enum, so the API translation of CSL errors has no message. Making the CSL error
  type carry its text is a core change left for a later phase.

### Behaviour change for C++ users

The 8 converted sites used to end the process with exit status 1. They now throw
(`mty::error::Exception` with type `ValueError`, or `csl::CSLError::ValueError`). A program that did
not catch them now ends in `std::terminate` (abort, exit status 134) instead of exit status 1. The
message on stderr is the same text. Nobody can rely on the old behaviour to continue, so this only
matters for scripts that check the exit status.

### Audit of `catch` by value of `mty::error::Type`

`grep -rn "catch" src tests examples` (source files only) finds no handler for `mty::error::Type`
or `csl::CSLError` by value except the translator `rethrowAsApiError()` in
`src/marty/api/common/error.h` (intended). The other handlers are `sgl::Exception`
(`src/marty/core/diracology.cpp`), `int` (`libraryevaluator.cpp`) and `std::out_of_range` /
`std::invalid_argument` in `lha` (`src/marty/lha/lha.cpp`, `tests/system/libsrc/lha.cpp`). The change of `CallHEPError` to throw
`mty::error::Exception` (derived from `std::runtime_error`) therefore breaks no handler in the tree.
User code that catches `mty::error::Type` by value no longer compiles into a working handler: it must
catch `mty::error::Exception` and read `type()`. This is recorded in section C of the report.
