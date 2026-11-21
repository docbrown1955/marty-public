// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

/// Conversions between Python `Index` objects (symbolic.cpp) and CSL indices,
/// shared with the modeling bindings.

#ifndef PYMARTY_SYMBOLIC_BRIDGE_H_INCLUDED
#define PYMARTY_SYMBOLIC_BRIDGE_H_INCLUDED

#include "csl/index.h"

#include <pybind11/pybind11.h>

/// CSL index of a Python `Index`. Raises `TypeError` for other objects.
csl::Index coreIndex(pybind11::handle value);

/// Python `Index` for a CSL index, without owner space (as for the indices of
/// an expression).
pybind11::object pyIndex(csl::Index const &index);

#endif
