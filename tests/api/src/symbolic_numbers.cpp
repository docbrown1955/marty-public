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
 * @file symbolic_numbers.cpp
 * @brief Numbers, operators and functions of the CSL API facade.
 */

#include "csl/api/error.h"
#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/api/transform.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>

using csl::api::ExprHandle;
using csl::api::Number;
using csl::api::NumberKind;

namespace {

/// Real value of a numerical expression, whatever its exact kind.
double realValueOf(ExprHandle const &expr)
{
    Number const value = csl::api::numericValue(expr);
    switch (value.kind) {
    case NumberKind::Integer:
        return static_cast<double>(value.num);
    case NumberKind::Rational:
        return static_cast<double>(value.num) / static_cast<double>(value.den);
    case NumberKind::Real:
    case NumberKind::Complex:
        return static_cast<double>(value.real);
    }
    return 0.0;
}

} // namespace

TEST(SymbolicApi, IntegerArithmeticExact)
{
    auto two   = ExprHandle::integer(2);
    auto three = ExprHandle::integer(3);
    EXPECT_TRUE((two + three).equals(ExprHandle::integer(5)));
    EXPECT_TRUE((two * three).equals(ExprHandle::integer(6)));
    EXPECT_TRUE((two - three).equals(ExprHandle::integer(-1)));
}

TEST(SymbolicApi, RationalNormalisation)
{
    EXPECT_TRUE(ExprHandle::rational(2, 4).equals(ExprHandle::rational(1, 2)));
    EXPECT_TRUE(ExprHandle::rational(4, 2).equals(ExprHandle::integer(2)));
    auto third = ExprHandle::rational(1, 3);
    auto value = third.number();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value->kind, NumberKind::Rational);
    EXPECT_EQ(value->num, 1);
    EXPECT_EQ(value->den, 3);
}

TEST(SymbolicApi, FloatContagion)
{
    auto sum = ExprHandle::integer(1) + ExprHandle::real(0.5L);
    EXPECT_TRUE(sum.equals(ExprHandle::real(1.5L)));
    auto value = sum.number();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value->kind, NumberKind::Real);
    EXPECT_DOUBLE_EQ(static_cast<double>(value->real), 1.5);
    EXPECT_TRUE(ExprHandle::real(2.0L).equals(ExprHandle::integer(2)));
}

TEST(SymbolicApi, ComplexNumbers)
{
    auto unit = ExprHandle::imaginaryUnit();
    auto kind = unit.number();
    ASSERT_TRUE(kind.has_value());
    EXPECT_EQ(kind->kind, NumberKind::Complex);
    EXPECT_TRUE(csl::api::power(unit, ExprHandle::integer(2))
                    .equals(ExprHandle::integer(-1)));

    auto z
        = ExprHandle::complex(ExprHandle::integer(1), ExprHandle::integer(2));
    auto value = z.number();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value->kind, NumberKind::Complex);
    EXPECT_DOUBLE_EQ(static_cast<double>(value->real), 1.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(value->imag), 2.0);

    EXPECT_TRUE(
        ExprHandle::complex(ExprHandle::integer(3), ExprHandle::integer(0))
            .equals(ExprHandle::integer(3)));
}

TEST(SymbolicApi, FunctionsSpecialValues)
{
    auto zero = ExprHandle::integer(0);
    auto one  = ExprHandle::integer(1);
    EXPECT_TRUE(csl::api::Sin(zero).equals(zero));
    EXPECT_TRUE(csl::api::Cos(zero).equals(one));
    EXPECT_TRUE(csl::api::Exp(zero).equals(one));
    EXPECT_TRUE(csl::api::Log(one).equals(zero));
    EXPECT_TRUE(
        csl::api::Sqrt(ExprHandle::integer(4)).equals(ExprHandle::integer(2)));
}

TEST(SymbolicApi, DivisionByZeroGivesInfinity)
{
    auto quotient = ExprHandle::integer(1) / ExprHandle::integer(0);
    EXPECT_TRUE(quotient.equals(ExprHandle::infinity()));
}

TEST(SymbolicApi, ZeroPowerNegativeRaises)
{
    EXPECT_THROW(
        csl::api::power(ExprHandle::integer(0), ExprHandle::integer(-1)),
        csl::api::ZeroDivisionError);
    EXPECT_THROW(
        csl::api::power(ExprHandle::infinity(), ExprHandle::integer(2)),
        csl::api::ValueError);
}

TEST(SymbolicApi, CSLErrorMessageNotLost)
{
    try {
        (void) csl::api::power(ExprHandle::integer(0),
                               ExprHandle::integer(-1));
        FAIL() << "expected csl::api::ZeroDivisionError";
    }
    catch (csl::api::ZeroDivisionError const &error) {
        std::string const message(error.what());
        EXPECT_FALSE(message.empty());
        EXPECT_NE(message.find("power"), std::string::npos);
        EXPECT_NE(message.find("division by zero"), std::string::npos);
    }
}

TEST(SymbolicApi, PiAndEConstants)
{
    auto pi = ExprHandle::pi();
    auto e  = ExprHandle::e();
    EXPECT_TRUE(pi.equals(ExprHandle::pi()));
    EXPECT_TRUE(e.equals(ExprHandle::e()));
    EXPECT_FALSE(pi.equals(e));
    EXPECT_NEAR(realValueOf(pi), std::numbers::pi, 1e-12);
    EXPECT_NEAR(realValueOf(e), std::numbers::e, 1e-12);
}

TEST(SymbolicApi, ModulusOfComplex)
{
    auto z
        = ExprHandle::complex(ExprHandle::integer(3), ExprHandle::integer(4));
    EXPECT_NEAR(realValueOf(csl::api::modulus(z)), 5.0, 1e-12);
}

TEST(SymbolicApi, ModulusOfRealIsAbsoluteValue)
{
    EXPECT_TRUE(csl::api::modulus(ExprHandle::integer(-2))
                    .equals(ExprHandle::integer(2)));
    EXPECT_TRUE(csl::api::modulus(ExprHandle::integer(3))
                    .equals(ExprHandle::integer(3)));
    EXPECT_TRUE(csl::api::modulus(ExprHandle::rational(-1, 2))
                    .equals(ExprHandle::rational(1, 2)));
    EXPECT_TRUE(csl::api::modulus(ExprHandle::integer(0))
                    .equals(ExprHandle::integer(0)));
}

TEST(SymbolicApi, ArgumentOfComplex)
{
    auto z
        = ExprHandle::complex(ExprHandle::integer(3), ExprHandle::integer(4));
    EXPECT_NEAR(
        realValueOf(csl::api::argument(z)), std::atan2(4.0, 3.0), 1e-12);
    EXPECT_NEAR(realValueOf(csl::api::argument(ExprHandle::integer(-2))),
                std::numbers::pi,
                1e-12);
}

TEST(SymbolicApi, AbsoluteValue)
{
    EXPECT_TRUE(
        csl::api::Abs(ExprHandle::integer(-3)).equals(ExprHandle::integer(3)));
    EXPECT_TRUE(
        csl::api::Abs(ExprHandle::integer(3)).equals(ExprHandle::integer(3)));
    EXPECT_TRUE(csl::api::Abs(ExprHandle::rational(-1, 2))
                    .equals(ExprHandle::rational(1, 2)));
}

TEST(SymbolicApi, ArgumentsOfSumAreTerms)
{
    auto x    = ExprHandle::variable("x");
    auto y    = ExprHandle::variable("y");
    auto args = (x + y).arguments();
    EXPECT_EQ(args.size(), 2u);
    EXPECT_TRUE(std::any_of(args.begin(),
                            args.end(),
                            [&](ExprHandle const &a) { return a.equals(x); }));
}

TEST(SymbolicApi, ArgumentOutOfRangeThrows)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    EXPECT_THROW((x * y).argument(2), csl::api::LookupError);
    EXPECT_TRUE(x.arguments().empty());
    EXPECT_THROW(x.argument(0), csl::api::LookupError);
}

TEST(SymbolicApi, OperandOfNonOperatorThrows)
{
    auto x = ExprHandle::variable("x");
    EXPECT_THROW(x.operand(), csl::api::TypeError);
}

TEST(SymbolicApi, NumericalFactorAndTerm)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    EXPECT_TRUE((ExprHandle::integer(3) * x)
                    .numericalFactor()
                    .equals(ExprHandle::integer(3)));
    EXPECT_TRUE((x + y).numericalFactor().equals(ExprHandle::integer(1)));
    auto term = (ExprHandle::integer(3) * x * y).term();
    ASSERT_TRUE(term.has_value());
    EXPECT_TRUE(term->equals(x * y));
}

TEST(SymbolicApi, FactorsAndExponentsOfProduct)
{
    auto x    = ExprHandle::variable("x");
    auto y    = ExprHandle::variable("y");
    auto z    = ExprHandle::variable("z");
    auto expr = csl::api::power(x, ExprHandle::integer(2)) * y;
    EXPECT_EQ(expr.factors().size(), 3u);
    auto exponents = expr.exponents({x, y, z});
    ASSERT_EQ(exponents.size(), 3u);
    EXPECT_TRUE(exponents[0].equals(ExprHandle::integer(2)));
    EXPECT_TRUE(exponents[1].equals(ExprHandle::integer(1)));
    EXPECT_TRUE(exponents[2].equals(ExprHandle::integer(0)));
}

TEST(SymbolicApi, ValueOfNumberAndValuedFlag)
{
    auto x = ExprHandle::variable("x");
    EXPECT_TRUE(ExprHandle::integer(3).valued());
    EXPECT_DOUBLE_EQ(ExprHandle::integer(3).value(), 3.0);
    EXPECT_FALSE(x.valued());
}

TEST(SymbolicApi, NumeratorAndDenominatorOfIntegersAndRationals)
{
    EXPECT_EQ(ExprHandle::rational(3, 4).numerator(), 3);
    EXPECT_EQ(ExprHandle::rational(3, 4).denominator(), 4);
    EXPECT_EQ(ExprHandle::integer(5).numerator(), 5);
    EXPECT_EQ(ExprHandle::integer(5).denominator(), 1);
}

TEST(SymbolicApi, NumeratorOfNonRationalThrows)
{
    EXPECT_THROW(static_cast<void>(ExprHandle::real(1.5).numerator()),
                 csl::api::TypeError);
    EXPECT_THROW(static_cast<void>(ExprHandle::variable("x").denominator()),
                 csl::api::TypeError);
}

TEST(SymbolicApi, PredicatesOfNumbersAndSymbols)
{
    auto x = ExprHandle::variable("x");
    EXPECT_TRUE(ExprHandle::integer(3).isInteger());
    EXPECT_TRUE(ExprHandle::integer(3).isReal());
    EXPECT_FALSE(ExprHandle::rational(1, 2).isInteger());
    EXPECT_TRUE(ExprHandle::real(1.5).isReal());
    EXPECT_FALSE(ExprHandle::real(1.5).isInteger());
    EXPECT_FALSE(ExprHandle::integer(3).isIndexed());
    EXPECT_FALSE(ExprHandle::integer(3).isAnOperator());
    EXPECT_FALSE(x.isAnOperator());
    EXPECT_FALSE(x.isIndexed());
}

TEST(SymbolicApi, PredicatesOfOperators)
{
    auto z = ExprHandle::variable("z", csl::ComplexProperty::Complex);
    EXPECT_TRUE(conjugated(z).isComplexConjugate());
    EXPECT_FALSE(z.isComplexConjugate());
    EXPECT_TRUE(realPart(z).isAnOperator());
    EXPECT_FALSE(z.isAnOperator());
}

TEST(SymbolicApi, ValueOfSymbolThrows)
{
    auto x = ExprHandle::variable("x");
    EXPECT_THROW(static_cast<void>(x.value()), csl::api::TypeError);
}

TEST(SymbolicApi, TypeAndPrimaryTypeOfExpressions)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    EXPECT_EQ(x.type(), csl::Type::Variable);
    EXPECT_EQ(x.primaryType(), csl::PrimaryType::Literal);
    EXPECT_EQ((x + y).type(), csl::Type::Sum);
}

TEST(SymbolicApi, ShapeOfScalarAndLatexName)
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    EXPECT_EQ(x.shape(), std::vector<int>{1});
    EXPECT_EQ(x.latexName(), "x");
    EXPECT_EQ((x + y).latexName(), "");
}
