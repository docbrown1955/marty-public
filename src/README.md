# Source tree root

`src/` contains the four components of the build, each in its own folder, and three umbrella
headers:

- `csl/`: the computer algebra kernel ([OVERVIEW](csl/OVERVIEW.md)).
- `marty/`: the physics layer ([OVERVIEW](marty/OVERVIEW.md)).
- `marty-plugins/`: plugins built on MARTY ([OVERVIEW](marty-plugins/OVERVIEW.md)).
- `grafed/`: the Qt diagram editor ([README](grafed/README.md)).
- `doc/`: Doxygen configuration ([README](doc/README.md)).
- `csl.h`, `marty.h`, `clooptools.h`: umbrella headers for user programs. `marty.h` is what the
  examples include.
- `CMakeLists.txt`: adds all of the above.
