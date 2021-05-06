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

 #include "quantumField.h"
#include "fermionOrder.h"
#include "insertion.h"
#include "mrtError.h"
#include <numeric>

namespace mty {

    bool ordered(
            mty::Insertion const &left,
            mty::Insertion const &right
            )
    {
        auto shouldBeFirst = [](mty::Insertion const &field) {
            return field.isIncomingAntiParticle()
                || field.isOutgoingParticle();
        };
        const bool leftFirst  = shouldBeFirst(left);
        const bool rightFirst = shouldBeFirst(right);
        if (leftFirst ^ rightFirst) // Standard situation, conjugate on the left
            return leftFirst;
        const bool leftMajorana  = left.getField()->isSelfConjugate();
        const bool rightMajorana = right.getField()->isSelfConjugate();
        if (leftMajorana ^ rightMajorana) // Non-majorana in priority
            return !leftMajorana;

        // If no field is lower than the other there is no order
        return false;
    }

    std::vector<mty::Insertion*> fermionsOf(
            std::vector<mty::Insertion> &fields
            )
    {
        std::vector<mty::Insertion*> qfields;
        qfields.reserve(fields.size());
        for (auto &f : fields) {
            if (f.getField()->isFermionic())
                qfields.push_back(&f);
        }

        return qfields;
    }

    void applyGoodAdjacentOrder(
            std::vector<int>    &numbers,
            std::vector<size_t> &indicesLeft,
            std::vector<mty::Insertion*> const &fermions
            )
    {
        for (size_t i = 0; i != indicesLeft.size(); i += 2) {
            const size_t index = indicesLeft[i];
            auto const &left  = *fermions[numbers[index]];
            auto const &right = *fermions[numbers[index+1]];
            const auto firstLeft = begin(indicesLeft);
            if (ordered(left, right)) {
                indicesLeft.erase(firstLeft + i, firstLeft + i + 2);
                i -= 2;
            }
            else if (ordered(right, left)) {
                std::swap(numbers[index], numbers[index+1]);
                indicesLeft.erase(firstLeft + i, firstLeft + i + 2);
                i -= 2;
            }
        } 
    }

    void applyGoodNonAdjacentOrder(
            std::vector<int>    &numbers,
            std::vector<size_t> &indicesLeft,
            std::vector<mty::Insertion*> const &fermions
            )
    {
        for (size_t i = 0; i != indicesLeft.size(); i += 2) {
            const size_t index_i = indicesLeft[i];
            auto const &left  = *fermions[numbers[index_i]];
            for (size_t j = i + 2; j != indicesLeft.size(); ++j) {
                const size_t index_j = indicesLeft[j];
                auto const &right = *fermions[numbers[index_j]];
                auto const firstLeft = begin(indicesLeft);
                if (ordered(left, right)) {
                    std::swap(numbers[index_i+1], numbers[index_j]);
                    indicesLeft.erase(firstLeft + i);
                    indicesLeft.erase(firstLeft + j);
                    i -= 2;
                    break;
                }
                else if (ordered(right, left)) {
                    std::swap(numbers[index_i+1], numbers[index_j]);
                    std::swap(numbers[index_i], numbers[index_i+1]);
                    indicesLeft.erase(firstLeft + i);
                    indicesLeft.erase(firstLeft + j);
                    i -= 2;
                    break;
                }
            }
        } 
    }

    std::vector<int> defaultFermionOrder(
           std::vector<mty::Insertion*> const &fields
           )
    {
        std::vector<int> numbers(fields.size());
        std::iota(begin(numbers), end(numbers), 0);
        if (fields.size() & 1)
            return numbers;
        std::vector<size_t> indicesLeft(fields.size());
        std::iota(begin(indicesLeft), end(indicesLeft), 0);
        applyGoodAdjacentOrder(numbers, indicesLeft, fields);
        applyGoodNonAdjacentOrder(numbers, indicesLeft, fields);

        return numbers;
    }

    void applyFermionOrder(
            std::vector<mty::Insertion> &insertions,
            std::vector<int>      const &order
            )
    {
        std::vector<mty::Insertion*> fermions = fermionsOf(insertions);
        HEPAssert(fermions.size() == order.size(),
                mty::error::ValueError,
                "Found " + toString(fermions.size()) + " for a fermion ordering"
                " of " + toString(order.size()) + " fermions.")
        for (size_t i = 0; i != order.size()/2; ++i) {
            const int i1 = order[2*i];
            const int i2 = order[2*i + 1];
            HEPAssert(i1 >= 0 && i1 < static_cast<int>(fermions.size()),
                    mty::error::ValueError,
                    "Fermion order index " + std::to_string(i1) 
                    + " out of bounds for external fermions (size "
                    + std::to_string(fermions.size()) + ")")
            HEPAssert(i2 >= 0 && i2 < static_cast<int>(fermions.size()),
                    mty::error::ValueError,
                    "Fermion order index " + std::to_string(i2) 
                    + " out of bounds for external fermions (size "
                    + std::to_string(fermions.size()) + ")")
            fermions[i1]->setPartnerShip({i1, i2, true});
            fermions[i2]->setPartnerShip({i2, i1, false});
        }
    }

    void applyDefaultFermionOrder(std::vector<mty::Insertion> &insertions)
    {
        applyFermionOrder(
                insertions,
                defaultFermionOrder(fermionsOf(insertions))
                );
    }
}
