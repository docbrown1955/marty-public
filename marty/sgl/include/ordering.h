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

    class GammaIndex;

    bool freeOrder(
            std::vector<csl::Index> const &mu,
            std::vector<csl::Index> const &nu
            );

    bool contractedOrder(
            GExpr const &A,
            GExpr const &B
            );

    bool isOrdered(
            GExpr      const &prod,
            GammaIndex const &mu,
            GammaIndex const &nu
            );

    size_t simplest(
            GExpr const &prod,
            GExpr const &chain,
            size_t       begin = 0
            );
}