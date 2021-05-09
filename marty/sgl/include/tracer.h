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

/**
 * @file tracer.h
 * @brief Trace identities for gamma matrices.
 * @author Grégoire Uhlrich
 * @version 
 * @date 2021-05-06
 */
#pragma once

#include "abstractgammasym.h"

namespace sgl {
    class GammaIndex;
}

namespace sgl::tracer {

    /**
     * @brief Calculates the traces of gamma matrices without chiral operator.
     *
     * @param indices Set of generalized indices representing gamma matrices.
     * @param Ddirac  Dirac space dimension.
     *
     * @return The expression corresponding to the trace.
     */
    GExpr standardTrace(
            std::vector<GammaIndex> const &indices,
            csl::Expr               const &Ddirac
            );

    /**
     * @brief Calculates the traces of gamma matrices with the chiral operator.
     *
     * @param indices Set of generalized indices representing gamma matrices.
     * @param Ddirac  Dirac space dimension.
     *
     * @return The expression corresponding to the trace.
     */
    GExpr chiralTrace(
            std::vector<GammaIndex> const &indices,
            csl::Expr               const &Ddirac
            );
}
