# LHA input

Reader for Les Houches Accord (LHA) parameter files, used for SM and SUSY input values.

- `lha.h` / `lha.cpp`: `mty::lha::Reader` parses a `.lha` file into blocks and entries.
- `lhaData.h` / `lhaData.cpp`: the data container (`LHAFileData`) holding every block with its
  elements.
- `lhaBlocks.h`: the block names used by MARTY.

Only the parameters already used by the built-in models (for example the pMSSM or NMFV inputs)
are supported. Built-in defaults live in `src/marty/core/lhaBuiltIn.h`. The folder builds the
`marty-lha` shared and static libraries. The system test `test_lha_reader` exercises it.
