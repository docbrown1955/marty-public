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

#include <vector>

namespace csl { class Expr; }

namespace mty {

    class Insertion;
    class QuantumField;

    bool ordered(
            mty::Insertion const &left, 
            mty::Insertion const &right
            );

    std::vector<mty::Insertion*> fermionsOf(
            std::vector<mty::Insertion> &fields
            );

    std::vector<int> defaultFermionOrder(
            std::vector<mty::Insertion*> const &fields
            );

    inline std::vector<int> defaultFermionOrder(
            std::vector<mty::Insertion> &fields
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