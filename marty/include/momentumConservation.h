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
 * @file momentumConservation.h
 * @brief Helper class mty::MomentumConservater to apply momentum conservation 
 * in amplitudes to simplify them.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include <vector>
#include "quantumField.h"

namespace mty {

    class MomentumConservater {

    public:

        MomentumConservater(
                std::vector<mty::QuantumField> const &insertions,
                std::vector<csl::Tensor>       const &t_momenta
                );
        bool apply(csl::Expr &expr);

    private:

        int findMomentum(csl::Expr const &sub);
        std::vector<csl::Expr> isMomentumSum(csl::Expr const &sub);
        std::optional<csl::Expr> applyOnMomentumSum(
                std::vector<csl::Expr> &factors
                );

        std::pair<int, csl::Expr> getMomentumStructure(csl::Expr const &term);
        bool simplify(std::vector<csl::Expr> &factors);
        csl::Expr recoverExpr(std::vector<csl::Expr> const &factors);

    private:

        std::vector<bool>        signs;
        std::vector<csl::Tensor> momenta;
        std::size_t                   posSimplestMomenta;
        csl::Index               mu;
    };
}
