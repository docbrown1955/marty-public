#include "csltestdata.h"
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
    using namespace data;
    EXPECT_EQ(a, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(x, x);
    EXPECT_NE(x, y);
    EXPECT_NE(a, csl::constant_s("a"));
    EXPECT_NE(x, csl::variable_s("x"));
}

TEST(csl_comparison, sum_and_prod)
{
    using namespace data;
    EXPECT_EQ(2 + a + 1, csl::sum_s({a, 3}));
    EXPECT_NE(2 + a + 1, csl::sum_s({a, 2}));
    EXPECT_EQ(a + b + 2 * x, 3 * x + b - x + a - 0);
    EXPECT_NE(a + b + 2 * x, 4 * x + b - x + a - 0);

    EXPECT_EQ(csl::prod_s({2, x, x * x, y}) / y, 2 * csl::pow_s(x, 3));
    EXPECT_NE(csl::prod_s({2, x, x * x, y}) / y * y, 2 * csl::pow_s(x, 3));
}

TEST(csl_comparison, indices)
{
    using namespace data;
    EXPECT_EQ(i1, i1);
    EXPECT_NE(i1, i2);
    EXPECT_EQ(i1, +i1);

    EXPECT_EQ(mu1, mu1);
    EXPECT_NE(mu1, mu2);
    EXPECT_EQ(mu1, +mu1);
}

TEST(csl_comparison, simple_tensors)
{
    using namespace data;
    EXPECT_EQ(T22({i1, i2}), T22({i1, i2}));
    EXPECT_NE(T22({i1, i2}), T22({i1, i1}));
    EXPECT_NE(T22({i1, i2}), T22({i2, i1}));

    EXPECT_NE(T22({i1, i2}), T33({I1, I2}));
    EXPECT_NE(T22({i1, i2}), T33({I1, I1}));
    EXPECT_NE(T22({i1, i2}), T33({I2, I1}));

    // EXPECT_THROW(T22({i1, I2}), CSLError);
}
