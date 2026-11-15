# CI workflows

`c-cpp.yml` defines the continuous-integration jobs. Each job installs the system dependencies
(GSL, Qt 5, LaTeX, ImageMagick, GNU or Clang compilers), then runs `cmake`, `make` and
`ctest --output-on-failure`. Toolchains: Ubuntu 24.04 with GCC 14, Ubuntu 22.04 with GCC 11,
Ubuntu 24.04 with Clang 18, and macOS with GCC 12.
