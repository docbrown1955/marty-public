# System test library sources

`tests/system/libsrc/` holds C++ sources that are used as generated or example libraries by some
system tests. Examples: `C7_SM.cpp`, `C8_SM.cpp`, `ckm.*` (CKM matrix utilities), and the
`example_bsmixing*.cpp` programs for B-meson mixing with vector-like quarks.

They are compiled and linked against the libraries that the system tests generate under
`tests/system/libs/` (created at run time and removed by the cleanup test).
