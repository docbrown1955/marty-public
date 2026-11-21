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
 * @file symbolic_transform.cpp
 * @brief Transformations, evaluation and conjugation of the CSL API facade.
 */

#include "csl.h"
#include "csl/api/error.h"
#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/api/transform.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using csl::api::ExprHandle;

namespace {

ExprHandle num(long long value)
{
    return ExprHandle::integer(value);
}

} // namespace

TEST(SymbolicApi, ExpandBinomial)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");

    auto expected = csl::api::power(x, num(2)) + num(2) * x * y
                    + csl::api::power(y, num(2));
    auto result   = csl::api::expanded(csl::api::power(x + y, num(2)));
    EXPECT_TRUE(result.equals(expected));
}

TEST(SymbolicApi, FactorCommonFactor)
{
    auto a = ExprHandle::variable("a");
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");

    auto result = csl::api::factored(a * x + a * y);
    EXPECT_TRUE(result.equals(a * (x + y)));
}

TEST(SymbolicApi, FactorNoCommonFactorIsIdentity)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");

    EXPECT_TRUE(csl::api::factored(x + y).equals(x + y));
}

TEST(SymbolicApi, CollectBySymbol)
{
    auto a = ExprHandle::variable("a");
    auto b = ExprHandle::variable("b");
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");

    auto result = csl::api::collected(a * x + b * x + y, {x});
    EXPECT_TRUE(result.equals((a + b) * x + y));
}

TEST(SymbolicApi, CollectIndexedFactorThrows)
{
    csl::Space  V{"V", 3};
    csl::Tensor y{"y", &V};
    csl::Index  i = V.generateIndex("i");

    ExprHandle yi{y(std::vector<csl::Index>{i})};
    auto       x = ExprHandle::variable("x");
    auto       z = ExprHandle::variable("z");

    EXPECT_THROW(csl::api::collected(x * yi + z * yi, {yi}),
                 csl::api::TypeError);
}

TEST(SymbolicApi, DiffPolynomial)
{
    // y is a constant: a CSL variable may depend on other variables.
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::constant("y");

    auto e        = csl::api::power(x, num(3)) + num(2) * x * y;
    auto expected = num(3) * csl::api::power(x, num(2)) + num(2) * y;
    EXPECT_TRUE(csl::api::derived(e, x).equals(expected));
}

TEST(SymbolicApi, DiffChainRule)
{
    auto x = ExprHandle::variable("x");

    auto inner    = csl::api::power(x, num(2));
    auto e        = csl::api::Sin(inner);
    auto expected = csl::api::Cos(inner) * num(2) * x;
    EXPECT_TRUE(csl::api::derived(e, x).equals(expected));
}

TEST(SymbolicApi, SubsSymbolSwap)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");

    auto result = csl::api::replaced(x + num(2) * y, {{x, y}, {y, x}});
    EXPECT_TRUE(result.equals(y + num(2) * x));
}

TEST(SymbolicApi, SubsDoesNotMutateOriginal)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    auto z = ExprHandle::variable("z");

    auto       e        = x + y;
    auto const snapshot = csl::api::text(e);

    auto result = csl::api::replaced(e, {{x, z}});
    EXPECT_TRUE(result.equals(z + y));
    EXPECT_TRUE(e.equals(x + y));
    EXPECT_EQ(csl::api::text(e), snapshot);
}

TEST(SymbolicApi, SubsCompoundKeyBehaviourPinned)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    auto z = ExprHandle::variable("z");
    auto w = ExprHandle::variable("w");

    // Compound keys are accepted by the facade: the replacement is done
    // structurally by CSL. The Python layer cannot pass them (not hashable).
    auto result = csl::api::replaced(x * y + w, {{x * y, z}});
    EXPECT_TRUE(result.equals(z + w));
}

TEST(SymbolicApi, EvaluateWithValues)
{
    auto x = ExprHandle::variable("x");

    auto result = csl::api::evaluated(csl::api::power(x, num(2)) + num(1),
                                      {{x, num(3)}});
    EXPECT_TRUE(result.equals(num(10)));
}

TEST(SymbolicApi, EvaluateWithFreeSymbolsKeepsThem)
{
    // y is a constant: CSL variables depend on each other by default.
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::constant("y");

    auto result
        = csl::api::evaluated(csl::api::power(x, num(2)) + y, {{x, num(2)}});
    EXPECT_TRUE(result.equals(num(4) + y));
    EXPECT_TRUE(csl::api::dependsOn(result, y));
    EXPECT_FALSE(csl::api::dependsOn(result, x));
}

TEST(SymbolicApi, EvaluateAmbiguousNameRaises)
{
    auto x1 = ExprHandle::variable("x");
    auto x2 = ExprHandle::variable("x");
    auto y  = ExprHandle::variable("y");

    EXPECT_THROW(csl::api::evaluatedByName(x1 + x2, {{"x", num(2)}}),
                 csl::api::ValueError);

    auto result = csl::api::evaluatedByName(x1 + y, {{"x", num(2)}});
    EXPECT_TRUE(result.equals(num(2) + y));
}

TEST(SymbolicApi, ConjugateInvolution)
{
    auto a = ExprHandle::variable("a");
    auto b = ExprHandle::variable("b");

    auto e = a + ExprHandle::imaginaryUnit() * b;
    EXPECT_TRUE(csl::api::conjugated(csl::api::conjugated(e)).equals(e));
}

TEST(SymbolicApi, ConjugateOfImaginaryUnit)
{
    EXPECT_TRUE(csl::api::conjugated(ExprHandle::imaginaryUnit())
                    .equals(-ExprHandle::imaginaryUnit()));
}

TEST(SymbolicApi, RealPartOfRealSymbol)
{
    auto x = ExprHandle::variable("x");

    EXPECT_TRUE(csl::api::realPart(x).equals(x));
}

TEST(SymbolicApi, ImagPartOfRealSymbol)
{
    auto x = ExprHandle::variable("x");

    EXPECT_TRUE(csl::api::imagPart(x).equals(num(0)));
}

TEST(SymbolicApi, EvaluateAllExpandsAbbreviations)
{
    auto a  = ExprHandle::variable("a");
    auto b  = ExprHandle::variable("b");
    auto ab = csl::api::makeAbbreviation("Tab", a * b);
    EXPECT_TRUE(csl::api::evaluated(num(3) * ab, {}, csl::api::EvalMode::All)
                    .equals(num(3) * a * b));
}

TEST(SymbolicApi, EvaluateAbbreviationModeExpandsNestedAbbreviations)
{
    auto a     = ExprHandle::variable("a");
    auto b     = ExprHandle::variable("b");
    auto inner = csl::api::makeAbbreviation("Tin", a + b);
    auto outer = csl::api::makeAbbreviation("Tout", num(2) * inner);
    EXPECT_TRUE(
        csl::api::evaluated(outer, {}, csl::api::EvalMode::Abbreviation)
            .equals(num(2) * (a + b)));
}

TEST(SymbolicApi, EvaluateBaseModeKeepsAbbreviations)
{
    auto a  = ExprHandle::variable("a");
    auto b  = ExprHandle::variable("b");
    auto ab = csl::api::makeAbbreviation("Tkeep", a * b);
    EXPECT_TRUE(
        csl::api::evaluated(ab, {}, csl::api::EvalMode::Base).equals(ab));
}
