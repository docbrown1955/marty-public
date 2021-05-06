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

 #pragma once

#include "abstractgammasym.h"

namespace sgl {

    struct TensorSet;

    GExpr csl_simplified(
            GExpr const &expr,
            TensorSet const &tensors
            );

    csl::Expr sgl_to_csl(
            GExpr const &expr,
            TensorSet const &tensors
            );

    GExpr csl_to_sgl(
            csl::Expr const &expr,
            TensorSet const &tensors
            );
}
