#include <csl.h>
#include <gtest/gtest.h>

TEST(csl_comparison, numbers)
{
    EXPECT_EQ(csl::int_s(3), 3);
    EXPECT_EQ(csl::intfraction_s(3, 2), csl::intfraction_s(6, 4));
    EXPECT_NE(csl::int_s(3), 2);
    EXPECT_NE(csl::intfraction_s(3, 2), csl::intfraction_s(5, 4));
    EXPECT_DOUBLE_EQ(csl::intfraction_s(7, 3)->evaluateScalar(), 7. / 3);
}

TEST(csl_comparison, variables)
{
    csl::Expr a = csl::constant_s("a");
    csl::Expr b = csl::constant_s("b");
    csl::Expr x = csl::variable_s("x");
    csl::Expr y = csl::variable_s("y");
    EXPECT_EQ(a, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(x, x);
    EXPECT_NE(x, y);
    EXPECT_NE(a, csl::constant_s("a"));
    EXPECT_NE(x, csl::variable_s("x"));
}
