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
 * @file contraction.h
 * @brief Contraction relations.
 * @author Grégoire Uhlrich
 * @version 
 * @date 2021-05-06
 */
#pragma once

#include "gammaindex.h"

namespace sgl {

    class IndexChain;

    GExpr contraction(
            GammaIndex        mu,
            GammaIndex        nu,
            csl::Index const &a,
            csl::Index const &b
            );

    GExpr epsilonContraction(
            std::vector<csl::Index> A,
            std::vector<csl::Index> B
            );

    GExpr momentumGammaContraction(
            IndexChain  const &init,
            csl::Tensor        p,
            csl::Index  const &mu,
            csl::Index  const &nu,
            std::size_t             i,
            std::size_t             j
            );
}
