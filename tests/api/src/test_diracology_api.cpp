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

#include "marty/api/common/error.h"
#include "marty/api/diracology.h"
#include "marty/sgl/sgloptions.h"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

namespace dirac = mty::api::diracology;

TEST(DiracologyApi, NegativeIndicesAreValueErrors)
{
    EXPECT_THROW(dirac::gamma(-1), mty::api::ValueError);
    EXPECT_THROW(dirac::g(0, -2), mty::api::ValueError);
    EXPECT_THROW(dirac::eps(0, 1, 2, -3), mty::api::ValueError);
    EXPECT_THROW(dirac::current({dirac::gamma(0)}, -1, 2),
                 mty::api::ValueError);
}


TEST(DiracologyApi, InvalidFierzInputIsValueError)
{
    EXPECT_THROW(dirac::singleFierz(dirac::gamma(0), dirac::gamma(1)),
                 mty::api::ValueError);
}

TEST(DiracologyApi, ArgumentOutOfRangeIsLookupError)
{
    auto atom = dirac::gamma(0);
    EXPECT_THROW(dirac::argument(atom, dirac::size(atom) + 1),
                 mty::api::LookupError);
}

TEST(DiracologyApi, EqualityIsCanonical)
{
    auto cur = [](int iL, int iR) {
        return dirac::current({dirac::gamma(0)}, iL, iR);
    };
    EXPECT_TRUE(dirac::equals(cur(1, 2), dirac::deepCopy(cur(1, 2))));
    EXPECT_FALSE(dirac::equals(cur(1, 2), dirac::scale(cur(1, 2), dirac::fromInt(2))));
    // Exactly one of the two orderings holds for distinct expressions.
    EXPECT_NE(dirac::less(cur(1, 2), dirac::scale(cur(1, 2), dirac::fromInt(2))),
              dirac::less(dirac::scale(cur(1, 2), dirac::fromInt(2)), cur(1, 2)));
}


TEST(DiracologyApi, OrderIsIdempotent)
{
    auto chain = dirac::current(
        {dirac::gamma(0), dirac::gamma(1), dirac::gamma(2)}, 1, 2);
    auto once  = dirac::ordered(chain);
    auto twice = dirac::ordered(once);
    EXPECT_TRUE(dirac::equals(once, twice));
}

TEST(DiracologyApi, SymbolicDimensionIsRestoredOnException)
{
    bool const before = sgl::option::keepEvanescentOperators;
    try {
        dirac::ScopedSymbolicDimension guard(!before);
        EXPECT_EQ(sgl::option::keepEvanescentOperators, !before);
        throw std::runtime_error("failure inside the guard");
    }
    catch (std::runtime_error const &) {
    }
    EXPECT_EQ(sgl::option::keepEvanescentOperators, before);
}

TEST(DiracologyApi, InvalidScalarsAreValueErrors)
{
    EXPECT_THROW(dirac::fromRational(1, 0), mty::api::ValueError);
    EXPECT_THROW(dirac::fromDouble(std::numeric_limits<double>::quiet_NaN()),
                 mty::api::ValueError);
}
