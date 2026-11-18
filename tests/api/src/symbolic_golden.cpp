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
 * @file symbolic_golden.cpp
 * @brief Golden set of printed expressions of the CSL API facade.
 *
 * Each expression is built with the facade and printed with csl::api::latex().
 * The expressions come from tests/units/csl/test_latex.cpp: the line of the
 * expected string is recorded next to each entry. The output is stored in
 * tests/api/data/symbolic/expressions.txt. With MARTY_UPDATE_GOLDEN=1 the file
 * is rewritten; otherwise it is compared line by line.
 */

#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/api/tensor.h"
#include "csl/api/transform.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using csl::api::ExprHandle;

namespace {

ExprHandle num(long long value)
{
    return ExprHandle::integer(value);
}

struct GoldenEntry {
    int        sourceLine; ///< Line of the expected string in test_latex.cpp.
    ExprHandle expr;
};

std::vector<GoldenEntry> goldenExpressions()
{
    auto a = ExprHandle::variable("a");
    auto b = ExprHandle::variable("b");
    auto c = ExprHandle::variable("c");
    auto d = ExprHandle::variable("d");
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    auto i = ExprHandle::imaginaryUnit();

    csl::api::SpaceHandle  V{"V", 3, {"i", "j"}};
    auto                   vi = V.index("i");
    auto                   vj = V.index("j");
    csl::api::TensorHandle yV{"y", {V}};
    csl::api::TensorHandle xV{"x", {V}};
    csl::api::TensorHandle MV{"M", {V, V}};
    csl::api::TensorHandle M1V{"M1;M_{1}", {V, V}};

    csl::api::SpaceHandle  S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}};
    csl::api::TensorHandle xS{"x", {S}};
    csl::api::TensorHandle yS{"y", {S}};
    csl::api::TensorHandle zS{"z", {S}};
    auto                   p1 = S.index("alpha");
    auto                   p2 = S.index("alpha");
    auto                   p3 = S.index("alpha");
    auto                   p  = S.index("alpha");
    auto                   f  = S.index("alpha");
    auto                   q  = S.index("beta");
    auto                   ka = S.index("alpha");
    auto                   kb = S.index("beta");

    auto M4 = csl::api::SpaceHandle::minkowski();
    auto mu = M4.index("\\mu");
    auto nu = M4.index("\\nu");

    csl::api::SpaceHandle  U{"U", 4, {"alpha", "beta"}};
    csl::api::TensorHandle xU{"x", {U}};
    auto                   ua = U.index("alpha");

    return {
        // LatexNumbers.integers
        {19, num(3)},
        {20, num(-3)},
        // LatexLiterals.variables
        {26, x},
        {27, ExprHandle::variable("M_W;M_{W}")},
        // LatexLiterals.imaginaryUnit
        {32, i},
        {33, num(2) * i},
        {34, a + i * b},
        // LatexSums.signs
        {41, a + b},
        {42, a - b},
        {43, a - num(2) * b},
        // LatexSums.negativeTermsOfAnyShape
        {51, a - b * c},
        {52, a - b / num(2)},
        {53, a - i * b},
        // LatexProducts.juxtapositionAndCoefficients
        {69, num(2) * a * b},
        {70, num(-2) * a},
        {71, num(-1) * a},
        // LatexProducts.dotBeforeBracketedSum
        {79, a * (b + c)},
        // LatexFractions.negativePowersAndDivision
        {87, csl::api::power(a, num(-1))},
        {88, a / b},
        {89, a / (b + c)},
        {90, num(2) * a / b},
        {91, a * c / b},
        // LatexFractions.rationalCoefficientGoesToDenominator
        {98, a / num(2)},
        {99, a / (num(2) * b)},
        // LatexNonCommuting.inversePowerOfCommutingFactorGoesToDenominator
        {106, x * csl::api::power(y, num(-1))},
        // LatexPowers.bracesAndBases
        {145, csl::api::power(a, num(2))},
        {146, csl::api::power(a, csl::api::power(b, c))},
        {147, csl::api::power(a + b, num(2))},
        {148, csl::api::power(a, num(-2))},
        {149, csl::api::Sqrt(a)},
        {150, csl::api::Sqrt(a + b)},
        // LatexFunctions.commonFunctions
        {157, csl::api::Abs(a)},
        {158, csl::api::Cos(a)},
        {159, csl::api::Exp(a + b)},
        // LatexMatrixChains.defaultSpaceUsesIndexNotation
        {216, yV({vi}) * MV({vi, vj})},
        // LatexMatrixChains.disabledSpaceFallsBackToIndexNotation
        {305, yV({vi}) * M1V({vi, vj}) * xV({vj})},
        // LatexMatrixChains.openChainFallsBackToIndexNotation
        {314, yV({vi}) * M1V({vi, vj})},
        // LatexTraces.disabledSpaceFallsBackToIndexNotation
        {367, M1V({vi, vi})},
        // LatexComposite.sumRaisedToPowerWithFraction
        {385, csl::api::power(a + csl::api::power(b, num(-1)), num(2))},
        // LatexComposite.coefficientFunctionsAndDenominator
        {394, num(2) * csl::api::Abs(a) * csl::api::Cos(b) / c},
        // LatexComposite.fractionInExponent
        {403, csl::api::power(a, b / c)},
        // LatexPrintLatex.dummyPairsAreRelabelledInOrder
        {458,
         csl::api::relabelIndices(xS({p1}) * yS({p1}) * xS({p2}) * yS({p2})
                                  * xS({p3}) * yS({p3}))},
        // LatexPrintLatex.freeIndexNamesAreSkipped
        {471, csl::api::relabelIndices(xS({p}) * yS({p}) * zS({f}))},
        // LatexPrintLatex.relabelDoesNotModifyTheInput
        {501, csl::api::relabelIndices(xS({p}) * yS({p}))},
        // LatexPrintLatex.freeIndicesAreRenamedToo
        {509, csl::api::relabelIndices(zS({q}))},
        // LatexNames.kroneckerDeltaHasLatexForm
        {517, csl::api::relabelIndices(csl::api::delta(ka, kb))},
        // LatexSignedIndices.metricWithEqualSignsIsG
        {562, csl::api::relabelIndices(csl::api::metric(mu, nu))},
        // LatexSignedIndices.freeDeltaIndicesAreRelabelledUpper
        {571, csl::api::relabelIndices(csl::api::delta(mu, nu.flipped()))},
        // LatexSignedIndices.unsignedSpaceKeepsSubscripts
        {580, xU({ua})},
        // LatexComposite.productOfSums
        {598, (a + b) * (c + d)},
    };
}

std::vector<std::string> const kHeader = {
    "# Golden set of the CSL API facade, LaTeX form (csl::api::latex).",
    "# One expression per line.",
    "# Lines starting with '#' are comments.",
    "# Expressions: tests/units/csl/test_latex.cpp.",
    "# Source line of each: tests/api/src/symbolic_golden.cpp.",
    "# Rewrite only for an intended change:",
    "# MARTY_UPDATE_GOLDEN=1 symbolic_golden.x",
};

} // namespace

TEST(SymbolicApi, GoldenExpressionsStable)
{
    std::string const path
        = std::string(MARTY_API_DATA_PATH) + "/symbolic/expressions.txt";

    std::vector<std::string> actual = kHeader;
    for (auto const &entry : goldenExpressions())
        actual.push_back(csl::api::latex(entry.expr));

    char const *update = std::getenv("MARTY_UPDATE_GOLDEN");
    if (update != nullptr && std::string(update) == "1") {
        std::filesystem::create_directories(
            std::filesystem::path(path).parent_path());
        std::ofstream out(path, std::ios::trunc);
        ASSERT_TRUE(out) << "cannot write " << path;
        for (auto const &line : actual)
            out << line << '\n';
        return;
    }

    std::vector<std::string> expected;
    std::ifstream            in(path);
    for (std::string line; std::getline(in, line);)
        expected.push_back(line);

    ASSERT_EQ(expected.size(), actual.size())
        << path << " must hold the header and one line per expression";
    for (std::size_t n = 0; n < actual.size(); ++n)
        EXPECT_EQ(expected[n], actual[n]) << path << ", line " << n + 1;
}
