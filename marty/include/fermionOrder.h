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
 * @file fermionOrder.h
 * @brief Utilities for fermion ordering in amplitudes.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include <vector>

namespace csl { class Expr; }

namespace mty {

    class Insertion;
    class QuantumField;

    bool ordered(
            mty::Insertion const &left, 
            mty::Insertion const &right
            );

    std::vector<mty::Insertion const*> fermionsOf(
            std::vector<mty::Insertion> const &fields
            );

    std::vector<mty::Insertion*> fermionsOf(
            std::vector<mty::Insertion> &fields
            );

    std::vector<int> defaultFermionOrder(
            std::vector<mty::Insertion const*> const &fields
            );

    inline std::vector<int> defaultFermionOrder(
            std::vector<mty::Insertion> const &fields
            )
    {
        return defaultFermionOrder(fermionsOf(fields));
    }

    void applyFermionOrder(
            std::vector<mty::Insertion> &insertions,
            std::vector<int>      const &order
            );

    void applyDefaultFermionOrder(std::vector<mty::Insertion> &insertions);

}
