# CSL library

Evaluation and grouping of expressions for generated numerical libraries.

- `libraryevaluator.cpp`: evaluation support for functions of a generated library.
- `libraryfunction.cpp`: a function exported by a generated library, built from an expression.

The generator itself (`LibraryGenerator`, `LibraryGroup`, `LibraryFunction`, `LibraryTensor`)
and its headers live in `src/csl/`. The C++ templates it writes are embedded there as
`lib*_data.h` headers.
