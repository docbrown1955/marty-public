#include "csltestdata.h"
#include <csl.h>
#include <gtest/gtest.h>

TEST(csl_numbers, integers)
{
    ASSERT_EQ(static_cast<int>(csl::int_s(1)->evaluateScalar()), 1);
    ASSERT_EQ(static_cast<int>(csl::int_s(2)->evaluateScalar()), 2);
    ASSERT_EQ(static_cast<int>(csl::int_s(-100000)->evaluateScalar()),
              -100000);
    ASSERT_TRUE(csl::int_s(0)->isInteger());
    ASSERT_TRUE(csl::IsInteger(csl::int_s(1)));
    ASSERT_TRUE(csl::int_s(0)->getType() == csl::Type::Integer);
    ASSERT_TRUE(csl::int_s(0)->getPrimaryType()
                == csl::PrimaryType::Numerical);
}
TEST(csl_numbers, integer_constants)
{
    ASSERT_EQ(static_cast<int>(CSL_0->evaluateScalar()), 0);
    ASSERT_EQ(static_cast<int>(CSL_1->evaluateScalar()), 1);
    ASSERT_EQ(static_cast<int>(CSL_2->evaluateScalar()), 2);
    ASSERT_EQ(static_cast<int>(CSL_M_1->evaluateScalar()), -1);
    ASSERT_EQ(static_cast<int>(CSL_M_2->evaluateScalar()), -2);
}
TEST(csl_numbers, integers_complex)
{
    ASSERT_TRUE(CSL_2->isReal());
    ASSERT_FALSE(CSL_2->isPurelyImaginary());
    ASSERT_EQ(static_cast<int>(csl::GetRealPart(CSL_2)->evaluateScalar()), 2);
    ASSERT_EQ(static_cast<int>(csl::GetImaginaryPart(CSL_2)->evaluateScalar()),
              0);
    ASSERT_EQ(
        static_cast<int>(csl::GetComplexModulus(CSL_2)->evaluateScalar()), 2);
    ASSERT_EQ(
        static_cast<int>(csl::GetComplexArgument(CSL_2)->evaluateScalar()), 0);
    ASSERT_EQ(
        static_cast<int>(csl::GetComplexConjugate(CSL_2)->evaluateScalar()),
        2);
}

TEST(csl_numbers, floats)
{
    ASSERT_DOUBLE_EQ(csl::float_s(3.5)->evaluateScalar(), 3.5);
    ASSERT_DOUBLE_EQ(csl::float_s(-32545.3)->evaluateScalar(), -32545.3);
    ASSERT_DOUBLE_EQ(csl::float_s(.7e65L)->evaluateScalar(), .7e65);
    ASSERT_FALSE(csl::float_s(0.5)->isInteger());
    ASSERT_TRUE(csl::float_s(1.)->isInteger());
    ASSERT_TRUE(csl::IsFloat(csl::float_s(0.5)));
    ASSERT_TRUE(csl::float_s(0.5)->getType() == csl::Type::Float);
    ASSERT_TRUE(csl::float_s(0.5)->getPrimaryType()
                == csl::PrimaryType::Numerical);
}
TEST(csl_numbers, float_constants)
{
    ASSERT_FLOAT_EQ(CSL_PI->evaluateScalar(), M_PI);
    ASSERT_FLOAT_EQ(CSL_E->evaluateScalar(), M_E);
}
TEST(csl_numbers, floats_complex)
{
    ASSERT_TRUE(CSL_PI->isReal());
    ASSERT_FALSE(CSL_PI->isPurelyImaginary());
    ASSERT_FLOAT_EQ(csl::GetRealPart(CSL_PI)->evaluateScalar(), M_PI);
    ASSERT_FLOAT_EQ(csl::GetImaginaryPart(CSL_PI)->evaluateScalar(), 0);
    ASSERT_FLOAT_EQ(csl::GetComplexModulus(CSL_PI)->evaluateScalar(), M_PI);
    ASSERT_FLOAT_EQ(csl::GetComplexArgument(CSL_PI)->evaluateScalar(), 0);
    ASSERT_FLOAT_EQ(csl::GetComplexConjugate(CSL_PI)->evaluateScalar(), M_PI);
}

TEST(csl_numbers, rationals)
{
    csl::Expr f1 = csl::intfraction_s(3, 5);
    csl::Expr f2 = csl::intfraction_s(3, 6);
    csl::Expr f3 = csl::intfraction_s(6, 3);

    ASSERT_EQ(f1->getType(), csl::Type::IntFraction);
    ASSERT_EQ(f1->getNum(), 3);
    ASSERT_EQ(f1->getDenom(), 5);
    ASSERT_EQ(f2->getType(), csl::Type::IntFraction);
    ASSERT_EQ(f2->getNum(), 1);
    ASSERT_EQ(f2->getDenom(), 2);
    ASSERT_EQ(f3->getType(), csl::Type::Integer);
    ASSERT_EQ(static_cast<int>(f3->evaluateScalar()), 2);

    ASSERT_EQ(f1->getPrimaryType(), csl::PrimaryType::Numerical);
    ASSERT_TRUE(csl::IsIntFraction(f1));
    ASSERT_FALSE(csl::IsIntFraction(f3));
}
TEST(csl_numbers, rational_constants)
{
    ASSERT_EQ(CSL_HALF->getNum(), 1);
    ASSERT_EQ(CSL_HALF->getDenom(), 2);
    ASSERT_EQ(CSL_M_HALF->getNum(), -1);
    ASSERT_EQ(CSL_M_HALF->getDenom(), 2);
    ASSERT_EQ(CSL_THIRD->getNum(), 1);
    ASSERT_EQ(CSL_THIRD->getDenom(), 3);
    ASSERT_EQ(CSL_M_THIRD->getNum(), -1);
    ASSERT_EQ(CSL_M_THIRD->getDenom(), 3);
}
TEST(csl_numbers, rationals_complex)
{
    ASSERT_TRUE(CSL_HALF->isReal());
    ASSERT_FALSE(CSL_HALF->isPurelyImaginary());
    ASSERT_FLOAT_EQ(csl::GetRealPart(CSL_HALF)->evaluateScalar(), 0.5);
    ASSERT_FLOAT_EQ(csl::GetImaginaryPart(CSL_HALF)->evaluateScalar(), 0);
    ASSERT_FLOAT_EQ(csl::GetComplexModulus(CSL_HALF)->evaluateScalar(), 0.5);
    ASSERT_FLOAT_EQ(csl::GetComplexArgument(CSL_HALF)->evaluateScalar(), 0);
    ASSERT_FLOAT_EQ(csl::GetComplexConjugate(CSL_HALF)->evaluateScalar(), 0.5);
}

TEST(csl_numbers, complex)
{
    ASSERT_EQ(static_cast<int>(
                  csl::GetRealPart(csl::complex_s(1, 0))->evaluateScalar()),
              1);
    ASSERT_EQ(
        static_cast<int>(
            csl::complex_s(2, 4)->getRealPart().value()->evaluateScalar()),
        2);
    ASSERT_EQ(static_cast<int>(
                  csl::complex_s(2, 4)->getImaginaryPart()->evaluateScalar()),
              4);
    ASSERT_EQ(static_cast<int>(
                  csl::complex_s(0, 5)->getImaginaryPart()->evaluateScalar()),
              5);
    ASSERT_TRUE(csl::complex_s(1, 0)->isReal());
    ASSERT_TRUE(csl::complex_s(0, 1)->isPurelyImaginary());
    ASSERT_FALSE(csl::complex_s(1, 1)->isReal());
    ASSERT_FALSE(csl::complex_s(1, 1)->isPurelyImaginary());
    ASSERT_EQ(csl::complex_s(1, 1)->getType(), csl::Type::Complex);
    ASSERT_EQ(csl::complex_s(0, 1)->getType(), csl::Type::Complex);
    ASSERT_EQ(csl::complex_s(1, 0)->getType(), csl::Type::Integer);
    ASSERT_EQ(csl::complex_s(1, 1)->getPrimaryType(),
              csl::PrimaryType::Numerical);
    ASSERT_TRUE(csl::IsComplex(csl::complex_s(1, 1)));

    ASSERT_FLOAT_EQ(
        csl::complex_s(1, 1)->getComplexModulus().value()->evaluateScalar(),
        std::sqrt(2));
    ASSERT_EQ(static_cast<int>(csl::complex_s(0, 1)
                                   ->getComplexModulus()
                                   .value()
                                   ->evaluateScalar()),
              1);
    ASSERT_EQ(
        static_cast<int>(
            csl::GetComplexModulus(csl::complex_s(1, 0))->evaluateScalar()),
        1);
    ASSERT_EQ(
        static_cast<int>(
            csl::GetComplexModulus(csl::complex_s(0, 0))->evaluateScalar()),
        0);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::complex_s(0, 1))->evaluateScalar(),
        M_PI / 2.);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::complex_s(1, 1))->evaluateScalar(),
        M_PI / 4.);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::complex_s(1, 0))->evaluateScalar(), 0);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::complex_s(-1, 1))->evaluateScalar(),
        3 * M_PI / 4.);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::complex_s(-1, 0))->evaluateScalar(),
        M_PI);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::complex_s(0, -1))->evaluateScalar(),
        -M_PI / 2.);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::make_shared<csl::Complex>(1, 0))
            ->evaluateScalar(),
        0);
    ASSERT_FLOAT_EQ(
        csl::GetComplexArgument(csl::make_shared<csl::Complex>(-1, 0))
            ->evaluateScalar(),
        M_PI);
    ASSERT_FLOAT_EQ(
        csl::GetComplexConjugate(csl::make_shared<csl::Complex>(-1, 0))
            ->evaluateScalar(),
        -1);
    // ASSERT_THROW(csl::complex_s(1, 1)->evaluateScalar(), CSLError);

    ASSERT_EQ(csl::GetComplexConjugate(csl::complex_s(0, 1)),
              csl::complex_s(0, -1));
    ASSERT_EQ(csl::GetComplexConjugate(csl::complex_s(0, 0)),
              csl::complex_s(0, 0));
    ASSERT_EQ(csl::GetComplexConjugate(csl::complex_s(1, 0)),
              csl::complex_s(1, 0));
    ASSERT_EQ(csl::GetComplexConjugate(csl::complex_s(1, 1)),
              csl::complex_s(1, -1));
}

TEST(csl_numbers, evaluation)
{
    ASSERT_FLOAT_EQ(csl::int_s(2)->evaluateScalar(), 2);
    ASSERT_FLOAT_EQ(csl::float_s(5.5)->evaluateScalar(), 5.5);
    ASSERT_FLOAT_EQ(csl::intfraction_s(6, 14)->evaluateScalar(), 3. / 7);
    ASSERT_EQ(csl::Evaluated(csl::int_s(3), csl::eval::all), csl::int_s(3));
    ASSERT_EQ(csl::Evaluated(csl::float_s(3.4), csl::eval::all),
              csl::float_s(3.4));
    ASSERT_FLOAT_EQ(csl::Evaluated(csl::intfraction_s(1, 2), csl::eval::all)
                        ->evaluateScalar(),
                    csl::float_s(1. / 2)->evaluateScalar());
}

TEST(csl_numbers, expr_conv)
{
    ASSERT_EQ(csl::Expr(nullptr), csl::Expr{});
    ASSERT_EQ(csl::Expr(0), CSL_0);
    ASSERT_EQ(csl::Expr(1), CSL_1);
    ASSERT_EQ(csl::Expr(2), CSL_2);
    ASSERT_FLOAT_EQ(csl::Expr(0.5)->evaluateScalar(),
                    csl::float_s(0.5)->evaluateScalar());
    ASSERT_EQ(csl::Expr(1LL), csl::int_s(1LL));
}

TEST(csl_numbers, print)
{
    auto int_      = csl::int_s(5);
    auto float_    = csl::float_s(2.5);
    auto rational_ = csl::intfraction_s(1, 2);
    auto complex_  = csl::complex_s(1, 2);
    PRINT_EXPR(int_)
    PRINT_EXPR(float_)
    PRINT_EXPR(rational_)
    PRINT_EXPR(complex_)
}

TEST(csl_numbers, operations)
{
    ASSERT_EQ(csl::intfraction_s(2, 4) * csl::complex_s(1, 1)
                  - csl::complex_s(0, csl::intfraction_s(1, 2)),
              csl::intfraction_s(15423, 30846));
    ASSERT_EQ(csl::pow_s(4, csl::intfraction_s(1, 2)), CSL_2);
    ASSERT_EQ(csl::pow_s(9, csl::intfraction_s(1, 2)), 1 + CSL_2);
}
