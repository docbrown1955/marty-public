#include <csl.h>
#include <gtest/gtest.h>

template <typename Call>
static void expectCSLError(Call call, CSLError expected)
{
    bool thrown = false;
    try {
        call();
    }
    catch (CSLError error) {
        thrown = true;
        EXPECT_EQ(error, expected);
    }
    EXPECT_TRUE(thrown);
}

TEST(csl_pow_special, PowInfinityBaseThrows)
{
    expectCSLError([] { csl::pow_s(CSL_INF, CSL_2); }, CSLError::ValueError);
    expectCSLError([] { csl::pow_s(CSL_INF, CSL_HALF); },
                   CSLError::ValueError);
}

TEST(csl_pow_special, PowZeroBaseNegativeExponentThrows)
{
    expectCSLError([] { csl::pow_s(CSL_0, CSL_M_1); },
                   CSLError::ZeroDivisionError);
    expectCSLError([] { csl::pow_s(CSL_0, CSL_M_HALF); },
                   CSLError::ZeroDivisionError);
}

TEST(csl_pow_special, SqrtPerfectSquare)
{
    ASSERT_EQ(csl::sqrt_s(4), CSL_2);
    ASSERT_EQ(csl::sqrt_s(9), csl::int_s(3));
}

TEST(csl_pow_special, PowPerfectPowerRational)
{
    ASSERT_EQ(csl::pow_s(csl::int_s(4), csl::intfraction_s(3, 2)),
              csl::int_s(8));
    ASSERT_EQ(csl::pow_s(csl::int_s(8), csl::intfraction_s(2, 3)),
              csl::int_s(4));
    ASSERT_EQ(csl::pow_s(csl::int_s(4), CSL_M_HALF), CSL_HALF);
}

TEST(csl_pow_special, PowNonPerfectUnchanged)
{
    csl::Expr root = csl::sqrt_s(2);
    ASSERT_EQ(root->getType(), csl::Type::Pow);
    ASSERT_EQ(root->getArgument(0), CSL_2);
    ASSERT_EQ(root->getArgument(1), CSL_HALF);
}

TEST(csl_pow_special, PowNegativeBaseUnchanged)
{
    csl::Expr root = csl::pow_s(csl::int_s(-4), CSL_HALF);
    ASSERT_EQ(root->getType(), csl::Type::Pow);
    ASSERT_EQ(root->getArgument(0), csl::int_s(-4));
    ASSERT_EQ(root->getArgument(1), CSL_HALF);
}
