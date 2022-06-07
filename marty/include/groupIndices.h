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
#ifndef GROUP_INDICES_H_INCLUDED
#define GROUP_INDICES_H_INCLUDED

#include <array>
#include <vector>
#include "mrtError.h"
#include "mrtUtils.h"
#include "group.h"

namespace mty {

template<class Type, std::size_t N>
    std::array<Type, N> toArray(
            std::vector<Type> const &vec
            )
    {
        HEPAssert(vec.size() == N,
                mty::error::TypeError,
                "Expecting a vector of size "  + toString(N) + " in conversion"
                ", vector of size " + toString(vec.size()) + " given.");
        std::array<Type, N> res;
        std::copy(vec.begin(), vec.end(), res.begin());
        return res;
    }

bool isIndexZeroSU(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroEvenSO(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroOddSO(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroSp(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroE6(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroE7(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroE8(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroF4(
        std::vector<int> const &rep,
        std::size_t p
        );
bool isIndexZeroG2(
        std::vector<int> const &rep,
        std::size_t p
        );

csl::Expr getIndexNorm(
        algebra::Type    algebra,
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getSUIndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getEvenSOIndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getOddSOIndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getSpIndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getE6IndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getE7IndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getE8IndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getF4IndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getG2IndexNorm(
        std::vector<int> const &rep,
        std::size_t                  n
        );

csl::Expr getIndex(
        algebra::Type    algebra,
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getSUIndex(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getEvenSOIndex(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getOddSOIndex(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getLastEvenSOIndex(
        std::vector<int> const &rep,
        std::size_t                  n,
        bool                    normalize = true
        );
csl::Expr getSpIndex(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getE6Index(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getE7Index(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getE8Index(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getF4Index(
        std::vector<int> const &rep,
        std::size_t                  n
        );
csl::Expr getG2Index(
        std::vector<int> const &rep,
        std::size_t                  n
        );

// All the following indices are taken from
// Source : arXiv:hep-ph/9802376v1 (20 feb 1998)
// Further indices have not been taken into account here but appear in the
// mentionned paper (could be therefore added if needed):
// For E7: I_14 and I_18 
// For E8: I_14, I_18, I_20, I_24 and I_30

///////////////////////////////////////////////////
/*************************************************/
// E6 indices                                    //
/*************************************************/
///////////////////////////////////////////////////

template<std::size_t N>
using GIndex_t = std::pair<std::array<int, N>, long long int>;

inline constexpr std::array<GIndex_t<6>, 12>
    I2_E6_data =
    {
        GIndex_t<6>({ {1, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 1}, 4 }),
        GIndex_t<6>({ {0, 1, 0, 0, 0, 0}, 25 }),
        GIndex_t<6>({ {0, 0, 0, 1, 0, 0}, 25 }),
        GIndex_t<6>({ {0, 0, 0, 0, 2, 0}, 28 }),
        GIndex_t<6>({ {2, 0, 0, 0, 0, 0}, 28 }),
        GIndex_t<6>({ {1, 0, 0, 0, 1, 0}, 50 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 1}, 160 }),
        GIndex_t<6>({ {1, 0, 0, 0, 0, 1}, 160 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 2}, 270 }),
        GIndex_t<6>({ {0, 0, 1, 0, 0, 0}, 300 })
    };

inline constexpr std::array<GIndex_t<6>, 12>
    I5_E6_data =
    {
        GIndex_t<6>({ {1, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 0}, -1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 1}, 0 }),
        GIndex_t<6>({ {0, 1, 0, 0, 0, 0}, 11 }),
        GIndex_t<6>({ {0, 0, 0, 1, 0, 0}, -11 }),
        GIndex_t<6>({ {0, 0, 0, 0, 2, 0}, -44 }),
        GIndex_t<6>({ {2, 0, 0, 0, 0, 0}, 44 }),
        GIndex_t<6>({ {1, 0, 0, 0, 1, 0}, 0 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 1}, -88 }),
        GIndex_t<6>({ {1, 0, 0, 0, 0, 1}, 88 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 2}, 0 }),
        GIndex_t<6>({ {0, 0, 1, 0, 0, 0}, 0 })
    };

inline constexpr std::array<GIndex_t<6>, 12>
    I6_E6_data =
    {
        GIndex_t<6>({ {1, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 1}, -6 }),
        GIndex_t<6>({ {0, 1, 0, 0, 0, 0}, -5 }),
        GIndex_t<6>({ {0, 0, 0, 1, 0, 0}, -5 }),
        GIndex_t<6>({ {0, 0, 0, 0, 2, 0}, 58 }),
        GIndex_t<6>({ {2, 0, 0, 0, 0, 0}, 58 }),
        GIndex_t<6>({ {1, 0, 0, 0, 1, 0}, 60 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 1}, -80 }),
        GIndex_t<6>({ {1, 0, 0, 0, 0, 1}, -80 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 2}, -720 }),
        GIndex_t<6>({ {0, 0, 1, 0, 0, 0}, -270 })
    };

inline constexpr std::array<GIndex_t<6>, 12>
    I8_E6_data =
    {
        GIndex_t<6>({ {1, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 1}, 18 }),
        GIndex_t<6>({ {0, 1, 0, 0, 0, 0}, -101 }),
        GIndex_t<6>({ {0, 0, 0, 1, 0, 0}, -101 }),
        GIndex_t<6>({ {0, 0, 0, 0, 2, 0}, 154 }),
        GIndex_t<6>({ {2, 0, 0, 0, 0, 0}, 154 }),
        GIndex_t<6>({ {1, 0, 0, 0, 1, 0}, 36 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 1}, 664 }),
        GIndex_t<6>({ {1, 0, 0, 0, 0, 1}, 664 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 2}, 3672 }),
        GIndex_t<6>({ {0, 0, 1, 0, 0, 0}, -918 })
    };

inline constexpr std::array<GIndex_t<6>, 12>
    I9_E6_data =
    {
        GIndex_t<6>({ {1, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 0}, -1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 1}, 0 }),
        GIndex_t<6>({ {0, 1, 0, 0, 0, 0}, -229 }),
        GIndex_t<6>({ {0, 0, 0, 1, 0, 0}, 229 }),
        GIndex_t<6>({ {0, 0, 0, 0, 2, 0}, -284 }),
        GIndex_t<6>({ {2, 0, 0, 0, 0, 0}, 284 }),
        GIndex_t<6>({ {1, 0, 0, 0, 1, 0}, 0 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 1}, 152 }),
        GIndex_t<6>({ {1, 0, 0, 0, 0, 1}, -152 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 2}, 0 }),
        GIndex_t<6>({ {0, 0, 1, 0, 0, 0}, 0 })
    };

inline constexpr std::array<GIndex_t<6>, 12>
    I12_E6_data =
    {
        GIndex_t<6>({ {1, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 1}, -62 }),
        GIndex_t<6>({ {0, 1, 0, 0, 0, 0}, -2021 }),
        GIndex_t<6>({ {0, 0, 0, 1, 0, 0}, -2021 }),
        GIndex_t<6>({ {0, 0, 0, 0, 2, 0}, 2074 }),
        GIndex_t<6>({ {2, 0, 0, 0, 0, 0}, 2074 }),
        GIndex_t<6>({ {1, 0, 0, 0, 1, 0}, 116 }),
        GIndex_t<6>({ {0, 0, 0, 0, 1, 1}, 424 }),
        GIndex_t<6>({ {1, 0, 0, 0, 0, 1}, 424 }),
        GIndex_t<6>({ {0, 0, 0, 0, 0, 2}, -131928 }),
        GIndex_t<6>({ {0, 0, 1, 0, 0, 0}, 122202 })
    };

///////////////////////////////////////////////////
/*************************************************/
// E7 indices                                    //
/*************************************************/
///////////////////////////////////////////////////

inline constexpr std::array<GIndex_t<7>, 21>
    I2_E7_data =
    {
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 0}, 3 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 1}, 30 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 2, 0}, 55 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 0, 0}, 54 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 1, 0}, 270 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 0, 0}, 351 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 0, 0}, 390 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 3, 0}, 1440 }),
        GIndex_t<7>({ {0, 0, 0, 1, 0, 0, 0}, 1430 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 1}, 2145 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 1, 0}, 2832 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 1}, 4995 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 2, 0}, 9450 }),
        GIndex_t<7>({ {1, 0, 0, 0, 1, 0, 0}, 9152 }),
        GIndex_t<7>({ {3, 0, 0, 0, 0, 0, 0}, 17940 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 2}, 17820 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 4, 0}, 24310 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 1, 0}, 21762 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 1, 0}, 23760 }),
        GIndex_t<7>({ {0, 0, 1, 0, 0, 0, 0}, 24750 })
    };

inline constexpr std::array<GIndex_t<7>, 21>
    I6_E7_data =
    {
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 0}, -2 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 1}, -10 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 2, 0}, 90 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 0, 0}, 24 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 1, 0}, 30 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 0, 0}, -354 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 0, 0}, -200 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 3, 0}, 3600 }),
        GIndex_t<7>({ {0, 0, 0, 1, 0, 0, 0}, -10 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 1}, 530 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 1, 0}, 2872 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 1}, -3165 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 2, 0}, 8400 }),
        GIndex_t<7>({ {1, 0, 0, 0, 1, 0, 0}, -328 }),
        GIndex_t<7>({ {3, 0, 0, 0, 0, 0, 0}, -26380 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 2}, -9000 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 4, 0}, 88400 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 1, 0}, -9318 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 1, 0}, 600 }),
        GIndex_t<7>({ {0, 0, 1, 0, 0, 0, 0}, -9000 })
    };

inline constexpr std::array<GIndex_t<7>, 21>
    I8_E7_data =
    {
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 0}, 10 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 1}, -82 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 2, 0}, 174 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 0, 0}, -72 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 1, 0}, 774 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 0, 0}, 2682 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 0, 0}, 40 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 3, 0}, 10176 }),
        GIndex_t<7>({ {0, 0, 0, 1, 0, 0, 0}, -3442 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 1}, -3658 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 1, 0}, 256 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 1}, 963 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 2, 0}, 41328 }),
        GIndex_t<7>({ {1, 0, 0, 0, 1, 0, 0}, 9320 }),
        GIndex_t<7>({ {3, 0, 0, 0, 0, 0, 0}, 271676 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 2}, -94824 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 4, 0}, 329888 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 1, 0}, 155826 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 1, 0}, 12672 }),
        GIndex_t<7>({ {0, 0, 1, 0, 0, 0, 0}, -63240 })
    };

inline constexpr std::array<GIndex_t<7>, 21>
    I10_E7_data =
    {
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 0}, -2 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 1}, 230 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 2, 0}, 570 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 0, 0}, -456 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 1, 0}, -210 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 0, 0}, -834 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 0, 0}, 760 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 3, 0}, 50160 }),
        GIndex_t<7>({ {0, 0, 0, 1, 0, 0, 0}, -7450 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 1}, 13490 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 1, 0}, -16568 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 1}, 36195 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 2, 0}, 59280 }),
        GIndex_t<7>({ {1, 0, 0, 0, 1, 0, 0}, -78088 }),
        GIndex_t<7>({ {3, 0, 0, 0, 0, 0, 0}, -116620 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 2}, 404280 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 4, 0}, 2153840 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 1, 0}, -75318 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 1, 0}, 22440 }),
        GIndex_t<7>({ {0, 0, 1, 0, 0, 0, 0}, 79800 })
    };

inline constexpr std::array<GIndex_t<7>, 21>
    I12_E7_data =
    {
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 0}, 1 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 0}, -30 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 1}, -2082 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 2, 0}, 2134 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 0, 0}, -1992 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 1, 0}, 534 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 0, 0}, -63438 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 0, 0}, 57480 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 3, 0}, 292896 }),
        GIndex_t<7>({ {0, 0, 0, 1, 0, 0, 0}, 63998 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 1, 1}, -171138 }),
        GIndex_t<7>({ {0, 0, 0, 0, 1, 1, 0}, -172464 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 0, 1}, -366717 }),
        GIndex_t<7>({ {1, 0, 0, 0, 0, 2, 0}, 410928 }),
        GIndex_t<7>({ {1, 0, 0, 0, 1, 0, 0}, -197560 }),
        GIndex_t<7>({ {3, 0, 0, 0, 0, 0, 0}, -13615284 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 0, 2}, -6024744 }),
        GIndex_t<7>({ {0, 0, 0, 0, 0, 4, 0}, 17066368 }),
        GIndex_t<7>({ {2, 0, 0, 0, 0, 1, 0}, -3178974 }),
        GIndex_t<7>({ {0, 1, 0, 0, 0, 1, 0}, 3531792 }),
        GIndex_t<7>({ {0, 0, 1, 0, 0, 0, 0}, 2601722601720 })
    };

///////////////////////////////////////////////////
/*************************************************/
// E8 indices                                    //
/*************************************************/
///////////////////////////////////////////////////

inline constexpr std::array<GIndex_t<8>, 29>
    I2_E8_data =
    {
        GIndex_t<8>({ {1, 0, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 1, 0}, 25 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 0, 0}, 225 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 0, 0}, 245 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 0, 1}, 1425 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 1, 0}, 8379 }),
        GIndex_t<8>({ {3, 0, 0, 0, 0, 0, 0}, 22750 }),
        GIndex_t<8>({ {0, 0, 1, 0, 0, 0, 0}, 29640 }),
        GIndex_t<8>({ {1, 1, 0, 0, 0, 0, 0}, 51200 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 2, 0}, 65610 }),
        GIndex_t<8>({ {0, 0, 0, 0, 1, 0, 0}, 88200 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 0, 1}, 372736 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 1, 0}, 1083000 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 1, 0}, 1148175 }),
        GIndex_t<8>({ {4, 0, 0, 0, 0, 0, 0}, 1404150 }),
        GIndex_t<8>({ {0, 0, 0, 1, 0, 0, 0}, 2360085 }),
        GIndex_t<8>({ {0, 2, 0, 0, 0, 0, 0}, 3441375 }),
        GIndex_t<8>({ {2, 1, 0, 0, 0, 0, 0}, 4843800 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 1, 1}, 5068800 }),
        GIndex_t<8>({ {1, 0, 1, 0, 0, 0, 0}, 5740875 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 2, 0}, 14773500 }),
        GIndex_t<8>({ {1, 0, 0, 0, 1, 0, 0}, 19426550 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 0, 1}, 40883535 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 0, 1}, 42214200 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 3, 0}, 60885500 }),
        GIndex_t<8>({ {3, 0, 0, 0, 0, 1, 0}, 79228100 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 0, 2}, 83281275 }),
        GIndex_t<8>({ {0, 0, 1, 0, 0, 1, 0}, 93400125 }),
        GIndex_t<8>({ {0, 0, 0, 0, 1, 0, 0}, 139094340 })
    };

inline constexpr std::array<GIndex_t<8>, 29>
    I8_E8_data =
    {
        GIndex_t<8>({ {1, 0, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 1, 0}, -17 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 0, 0}, 393 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 0, 0}, 119 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 0, 1}, -801 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 1, 0}, 357 }),
        GIndex_t<8>({ {3, 0, 0, 0, 0, 0, 0}, 64330 }),
        GIndex_t<8>({ {0, 0, 1, 0, 0, 0, 0}, 576 }),
        GIndex_t<8>({ {1, 1, 0, 0, 0, 0, 0}, 59264 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 2, 0}, -68202 }),
        GIndex_t<8>({ {0, 0, 0, 0, 1, 0, 0}, -64176 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 0, 1}, 12544 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 1, 0}, 991440 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 1, 0}, -64071 }),
        GIndex_t<8>({ {4, 0, 0, 0, 0, 0, 0}, 6100842 }),
        GIndex_t<8>({ {0, 0, 0, 1, 0, 0, 0}, -942669 }),
        GIndex_t<8>({ {0, 2, 0, 0, 0, 0, 0}, 3576615 }),
        GIndex_t<8>({ {2, 1, 0, 0, 0, 0, 0}, 10500696 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 1, 1}, -4540800 }),
        GIndex_t<8>({ {1, 0, 1, 0, 0, 0, 0}, 3591945 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 2, 0}, -7295820 }),
        GIndex_t<8>({ {1, 0, 0, 0, 1, 0, 0}, -3552406 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 0, 1}, 36197469 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 0, 1}, -2179296 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 3, 0}, -95237740 }),
        GIndex_t<8>({ {3, 0, 0, 0, 0, 1, 0}, 167887580 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 0, 2}, -77203203 }),
        GIndex_t<8>({ {0, 0, 1, 0, 0, 1, 0}, -36251565 }),
        GIndex_t<8>({ {0, 0, 0, 0, 1, 0, 0}, -107301348 })
    };

inline constexpr std::array<GIndex_t<8>, 29>
    I12_E8_data =
    {
        GIndex_t<8>({ {1, 0, 0, 0, 0, 0, 0}, 1 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 1, 0}, 223 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 0, 0}, 2073 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 0, 0}, -1801 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 0, 1}, -3921 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 1, 0}, 64677 }),
        GIndex_t<8>({ {3, 0, 0, 0, 0, 0, 0}, 653050 }),
        GIndex_t<8>({ {0, 0, 1, 0, 0, 0, 0}, -300624 }),
        GIndex_t<8>({ {1, 1, 0, 0, 0, 0, 0}, -176896 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 2, 0}, 1623078 }),
        GIndex_t<8>({ {0, 0, 0, 0, 1, 0, 0}, 344544 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 0, 1}, -928256 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 1, 0}, 15398400 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 1, 0}, 732969 }),
        GIndex_t<8>({ {4, 0, 0, 0, 0, 0, 0}, 97389402 }),
        GIndex_t<8>({ {0, 0, 0, 1, 0, 0, 0}, -20062029 }),
        GIndex_t<8>({ {0, 2, 0, 0, 0, 0, 0}, -53721225 }),
        GIndex_t<8>({ {2, 1, 0, 0, 0, 0, 0}, 50453496 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 1, 1}, 36284160 }),
        GIndex_t<8>({ {1, 0, 1, 0, 0, 0, 0}, -51773175 }),
        GIndex_t<8>({ {1, 0, 0, 0, 0, 2, 0}, 368355300 }),
        GIndex_t<8>({ {1, 0, 0, 0, 1, 0, 0}, 74388314 }),
        GIndex_t<8>({ {2, 0, 0, 0, 0, 0, 1}, 182867949 }),
        GIndex_t<8>({ {0, 1, 0, 0, 0, 0, 1}, -439704336 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 3, 0}, 3610174100 }),
        GIndex_t<8>({ {3, 0, 0, 0, 0, 1, 0}, 2727186380 }),
        GIndex_t<8>({ {0, 0, 0, 0, 0, 0, 2}, -459950403 }),
        GIndex_t<8>({ {0, 0, 1, 0, 0, 1, 0}, -180072525 }),
        GIndex_t<8>({ {0, 0, 0, 0, 1, 0, 0}, -484327668 })
    };

///////////////////////////////////////////////////
/*************************************************/
// F4 indices                                    //
/*************************************************/
///////////////////////////////////////////////////

inline constexpr std::array<GIndex_t<4>, 7>
    I2_F4_data =
    {
        GIndex_t<4>({ {0, 0, 0, 1}, 1 }),
        GIndex_t<4>({ {1, 0, 0, 0}, 3 }),
        GIndex_t<4>({ {0, 0, 1, 0}, 21 }),
        GIndex_t<4>({ {0, 0, 0, 2}, 27 }),
        GIndex_t<4>({ {1, 0, 0, 1}, 108 }),
        GIndex_t<4>({ {2, 0, 0, 0}, 135 }),
        GIndex_t<4>({ {0, 1, 0, 0}, 147 })
    };

inline constexpr std::array<GIndex_t<4>, 7>
    I6_F4_data =
    {
        GIndex_t<4>({ {0, 0, 0, 1}, 1 }),
        GIndex_t<4>({ {1, 0, 0, 0}, -7 }),
        GIndex_t<4>({ {0, 0, 1, 0}, 1 }),
        GIndex_t<4>({ {0, 0, 0, 2}, 57 }),
        GIndex_t<4>({ {1, 0, 0, 1}, -132 }),
        GIndex_t<4>({ {2, 0, 0, 0}, -645 }),
        GIndex_t<4>({ {0, 1, 0, 0}, -133 })
    };

inline constexpr std::array<GIndex_t<4>, 7>
    I8_F4_data =
    {
        GIndex_t<4>({ {0, 0, 0, 1}, 1 }),
        GIndex_t<4>({ {1, 0, 0, 0}, 17 }),
        GIndex_t<4>({ {0, 0, 1, 0}, -119 }),
        GIndex_t<4>({ {0, 0, 0, 2}, 153 }),
        GIndex_t<4>({ {1, 0, 0, 1}, 612 }),
        GIndex_t<4>({ {2, 0, 0, 0}, 2907 }),
        GIndex_t<4>({ {0, 1, 0, 0}, -1309 })
    };

inline constexpr std::array<GIndex_t<4>, 7>
    I12_F4_data =
    {
        GIndex_t<4>({ {0, 0, 0, 1}, 1 }),
        GIndex_t<4>({ {1, 0, 0, 0}, -63 }),
        GIndex_t<4>({ {0, 0, 1, 0}, -1959 }),
        GIndex_t<4>({ {0, 0, 0, 2}, 2073 }),
        GIndex_t<4>({ {1, 0, 0, 1}, 372 }),
        GIndex_t<4>({ {2, 0, 0, 0}, -134373 }),
        GIndex_t<4>({ {0, 1, 0, 0}, 125811 })
    };

///////////////////////////////////////////////////
/*************************************************/
// G2 indices                                    //
/*************************************************/
///////////////////////////////////////////////////

inline constexpr std::array<GIndex_t<2>, 16>
    I2_G2_data =
    {
        GIndex_t<2>({ {0, 1}, 1 }),
        GIndex_t<2>({ {1, 0}, 4 }),
        GIndex_t<2>({ {0, 2}, 9 }),
        GIndex_t<2>({ {1, 1}, 32 }),
        GIndex_t<2>({ {0, 3}, 44 }),
        GIndex_t<2>({ {2, 0}, 55 }),
        GIndex_t<2>({ {0, 4}, 156 }),
        GIndex_t<2>({ {1, 2}, 144 }),
        GIndex_t<2>({ {3, 0}, 351 }),
        GIndex_t<2>({ {2, 1}, 286 }),
        GIndex_t<2>({ {0, 5}, 450 }),
        GIndex_t<2>({ {1, 3}, 480 }),
        GIndex_t<2>({ {0, 6}, 1122 }),
        GIndex_t<2>({ {2, 2}, 972 }),
        GIndex_t<2>({ {4, 0}, 1496 }),
        GIndex_t<2>({ {3, 1}, 1472 })
    };

inline constexpr std::array<GIndex_t<2>, 16>
    I6_G2_data =
    {
        GIndex_t<2>({ {0, 1}, 1 }),
        GIndex_t<2>({ {1, 0}, -26 }),
        GIndex_t<2>({ {0, 2}, 39 }),
        GIndex_t<2>({ {1, 1}, -208 }),
        GIndex_t<2>({ {0, 3}, 494 }),
        GIndex_t<2>({ {2, 0}, -1235 }),
        GIndex_t<2>({ {0, 4}, 3666 }),
        GIndex_t<2>({ {1, 2}, -456 }),
        GIndex_t<2>({ {3, 0}, -20709 }),
        GIndex_t<2>({ {2, 1}, -7904 }),
        GIndex_t<2>({ {0, 5}, 19500 }),
        GIndex_t<2>({ {1, 3}, 2640 }),
        GIndex_t<2>({ {0, 6}, 82212 }),
        GIndex_t<2>({ {2, 2}, -27378 }),
        GIndex_t<2>({ {4, 0}, -193324 }),
        GIndex_t<2>({ {3, 1}, -109408 })
    };

} // End of namespace mty

#endif
