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
 * @file sglcommutation.h
 * @brief Commutation relations.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "gammaindex.h"

namespace sgl {

struct Commutation {
    GExpr      factor;
    GammaIndex left;
    GammaIndex right;
    GExpr      remnant;
};

std::ostream &operator<<(std::ostream &out, Commutation const &com);

Commutation commute(GammaIndex const &mu,
                    GammaIndex const &nu,
                    csl::Index const &a,
                    csl::Index const &b,
                    bool              inverseCall = false);
} // namespace sgl
