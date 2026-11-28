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
 * @file symbolic_printing.cpp
 * @brief Text and LaTeX output of the CSL API facade.
 *
 * The expected strings are the ones of tests/units/csl/test_latex.cpp.
 */

#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/api/transform.h"

#include <gtest/gtest.h>

#include <sstream>

using csl::api::ExprHandle;

namespace {

ExprHandle num(long long value)
{
    return ExprHandle::integer(value);
}

} // namespace

TEST(SymbolicApi, LatexMatchesExistingCslOutput)
{
    auto a = ExprHandle::variable("a");
    auto b = ExprHandle::variable("b");
    auto c = ExprHandle::variable("c");

    // Expected strings: test_latex.cpp, LatexLiterals.imaginaryUnit,
    // LatexSums.negativeTermsOfAnyShape, LatexProducts.dotBeforeBracketedSum,
    // LatexFractions.negativePowersAndDivision, LatexPowers.bracesAndBases,
    // LatexFunctions.commonFunctions.
    EXPECT_EQ(csl::api::latex(a + ExprHandle::imaginaryUnit() * b),
              "a + i\\,b");
    EXPECT_EQ(csl::api::latex(a - b / num(2)), "a - \\frac{b}{2}");
    EXPECT_EQ(csl::api::latex(a * (b + c)), "a\\cdot\\left(b + c\\right)");
    EXPECT_EQ(csl::api::latex(csl::api::power(a, num(-1))), "\\frac{1}{a}");
    EXPECT_EQ(csl::api::latex(csl::api::power(a + b, num(2))),
              "\\left(a + b\\right)^{2}");
    EXPECT_EQ(csl::api::latex(csl::api::Sqrt(a)), "\\sqrt{a}");
    EXPECT_EQ(csl::api::latex(csl::api::Exp(a + b)), "e^{a + b}");
    EXPECT_EQ(csl::api::latex(csl::api::Cos(a)), "\\cos\\left(a\\right)");
}

TEST(SymbolicApi, CodeOutputStable)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    auto e = csl::api::power(x + y, num(2)) - num(3) * x;

    auto const first  = csl::api::text(e);
    auto const second = csl::api::text(e);
    EXPECT_EQ(first, second);
    EXPECT_EQ(first, "(-3)*x + (x + y)^2\n");
    EXPECT_EQ(csl::api::latex(e), "-3x + \\left(x + y\\right)^{2}");

    std::ostringstream raw;
    e.raw()->print(0, raw);
    EXPECT_EQ(first, raw.str());
    EXPECT_EQ(csl::api::latex(e), e.raw()->printLaTeX(0));
}
