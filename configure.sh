#!/bin/sh

EXTRA_OPTS="$*"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./.install "$EXTRA_OPTS"
