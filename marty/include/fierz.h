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

#include "csl.h"

namespace mty::fierz {

    enum FierzMode {
        Left,
        Right
    };

    bool isFermionic(csl::Expr const &arg);
    std::vector<csl::Index> spinorIndex(
            csl::Expr const &prod,
            FierzMode        mode
            );

    size_t nBasisElement();

    std::pair<csl::Expr, csl::Expr> basisElements(
            size_t            i,
            csl::Index const &a,
            csl::Index const &b,
            csl::Index const &c,
            csl::Index const &d
            );

    void insertIdentity(
            csl::Expr        &prod,
            csl::Index const &a1,
            csl::Index const &a2,
            csl::Index const &b1,
            csl::Index const &b2
            );

    void doApplyFierz(
            csl::Expr &prod,
            csl::Index const &alpha,
            csl::Index const &beta,
            FierzMode         mode
            );

    void applyFierz(
            csl::Expr &prod, 
            FierzMode  mode
            );

} // namespace mty::fierz
