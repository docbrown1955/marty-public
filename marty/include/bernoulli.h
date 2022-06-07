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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * \brief
 */
#ifndef BERNOULLI_H_INCLUDED
#define BERNOULLI_H_INCLUDED

#include <csl.h>
#include "mrtError.h"

namespace mty {

inline csl::Expr bernoulliCombinatorial(std::size_t i, std::size_t j)
{
    if (j < i - j)
        return bernoulliCombinatorial(i, i - j);
    csl::Expr rational = CSL_1;
    std::size_t denom_j = i - j;
    for (std::size_t num_i = i; num_i > j; --num_i) {
        rational *= num_i;
        if (denom_j > 0)
            rational /= denom_j--;
    }
    return rational;
}

inline csl::Expr bernoulliRecursion(
        std::vector<csl::Expr> const &B,
        std::size_t                   n
        )
{
    std::vector<csl::Expr> terms(n);
    for (std::size_t i = 0; i != n; ++i)
        terms[i] = bernoulliCombinatorial(n, i) * B[i] / (n - i + 1);
    return -csl::sum_s(terms);
}

// Definition of B- found in
// https://en.wikipedia.org/wiki/Bernoulli_number
inline csl::Expr bernoulliNumber(std::size_t i)
{
    static constexpr std::size_t maxBernouilli = 25;
    HEPAssert(i <= maxBernouilli,
            mty::error::ValueError,
            "Asking Bernouilli number " + toString(i) + ", but cannot go "
            "beyond " + toString(maxBernouilli))
    static std::vector<csl::Expr> B = {1};
    if (i < B.size())
        return B[i];
    for (std::size_t j = B.size(); j <= i; ++j)
        B.push_back(bernoulliRecursion(B, j));

    return B.back();
}

}

#endif
